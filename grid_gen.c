#include <stdio.h>
#include <stdlib.h>

double** GenerateGrid(int size) {
  double** grid = calloc(size * size, sizeof(double*));

  for (int i = 0; i < size; i++) {
    grid[i] = calloc(size, sizeof(double));
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == 0 || i == (size - 1) || j == 0 || j == (size - 1)) {
        grid[i][j] = 100.00f;
      }
    }
  }
  return grid;
}
