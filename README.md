# A5: Hybrid Parallel Programming

## Background
This assignment consists of the implementation of a sequential, MPI-only, and MPI + OpenMP of the Two Dimensional Heat Diffusion Problem (TDHDP)

## Requirements
* CMake 3.10+
* MPI
* OpenMP
* (optional although preferable) A Linux-based OS or WSL if running the `test suite`, it uses POSIX syscalls

## General Information

### Hybrid
This is the OpenMP + MPI implementation of the TDHDF

### Sequential
Implementation of TDHDF without any parallel programming tools

### MPI
Message Passing Interface exclusive implementation of the TDHF

## Test Suite
Environment for data collection; that is: execution times for the different implementations. It will use POSIX fork() and execlp() to run each implementation and gather execution times.

## grid_gen.c
Generates an N by N grid with the edges of the grid set to 100.00

**Note that for the test suite to execute, all the other projects must be compiled, built, and with an executable file available**

## Compile and Run (Non-MPI)
The following instructions are relevant for the following projects (non-mpi):
* `sequential`
* `test_suite`

### Building (Linux)
From directory H5

```bash
cd <desired implementation>
mkdir build
cd build
cmake -S .. -B .
```

### Compiling & Running (Linux)
From the build directory of the desired implementation:
```bash
make
./<executable name>
```
### Compiling & Running (Windows)
From the build directory of the desired implementation:
```bash
gcc ../src/main.c -o <executable-name>
```

Executable names include:
* `sequential_e`
* `test`

## Compile & Run (MPI)
From the desired implementation directory (hybrid or mpi):
```bash
cd build
chmod +x build-run.sh
./build-run.sh
```