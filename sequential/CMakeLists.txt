cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILE_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_COMPILE_FLAGS_RELEASE "-O3 -DNDEBUG")

project(sequential)

add_executable(${CMAKE_PROJECT_NAME} src/main.c)
