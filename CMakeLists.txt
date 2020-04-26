cmake_minimum_required(VERSION 3.16)

project(m, VERSION 0.0.9 LANGUAGES CXX C)

# set any CMAKE_ variables here
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_subdirectory(extern/googletest)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(samples)

