cmake_minimum_required(VERSION 3.16)

# set any CMAKE_ variables here
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)


project(m, VERSION 0.0.9 LANGUAGES CXX C)

if(WASM)

message(STATUS "building for wasm")

set(CMAKE_AR "llvm-ar")
set(CMAKE_RANLIB "llvm-ranlib")
add_compile_definitions(__wasi__=1)
add_compile_definitions(__wasm32__=1)
add_compile_definitions(UNITY_EXCLUDE_SETJMP_H=1)
add_compile_options(
                      "--target=wasm32"
                      "--sysroot=/usr/lib/wasi-libc"
                      "-nostdlib"
                      "-o3"  # aggressive optimization
                      "-flto" # add meta data for link time optimization
                      "-fvisibility=hidden"
                      "-g"
                      ) 
endif()


if (MSVC)
    # warning level 4 and all warnings as errors
    # add_compile_options(/W4 /WX /Wv:18)
    # add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(EMSCRIPTEN)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
    endif ()    
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_compile_options(-Wno-null-pointer-arithmetic)
endif()


add_subdirectory(lib)
add_subdirectory(apps)

if(NOT WASM)
    add_subdirectory(extern/googletest)
    add_subdirectory(test)
endif()

if (NOT EMSCRIPTEN AND NOT WASM)
    add_subdirectory(samples)
endif()

