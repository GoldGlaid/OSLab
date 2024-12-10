cmake_minimum_required(VERSION 3.22)
project(OSLab C)

set(CMAKE_C_STANDARD 23)

add_executable(parent_lab2 lab2/parent.c lab2/functions.h)
add_executable(GenCords lab2/generator_rand_corrds.c)
add_executable(parent_lab3 lab3/parent.c)
