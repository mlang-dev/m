cmake_minimum_required(VERSION 3.16)

include(GoogleTest)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM BIN ${LLVM_TOOLS_BINARY_DIR}")
execute_process(COMMAND llvm-config --libnames
                OUTPUT_VARIABLE llvm_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --system-libs
                OUTPUT_VARIABLE sys_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " -llibxml2.tbd" "" sys_libraries ${sys_libraries})
string(REPLACE " " ";" llvm_libraries ${llvm_libraries})
string(REPLACE " " ";" sys_libraries ${sys_libraries})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Found LLVM libraries ${llvm_libraries}")
message(STATUS "Found LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Found SYS libraries ${sys_libraries}") 
if(WIN32)
  set(clang_lib libclang)
  message(STATUS, "Found Windows Kit: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
  message(STATUS, "Found Windows Platform: ${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}")
else()
  set(clang_lib clang)
endif(WIN32)


include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(mlr 
  clib/math.c
  clib/object.c
  clib/array.c
  clib/string.c
  clib/symbol.c
  clib/symboltable.c
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
  sema/type.c
  sema/analyzer.c
  sema/sema_context.c
  codegen/type_size_info.c
  codegen/abi_arg_info.c
  codegen/fun_info.c
  codegen/target_info.c
  codegen/x86_64_abi.c
  codegen/winx86_64_abi.c
  codegen/codegen.c
  codegen/ir_arg_info.c
  codegen/env.c
  codegen/compute_fun_info.c
  compiler/repl.c
  compiler/jit.c
  compiler/compiler.c
  win/getopt.c
  win/libfmemopen.c
  sys/getpath.c
 )

include_directories(
  ${CMAKE_SOURCE_DIR}/include
)

link_directories(
  ${LLVM_INCLUDE_DIRS}/../lib
)