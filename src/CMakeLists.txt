cmake_minimum_required(VERSION 3.16)

include(GoogleTest)

find_package(LLVM REQUIRED CONFIG)
execute_process(COMMAND llvm-config --libs all
                OUTPUT_VARIABLE llvm_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Found LLVM libraries ${llvm_libraries}")
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
  pthread
  dl
)

target_link_libraries(c2m PRIVATE 
  mlr 
  clang
)
