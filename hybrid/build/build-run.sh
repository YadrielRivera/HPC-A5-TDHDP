# Compile
mpicc ../src/main.c -fopenmp -o hybrid_e
# Run
mpirun -np 2 ./hybrid_e 10