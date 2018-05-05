cmake_minimum_required(VERSION 3.9)
project(PID)

set(CMAKE_CXX_STANDARD 11)

file(GLOB pid_folder
        ../PID/*.h
        ../PID/*.cpp
    )

file(GLOB inc_folder
        ../include/*.h
        ../include/*.c
        ../include/*.cpp
        )

include_directories(../include/)

add_executable(PID
        ${pid_folder}
        ${inc_folder}
        )