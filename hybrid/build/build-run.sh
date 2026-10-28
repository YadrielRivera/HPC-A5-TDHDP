# Compile
mpicc ../src/main.c -fopenmp -o hybrid_e

export OMP_NUM_THREADS=4
# Run
mpirun -np 2 ./hybrid_e 10