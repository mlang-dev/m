cmake_minimum_required(VERSION 3.16)

include(GoogleTest)

find_package(LLVM REQUIRED CONFIG)
execute_process(COMMAND llvm-config --libs
                OUTPUT_VARIABLE llvm_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --system-libs
                OUTPUT_VARIABLE sys_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " -llibxml2.tbd" "" sys_libraries ${sys_libraries})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Found LLVM libraries ${llvm_libraries}")
message(STATUS "Found SYS libraries ${sys_libraries}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


add_library(mlr 
  clib/math.c
  clib/object.c
  clib/array.c
  clib/string.c
  clib/queue.c
  clib/hash.c
  clib/hashset.c
  clib/hashtable.c
  clib/util.c
  clib/generic.c
  tool/cmodule.c
  lexer/lexer.c
  parser/ast.c
  parser/parser.c
  parser/astdump.c
  analyzer/type.c
  analyzer/analyzer.c
  analyzer/env.c
  codegen/codegen.c
  repl/repl.c
  repl/jit.c
  compiler/compiler.c
 )


include_directories(
  ${CMAKE_SOURCE_DIR}/include
  /usr/local/include
)

link_directories(
  /usr/local/lib
)

add_executable(m
  driver/m.c
)

add_executable(c2m
  tool/c2m.c
)

set_target_properties(m PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(m PRIVATE 
  mlr
  ${llvm_libraries}
  ${sys_libraries}
  pthread
  dl
 )

add_custom_command(
  TARGET m
  POST_BUILD
  COMMAND m ${CMAKE_SOURCE_DIR}/samples/sample_lib.m
  COMMAND m ${CMAKE_SOURCE_DIR}/samples/sample.m
  # MacOS COMMAND ld ${CMAKE_CURRENT_SOURCE_DIR}/sample.o ${CMAKE_CURRENT_SOURCE_DIR}/../build/src/libmlr.a /usr/lib/libc.dylib -o ${CMAKE_CURRENT_SOURCE_DIR}/sample
  # COMMAND ld ${CMAKE_CURRENT_SOURCE_DIR}/sample.o ${CMAKE_CURRENT_SOURCE_DIR}/../build/src/libmlr.a /usr/lib/x86_64-linux-gnu/libc.so -o ${CMAKE_CURRENT_SOURCE_DIR}/sample
)

target_link_libraries(c2m PRIVATE 
  mlr 
  clang
)
