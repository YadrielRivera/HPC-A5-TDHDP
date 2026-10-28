# Compile
mpicc ../src/main.c -o mpi_e
# Run
mpirun -np 2 ./mpi_e 10