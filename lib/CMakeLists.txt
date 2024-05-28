cmake_minimum_required(VERSION 3.16)

add_library(clibw 
clib/math.c
clib/object.c
clib/array.c
clib/byte_array.c
clib/string.c
clib/symbol.c
clib/symboltable.c
clib/queue.c
clib/stack.c
clib/hash.c
clib/hashset.c
clib/hashtable.c
clib/util.c
clib/generic.c
clib/regex.c
#   clib/win/getopt.c
#   clib/win/libfmemopen.c
#   clib/getpath.c
)

target_link_libraries(clibw PRIVATE wasm_options)

add_library(mlrw
app/app.c
app/error.c
lexer/token.c
lexer/lexer.c
parser/node_type.c
parser/ast.c
parser/astdump.c
parser/m/m_parsing_table.c
parser/parser.c
pgen/lang_token.c
sema/type.c
sema/sema_context.c
sema/analyzer.c
sema/eval.c
sema/frontend.c
sema/type_size_info.c
codegen/backend.c
codegen/abi_arg_info.c
codegen/fun_info.c
codegen/fun_context.c
codegen/target_arg_info.c
codegen/target_info.c
codegen/wasm/cg_wasm.c
codegen/wasm/cg_var_wasm.c
codegen/wasm/cg_call_wasm.c
codegen/wasm/cg_fun_wasm.c
codegen/wasm/cg_aggregate_wasm.c
codegen/wasm/wasm_abi.c
codegen/wasm/wasm_api.c
compiler/engine.c
compiler/engine_wasm.c
wasm/sys.c
)

target_link_libraries(mlrw PRIVATE wasm_options)

find_package(LLVM 18.1.3 REQUIRED CONFIG)
message(STATUS "Found LLVM CMAKE DIR: ${LLVM_CMAKE_DIR}, LLVM Include: ${LLVM_INCLUDE_DIRS}")

add_library(clib 
  clib/math.c
  clib/object.c
  clib/array.c
  clib/byte_array.c
  clib/string.c
  clib/symbol.c
  clib/symboltable.c
  clib/queue.c
  clib/stack.c
  clib/hash.c
  clib/hashset.c
  clib/hashtable.c
  clib/util.c
  clib/generic.c
  clib/win/getopt.c
  clib/win/libfmemopen.c
  clib/regex.c
  clib/getpath.c
)

target_include_directories(clib PUBLIC
  ${LLVM_INCLUDE_DIRS}
)

add_executable(pgen
  app/app.c
  app/error.c
  lexer/lexer.c
  lexer/pgen/grammar_token.c
  parser/node_type.c
  parser/grammar.c
  sema/type_size_info.c
  pgen/lalr_parser_generator.c
  pgen/lang_token.c
  pgen/pgen.c
)

target_compile_definitions(pgen PRIVATE GRAMMAR_PARSER=1)

target_include_directories(pgen PUBLIC
  ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(pgen PRIVATE 
  clib
)

if(REGEN_PARSER)
  add_custom_command(
    TARGET pgen
    POST_BUILD
    COMMAND pgen m ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/lib/
    COMMAND pgen wit ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/lib/
  )
endif()

add_library(mlr
  app/app.c
  app/error.c
  lexer/token.c
  lexer/lexer.c
  parser/node_type.c
  parser/ast.c
  parser/astdump.c
  pgen/lang_token.c
  parser/m/m_parsing_table.c
  parser/parser.c
  sema/type.c
  sema/analyzer.c
  sema/eval.c
  sema/sema_context.c
  sema/frontend.c
  sema/frontend_sys.c
  sema/type_size_info.c
  codegen/backend.c
  codegen/abi_arg_info.c
  codegen/fun_info.c
  codegen/fun_context.c
  codegen/target_info.c
  codegen/target_arg_info.c
  codegen/wasm/cg_wasm.c
  codegen/wasm/cg_var_wasm.c
  codegen/wasm/cg_fun_wasm.c
  codegen/wasm/cg_call_wasm.c
  codegen/wasm/cg_aggregate_wasm.c    
  codegen/wasm/wasm_abi.c
  codegen/wasm/wasm_api.c
  compiler/engine.c
  compiler/engine_wasm.c
)

target_include_directories(mlr PUBLIC
  ${LLVM_INCLUDE_DIRS}
)

add_library(mlrl
  app/app.c
  app/error.c
  lexer/token.c
  lexer/lexer.c
  parser/node_type.c
  parser/ast.c
  parser/astdump.c
  pgen/lang_token.c
  parser/m/m_parsing_table.c
  parser/parser.c
  sema/type.c
  sema/analyzer.c
  sema/eval.c
  sema/sema_context.c
  sema/frontend.c
  sema/frontend_sys.c
  sema/type_size_info.c
  codegen/backend.c
  codegen/abi_arg_info.c
  codegen/fun_info.c
  codegen/fun_context.c
  codegen/target_info.c
  codegen/target_arg_info.c
  codegen/llvm/x86_64_abi.c
  codegen/llvm/winx86_64_abi.c
  codegen/llvm/cg_llvm.c
  codegen/llvm/cg_fun_llvm.c
  codegen/llvm/cg_var_llvm.c
  codegen/llvm/cg_call_llvm.c
  codegen/llvm/llvm_api.c
  codegen/mlir/cg_mlir.c
  codegen/mlir/mlir_test.c
  codegen/mlir/mlir_api.c
  codegen/wasm/cg_wasm.c
  codegen/wasm/cg_var_wasm.c
  codegen/wasm/cg_fun_wasm.c
  codegen/wasm/cg_call_wasm.c
  codegen/wasm/cg_aggregate_wasm.c    
  codegen/wasm/wasm_abi.c
  codegen/wasm/wasm_api.c
  compiler/repl.c
  compiler/jit.c
  compiler/compiler.c
  compiler/engine.c
  compiler/engine_llvm.c
  compiler/engine_mlir.c
  compiler/engine_wasm.c
)

target_include_directories(mlrl PUBLIC
  ${LLVM_INCLUDE_DIRS}
)
