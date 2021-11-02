cmake_minimum_required(VERSION 3.16)

# set any CMAKE_ variables here
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

project(m, VERSION 0.0.9 LANGUAGES CXX C)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(extern/googletest)
add_subdirectory(lib)
add_subdirectory(apps)
add_subdirectory(test)
add_subdirectory(samples)

