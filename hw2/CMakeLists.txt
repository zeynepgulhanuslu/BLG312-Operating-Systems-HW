cmake_minimum_required(VERSION 3.16)
project(hw2 C)

set(CMAKE_C_COMPILER gcc)

set(CMAKE_C_FLAGS "-Wall -Wextra -pthread -lm -Werror=implicit-function-declaration -lrt")

add_executable(customer.h multi_thread.c)
add_executable(multi_process customer.h multi_process.c)
