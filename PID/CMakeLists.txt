cmake_minimum_required(VERSION 3.9)
project(PID)

set(CMAKE_CXX_STANDARD 11)

add_executable(PID
        pid_main.c
        pid_source.c
        pid_source.h
        setpoint.h
        structexample.cpp)

