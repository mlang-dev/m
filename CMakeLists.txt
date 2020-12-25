cmake_minimum_required(VERSION 3.16)

# set any CMAKE_ variables here
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

project(m, VERSION 0.0.9 LANGUAGES CXX C)

if(DEFINED ENV{MPATH})
    message(STATUS "FOUND MPATH pointing to $ENV{MPATH}")
else()
    message(STATUS "missing MPATH environment variable !")
    if(WIN32)
        message(FATAL_ERROR "Please set MPATH environment variable to the m src folder. for example: \"set MPATH=c:\\dev\\m\\src\"")
    else()
        message(FATAL_ERROR "Please set MPATH environment variable to the m src folder. for example: \"export MPATH=~/dev/m/src\"")
    endif(WIN32)
endif()

add_subdirectory(extern/googletest)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(samples)

