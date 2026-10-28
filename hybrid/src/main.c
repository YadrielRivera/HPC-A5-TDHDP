#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../grid_gen.c"

#define ALPHA 0.1
#define BORDER_VALUE 100.0

void PrintDistributedGrid(double** old_partition, int workload, int N,
                          int curr_rank, int thread_count) {
  double* send_buf = (double*)malloc(workload * N * sizeof(double));
  for (int i = 0; i < workload; i++) {
    for (int j = 0; j < N; j++) {
      send_buf[i * N + j] = old_partition[i + 1][j];
    }
  }

  // Allocate full grid buffer only on Root (Rank 0)
  double* full_grid = NULL;
  if (curr_rank == 0) {
    full_grid = (double*)malloc((size_t)N * N * sizeof(double));
  }

  // Gather chunks from all processes into full_grid on root
  MPI_Gather(send_buf, workload * N, MPI_DOUBLE, full_grid, workload * N,
             MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Only Rank 0 prints the reconstructed matrix
  if (curr_rank == 0) {
    printf("\n--- Reconstructed Grid (%dx%d) ---\n", N, N);
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        printf("%6.2f ", full_grid[i * N + j]);
      }
      printf("\n");
    }
    printf("----------------------------------\n\n");
    free(full_grid);
  }

  free(send_buf);
}

void Simulation(double** old_partition, double** new_partition, int N,
                int total_steps, int curr_rank, int thread_count) {
  int workload = N / thread_count;

  int up_neighbor = (curr_rank > 0) ? curr_rank - 1 : MPI_PROC_NULL;
  int down_neighbor =
      (curr_rank < thread_count - 1) ? curr_rank + 1 : MPI_PROC_NULL;

  for (int step = 0; step < total_steps; step++) {
    // Halo exchange
    // Whatever we send in one direction, we receive from the opposite one.

    // Send first owned row UP, receive bottom ghost row from the rank BELOW
    MPI_Sendrecv(&old_partition[1][0], N, MPI_DOUBLE, up_neighbor, 0,
                 &old_partition[workload + 1][0], N, MPI_DOUBLE, down_neighbor,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Send last owned row DOWN, receive top ghost row from the rank ABOVE
    MPI_Sendrecv(&old_partition[workload][0], N, MPI_DOUBLE, down_neighbor, 1,
                 &old_partition[0][0], N, MPI_DOUBLE, up_neighbor, 1,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

// Apply formula on owned rows using OMP threads
#pragma omp parallel for
    for (int i = 1; i <= workload; i++) {
      int global_row = (curr_rank * workload) + (i - 1);

      for (int j = 0; j < N; j++) {
        // Preserve the absolute global boundaries of the N x N grid
        if ((global_row == 0) || (global_row == (N - 1)) || (j == 0) ||
            (j == (N - 1))) {
          new_partition[i][j] = old_partition[i][j];
          continue;
        }

        new_partition[i][j] =
            old_partition[i][j] +
            ALPHA * (old_partition[i - 1][j] + old_partition[i + 1][j] +
                     old_partition[i][j - 1] + old_partition[i][j + 1] -
                     4.0 * old_partition[i][j]);
      }
    }

// Copy new partition back to old partition for the next time step
#pragma omp parallel for
    for (int i = 1; i <= workload; i++) {
      memcpy(old_partition[i], new_partition[i], N * sizeof(double));
    }
  }
}

int main(int argc, char* argv[]) {
  int curr_rank, world_size;
  int prov;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &prov);
  MPI_Comm_rank(MPI_COMM_WORLD, &curr_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int N = 8;
  int steps = 500;

  if (argc > 1) {
    N = atoi(argv[1]);

    if (argc == 3) {
      omp_set_num_threads(atoi(argv[2]));
    }
  }

  if (N % world_size != 0) {
    if (curr_rank == 0) {
      printf("Grid size (%d) must be divisible by processes (%d)\n", N,
             world_size);
    }
    MPI_Finalize();
    return 1;
  }

  int workload = N / world_size;
  int total_local_rows = workload + 2;  // +2 for top and bottom ghost rows

  // Allocate flat memory with space for ghost rows
  double* old_flat =
      (double*)calloc((size_t)total_local_rows * N, sizeof(double));
  double* new_flat =
      (double*)calloc((size_t)total_local_rows * N, sizeof(double));
  double** old_partition = (double**)malloc(total_local_rows * sizeof(double*));
  double** new_partition = (double**)malloc(total_local_rows * sizeof(double*));

  for (int i = 0; i < total_local_rows; i++) {
    old_partition[i] = &old_flat[i * N];
    new_partition[i] = &new_flat[i * N];
  }

  // Initialize owned rows (index 1 to workload) to match the sequential grid:
  // 100 on the global border, 0 in the interior.
  for (int i = 1; i <= workload; i++) {
    int global_row = (curr_rank * workload) + (i - 1);
    for (int j = 0; j < N; j++) {
      if (global_row == 0 || global_row == N - 1 || j == 0 || j == N - 1) {
        old_partition[i][j] = BORDER_VALUE;
      } else {
        old_partition[i][j] = 0.0;
      }
      new_partition[i][j] = old_partition[i][j];
    }
  }

  // Ghost rows on the outermost ranks are never filled by MPI; set them to the
  // border value for safety (the stencil never reads them anyway).
  if (curr_rank == 0) {
    for (int j = 0; j < N; j++) old_partition[0][j] = BORDER_VALUE;
  }
  if (curr_rank == world_size - 1) {
    for (int j = 0; j < N; j++) old_partition[workload + 1][j] = BORDER_VALUE;
  }

  MPI_Barrier(MPI_COMM_WORLD);  // start timing together
  double time_s = MPI_Wtime();

  Simulation(old_partition, new_partition, N, steps, curr_rank, world_size);

  MPI_Barrier(MPI_COMM_WORLD);  // wait for the slowest rank
  double time_e = MPI_Wtime();

  // PrintDistributedGrid(old_partition, workload, N, curr_rank, world_size);
  if (curr_rank == 0) {
    printf("This is the MPI + OpenMP implementation!\n");
    printf(
        "[HYBRID]: Took %.12f seconds for %d steps on %dx%d grid across %d "
        "processes and "
        "%d OpenMP threads\n\n",
        (time_e - time_s), steps, N, N, world_size, omp_get_max_threads());
  }

  free(old_flat);
  free(new_flat);
  free(old_partition);
  free(new_partition);

  MPI_Finalize();
  return 0;
}