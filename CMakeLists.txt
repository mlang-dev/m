cmake_minimum_required(VERSION 3.16)

# set any CMAKE_ variables here
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(HAVE_FLAG_SEARCH_PATHS_FIRST 0)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_LINK_FLAGS "")

project(m, VERSION 0.0.9 LANGUAGES CXX C)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/test/unity
    ${CMAKE_SOURCE_DIR}/test
)

set(CMAKE_AR "llvm-ar")
set(CMAKE_RANLIB "llvm-ranlib")

add_library(wasm_options INTERFACE)

target_compile_definitions(wasm_options INTERFACE 
__wasi__=1
__wasm32__=1
WASM=1
UNITY_EXCLUDE_SETJMP_H=1)

target_compile_options(wasm_options INTERFACE
                      --target=wasm32
                      --sysroot=${CMAKE_SOURCE_DIR}/extern/wasi-libc/sysroot
                      -nostdlib
                      # -o3  # aggressive optimization
                      # -flto # add meta data for link time optimization
                      -fvisibility=hidden
                      -g
                      ) 

if (MSVC)
    # warning level 4 and all warnings as errors
    # add_compile_options(/W4 /WX /Wv:18)
    # add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unused-parameter # -Wno-error=unused-command-line-argument
    -Wno-missing-braces -Wno-enum-conversion -g)
endif()

add_subdirectory(extern/googletest)
add_subdirectory(lib)
add_subdirectory(apps)
add_subdirectory(test)
add_subdirectory(samples)

