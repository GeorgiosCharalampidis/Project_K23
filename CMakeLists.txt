cmake_minimum_required(VERSION 3.26)
project(ProjectEm)

set(CMAKE_CXX_STANDARD 17)

add_executable(ProjectEm main.cpp minst.cpp
        minst.h
        lsh.h
        lsh.cpp
        Hypercube.cpp
        Hypercube.h
        global_functions.cpp
        global_functions.h
        main_helper.cpp
        main_helper.h
        KMeansPLusPlus.cpp
        KMeansPLusPlus.h
        HashFunction.cpp
        HashFunction.h
)
