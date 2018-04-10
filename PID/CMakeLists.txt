cmake_minimum_required(VERSION 3.9)
project(PID)

set(CMAKE_CXX_STANDARD 11)

add_executable(PID
        pid2d_main.c
        pid_source.c
        ../include/pid_source.h
        ../include/pid_params.h
        )