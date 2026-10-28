#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../grid_gen.c"

#define ALPHA 0.1

void PrintGrid(double** grid, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      printf("%.2f ", grid[i][j]);
    }
    printf("\n");
  }
}

/** This function applies the mathematical five-point
 * stencil, also called Time Stepping.
 *
 * It approximates the 2D laplacian operator
 * */
double** DiffuseHeat(double** old_grid, double** new_grid, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      // Checks if we are at any of the square's edges and ignores it
      if ((i == 0) || (i == (size - 1)) || (j == 0) || (j == (size - 1))) {
        continue;
      }
      // Five-point stencil formula
      new_grid[i][j] =
          (double)(old_grid[i][j] +
                   ALPHA * (old_grid[i - 1][j] + old_grid[i + 1][j] +
                            old_grid[i][j - 1] + old_grid[i][j + 1] -
                            4 * old_grid[i][j]));
    }
  }
}

/* Isolated function dedicated to run the simulation.
 * This could have been done in main, but for the sake of readability
 * it was done this way.
 */
void Simulation(double** old_grid, double** new_grid, int N, int total_steps) {
  for (int step = 0; step < total_steps; step++) {
    // printf("step %d\n", step);
    DiffuseHeat(old_grid, new_grid, N);

    // PrintGrid(new_grid, N);
    // printf("\n\n\n");

    // change the old grid
    for (int i = 0; i < N; i++) {
      memcpy(old_grid[i], new_grid[i], N * sizeof(double));
    }
  }
}

int main(int argc, char* argv[]) {
  printf("This is the sequential implementation!\n");

  int N = 8;  // default value for grid size

  // change value to custom argument
  if (argc > 1) {
    N = atoi(argv[1]);
  }

  double** old_grid = GenerateGrid(N);
  double** new_grid = GenerateGrid(N);

  int time_steps = 500;
  struct timespec start, end;
  // Simulation
  timespec_get(&start, TIME_UTC);
  Simulation(old_grid, new_grid, N, time_steps);
  timespec_get(&end, TIME_UTC);

  double elapsed =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf(
      "[SEQUENTIAL]: Took %.12f seconds to simulate %d steps for %dx%d grid.\n",
      elapsed, time_steps, N, N);

  // free the grid
  for (int i = 0; i < N; i++) {
    free(old_grid[i]);
    free(new_grid[i]);
  }
  free(old_grid);
  free(new_grid);
  return 0;
}
