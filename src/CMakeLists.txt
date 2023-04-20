cmake_minimum_required(VERSION 3.16)

project(mlang)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
enable_language(M)
add_library(mlanglib OBJECT 
    lib/array.m
    sys/stdio.m
    sys/math.m
)
