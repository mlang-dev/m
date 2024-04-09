cmake_minimum_required(VERSION 3.16)

project(mtest, VERSION 0.0.9 LANGUAGES CXX)

add_executable(mtestw
  ../lib/parser/grammar.c
  test.c
  clib/test_array.c
  clib/test_byte_array.c
  clib/test_symbol.c
  clib/test_symboltable.c
  clib/test_math.c
  clib/test_string.c
  clib/test_queue.c
  clib/test_stack.c
  clib/test_hashtable.c
  clib/test_hashset.c
  clib/test_util.c
  clib/test_regex.c

  lexer/test_lexer.c
  lexer/test_lexer_error.c
  lexer/test_m_lexer.c
  lexer/test_token.c
  parser/test_ast.c
  parser/test_parser_expr.c
  parser/test_parser.c
  parser/test_parser_type.c
  parser/test_parser_struct.c
  parser/test_parser_variant.c
  parser/test_parser_error.c
  parser/test_grammar.c
  sema/test_analyzer.c
  sema/test_analyzer_variant.c
  sema/test_analyzer_pm.c
  sema/test_analyzer_struct.c
  sema/test_analyzer_type.c
  sema/test_analyzer_mut.c
  sema/test_analyzer_errors.c
  codegen/test_type_size_info.c
  codegen/wasm/test_wasm_codegen.c

  unity/unity.c
)

set_target_properties(mtestw PROPERTIES LINKER_LANGUAGE C
                                    SUFFIX ".wasm")
target_link_libraries(mtestw PRIVATE wasm_options mlrw clibw
  ${CMAKE_SOURCE_DIR}/extern/wasi-libc/sysroot/lib/wasm32-wasi/libc.a
  ${CMAKE_SOURCE_DIR}/lib/wasi/libclang_rt.builtins-wasm32.a
  --target=wasm32
  -nostdlib
  -g
  -v
  -Wl,--no-entry,--strip-all,--export-dynamic
)

add_custom_command(TARGET mtestw POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/mtestw.wasm ${CMAKE_SOURCE_DIR}/docs
)


find_package(LLVM 18.1.3 REQUIRED CONFIG)
execute_process(COMMAND llvm-config-18 --libfiles
                OUTPUT_VARIABLE llvm_libfiles OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-18 --system-libs
                OUTPUT_VARIABLE sys_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT ${sys_libraries} STREQUAL "")
  string(REPLACE " -llibxml2.tbd" "" sys_libraries ${sys_libraries})
  string(REPLACE " " ";" sys_libraries ${sys_libraries})
endif()
message(STATUS "Found LLVM for mtest ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM shared libs ")
message(STATUS "Found SYS libraries ${sys_libraries}")
add_definitions(${LLVM_DEFINITIONS})

add_executable(mtest
  tutil.cc
  test_main.cc
  codegen/llvm/test_cg_relational.cc
  codegen/llvm/test_cg_logical.cc
  codegen/llvm/test_cg_var.cc
  codegen/llvm/test_cg_fun_call.cc
  compiler/test_jit_control.cc
  compiler/test_jit_relational.cc
  compiler/test_jit_logical.cc
  compiler/test_jit_adt.cc
  compiler/test_jit.cc
  compiler/test_jit_error.cc
  compiler/test_jit_array.cc
)

target_compile_options(mtest PRIVATE
-fsanitize=address
-g
)

target_link_options(mtest PRIVATE
-fsanitize=address
)

if(MSVC)
    set_property(TARGET mtest APPEND PROPERTY LINK_FLAGS /DEBUG)
endif()

TARGET_LINK_LIBRARIES(mtest gtest mlrl clib
  ${llvm_libfiles}
  ${sys_libraries}
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(mtest)

add_executable(mctest
../lib/parser/grammar.c
test.c
clib/test_hashset.c
clib/test_array.c
clib/test_byte_array.c
clib/test_hashtable.c
clib/test_math.c
clib/test_queue.c
clib/test_stack.c
clib/test_string.c
clib/test_symbol.c
clib/test_symboltable.c
clib/test_util.c
clib/test_regex.c
lexer/test_lexer.c
lexer/test_lexer_error.c
lexer/test_m_lexer.c
lexer/test_token.c
parser/test_ast.c
parser/test_parser_expr.c
parser/test_parser.c
parser/test_parser_type.c
parser/test_parser_struct.c
parser/test_parser_variant.c
parser/test_parser_error.c
parser/test_grammar.c
sema/test_analyzer.c
sema/test_analyzer_variant.c
sema/test_analyzer_pm.c
sema/test_analyzer_struct.c
sema/test_analyzer_type.c
sema/test_analyzer_mut.c
sema/test_analyzer_errors.c
codegen/test_type_size_info.c
codegen/wasm/test_wasm_codegen.c
unity/unity.c
)

target_compile_options(mctest PRIVATE
-fsanitize=address
-g
)

target_link_options(mctest PRIVATE
-fsanitize=address
)

TARGET_LINK_LIBRARIES(mctest mlr clib
  -g
)

add_custom_command(
  TARGET mctest
  POST_BUILD
  COMMAND mctest
)

add_custom_command(
  TARGET mtest
  POST_BUILD
  COMMAND mtest ${CMAKE_SOURCE_DIR}/src/sys
)
