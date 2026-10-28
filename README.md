# A5: Hybrid Parallel Programming

## Background
This assignment consists of the implementation of a sequential, MPI-only, and MPI + OpenMP of the Two Dimensional Heat Diffusion Problem (TDHDP)

## Requirements
* CMake 3.10+
* OpenMPI (OpenRTE 4.1.6)
* OpenMP
* Linux-based Operating System

## General Information

### Sequential
Implementation of TDHDF without any parallel programming tools.

### MPI
Message Passing Interface implementation of the TDHDF.

**Requirement**: the grid size (N) must be divisible by the number of MPI processes (P). This limitation is for simplicity at the moment of partitioning contiguous rows.

### Hybrid
OpenMP + MPI implementation of the TDHDF. The code is very similar as the MPI implementation, it only adds omp parallel for threading directives in the local partition modification and new-grid/old-grid updating.

## Test Suite
Environment for data collection; that is: execution times for the different implementations. It will use POSIX fork() and execlp() to run each implementation and aggregate execution times.

**Note that for the test suite to execute, all the other projects must be built, compiled, and with an executable file available**

## Grid Generator (grid_gen.c)
Generates an NxN grid with the edges of the grid set to 100.00

Example Output (10x10):
```
100.00 100.00 100.00 100.00 100.00 100.00 100.00 100.00 100.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00   0.00 100.00 
100.00 100.00 100.00 100.00 100.00 100.00 100.00 100.00 100.00 100.00 
```

## Build, Compile, and Run (Non-MPI)
The following instructions are relevant for the following projects (non-mpi):
* `sequential`
* `test_suite`

### Sequential Implementation
From root directory

```bash
cd sequential
mkdir build
cd build
cmake -S .. -B .
make
./sequential <grid size>
```

## Test Suite
From root directory

```bash
cd test_suite
mkdir build
cd build
cmake -S .. -B .
make
./test <grid size> <mpi process count> <omp thread count>
```
**Ensure the grid size is divisible by the mpi process count**

## Build, Compile, and Run (MPI)
The following instructions are relevant for the following projects (mpi-dependent):
* `mpi`
* `hybrid`

## MPI Only
From the root directory:
```bash
cd mpi/build/
chmod +x build-run.sh
./build-run.sh
```

If you want to use values different than the default ones in the script:
```bash
cd mpi/build/
mpicc ../src/main.c -o mpi_e
mpirun -np <mpi proc count> ./mpi_e <grid size>
```
**Ensure the grid size is divisible by the mpi process count**

## Hybrid
From the root directory:
```bash
cd hybrid/build/
chmod +x build-run.sh
./build-run.sh
```

If you want to use values different than the default ones in the script:

```bash
cd hybrid/build/
mpicc ../src/main.c -fopenmp -o hybrid_e
mpirun -np <mpi proc count> ./hybrid_e <grid size> <omp thread count>
```
**Ensure the grid size is divisible by the mpi process count**
