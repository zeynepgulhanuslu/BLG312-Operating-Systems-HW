cmake_minimum_required(VERSION 3.16)
project(hw1)

# This line sets the compiler to be used to GCC.
# This is optional as CMake detects and sets the compiler automatically.
# However, it can be useful if you want to specify a specific compiler version.
set(CMAKE_C_COMPILER gcc)

set(CMAKE_C_FLAGS "-Wall -Wextra -std=c99 -Werror=implicit-function-declaration")


# Create an executable target 'q1' that depends on the source file 'q1.c'.
add_executable(q1 q1.c)
# Create an executable target 'q2' that depends on the source file 'q2.c'.
add_executable(q2 q2.c)
add_custom_target(hw1 clean
        COMMAND ${CMAKE_COMMAND} -E remove q1 q2 *.o
        )
