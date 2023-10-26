cmake_minimum_required(VERSION 3.26)
project(ProjectEm)

set(CMAKE_CXX_STANDARD 17)

add_executable(ProjectEm mnist.cpp
        mnist.h
        lsh_class.h
        lsh_class.cpp
        Hypercube.cpp
        Hypercube.h
        global_functions.cpp
        global_functions.h
        KMeansPLusPlus.cpp
        KMeansPLusPlus.h
        mnist.cpp
        cluster.cpp
        cube.cpp
        lsh.cpp
)
