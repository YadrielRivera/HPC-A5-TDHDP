#include <stdio.h>
#include <stdlib.h>

#include "../../grid_gen.c"

int main(int argc, char* argv[]) {
  printf("This is the sequential implementation!\n");
  int N = 5;
  if (argc > 1) {
    N = atoi(argv[1]);
  }

  double** grid = GenerateGrid(N);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%.2f ", grid[i][j]);
    }
    printf("\n");
  }

  for (int i = 0; i < N; i++) {
    free(grid[i]);
  }
  free(grid);
  return 0;
}
