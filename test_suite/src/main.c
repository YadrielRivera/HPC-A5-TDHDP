#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// Executable receives 3 optional arguments (1) grid_size N, (2) mpi proc count,
// (3) omp thread count
int main(int argc, char* argv[]) {
  pid_t seq_pid, mpi_pid, hybrid_pid = -1;

  char* grid_size = "8";
  char* omp_thread_num = "4";
  char* mpi_procs = "2";

  if (argc > 1) {
    grid_size = argv[1];

    if (argc > 2) {
      mpi_procs = argv[2];
    }

    if (argc > 3) {
      omp_thread_num = argv[3];
    }
  }

  seq_pid = fork();
  if (seq_pid == -1) {
    perror("Error forking sequential code.\n");
    return -1;
  }

  if (seq_pid == 0) {
    execlp("../../sequential/build/sequential_e", "sequential_e", grid_size,
           NULL);
    perror("Error executing sequential code. Is the code compiled?");
    return -1;
  }
  wait(NULL);

  mpi_pid = fork();
  if (mpi_pid == -1) {
    perror("Error forking MPI code.\n");
    return -1;
  }

  if (mpi_pid == 0) {
    execlp("mpirun", "mpirun", "-np", mpi_procs, "../../mpi/build/mpi_e",
           grid_size, NULL);
    perror("Error executing MPI code. Is the code compiled?");
    return -1;
  }
  wait(NULL);

  hybrid_pid = fork();
  if (hybrid_pid == -1) {
    perror("Error forking MPI + OpenMP code.\n");
    return -1;
  }

  if (hybrid_pid == 0) {
    setenv("OMP_NUM_THREADS", omp_thread_num, 1);
    execlp("mpirun", "mpirun", "-np", mpi_procs, "../../hybrid/build/hybrid_e",
           grid_size, NULL);
    perror("Error executing MPI + OpenMP code. Is the code compiled?");
    return -1;
  }
  wait(NULL);

  return 0;
}