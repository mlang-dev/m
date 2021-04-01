cmake_minimum_required(VERSION 3.16)

project(mtest, VERSION 0.0.9 LANGUAGES CXX)

find_package(LLVM REQUIRED CONFIG)
execute_process(COMMAND llvm-config --libnames
                OUTPUT_VARIABLE llvm_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --system-libs
                OUTPUT_VARIABLE sys_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " -llibxml2.tbd" "" sys_libraries ${sys_libraries})
string(REPLACE " " ";" llvm_libraries ${llvm_libraries})
string(REPLACE " " ";" sys_libraries ${sys_libraries})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Found LLVM libraries ${llvm_libraries}")
message(STATUS "Found SYS libraries ${sys_libraries}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(
  ${CMAKE_SOURCE_DIR}/test
  ${CMAKE_SOURCE_DIR}/include
)

link_directories(
  ${LLVM_INCLUDE_DIRS}/../lib
)

add_executable(mtest
  test.cc
  tutil.cc
  test_symbol.cc
  test_symboltable.cc
  test_math.cc
  test_string.cc
  test_array.cc
  test_queue.cc
  test_hashtable.cc
  test_hashset.cc
  test_util.cc
  test_lexer.cc
  test_parser.cc
  test_parser_logical.cc
  test_parser_relational.cc
  test_astdump.cc
  test_analyzer.cc
  test_analyzer_errors.cc
  test_jit_relational.cc
  test_jit_logical.cc
  test_jit.cc
  test_cmodule.cc
  codegen/test_type_size_info.cc
)

if(WIN32)
  set(clang_lib libclang)
else()
  set(clang_lib clang)
endif(WIN32)

TARGET_LINK_LIBRARIES(mtest gtest gtest_main mlr
  ${llvm_libraries}
  ${sys_libraries}
  ${clang_lib}
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(mtest)
if(WIN32)
add_custom_command(
  TARGET mtest PRE_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/mlib" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/mlib"
)
else()
file(COPY ${CMAKE_SOURCE_DIR}/mlib/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/mlib/)
endif(WIN32)

add_custom_command(
  TARGET mtest
  POST_BUILD
  COMMAND mtest
)


