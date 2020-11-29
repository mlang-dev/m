cmake_minimum_required(VERSION 3.16)

project(msample, VERSION 0.0.9 LANGUAGES CXX)

link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(msample
  sample_main.cc
)

add_custom_command(
  TARGET msample
  PRE_BUILD
  COMMAND m ${CMAKE_CURRENT_SOURCE_DIR}/sample_lib.m
  COMMAND m ${CMAKE_CURRENT_SOURCE_DIR}/sample.m
  # MacOS COMMAND ld ${CMAKE_CURRENT_SOURCE_DIR}/sample.o ${CMAKE_CURRENT_SOURCE_DIR}/../build/src/libmlr.a /usr/lib/libc.dylib -o ${CMAKE_CURRENT_SOURCE_DIR}/sample
  COMMAND ld ${CMAKE_CURRENT_SOURCE_DIR}/sample.o ${CMAKE_CURRENT_SOURCE_DIR}/../build/src/libmlr.a /usr/lib/x86_64-linux-gnu/libc.so -o ${CMAKE_CURRENT_SOURCE_DIR}/sample
)

TARGET_LINK_LIBRARIES(msample PRIVATE mlr ${CMAKE_CURRENT_SOURCE_DIR}/sample_lib.o)

add_custom_command(
  TARGET msample
  POST_BUILD
  COMMAND msample
)


