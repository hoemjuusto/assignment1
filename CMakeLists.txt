cmake_minimum_required(VERSION 3.14)
project(assignment1 C)

set(CMAKE_C_STANDARD 99)

add_executable(assignment1 filehandlers.c file_edits.c filehandlers.h file_edits.h testmain.c clean-process.c clean-process.h)