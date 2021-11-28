cmake_minimum_required(VERSION 3.16)

add_library(unity STATIC unity.c)

target_include_directories(unity PUBLIC unity)
