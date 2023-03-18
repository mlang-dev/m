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
error/error.c
lexer/terminal.c
lexer/lexer.c
parser/node_type.c
parser/ast.c
parser/astdump.c
parser/m_parsing_table.c
parser/parser.c
parser/grammar.c
sema/type.c
sema/sema_context.c
sema/analyzer.c
sema/eval.c
sema/frontend.c
codegen/backend.c
codegen/type_size_info.c
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

find_package(LLVM 14 REQUIRED CONFIG)
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
  error/error.c
  lexer/lexer.c
  lexer/terminal.c
  lexer/pgen_token.c
  parser/node_type.c
  parser/grammar.c
  parser/m_parsing_table.c
  parser/lalr_parser_generator.c
  codegen/type_size_info.c
  pgen/pgen.c
)

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
    COMMAND pgen ${CMAKE_SOURCE_DIR}/lib/parser/m_grammar.txt ${CMAKE_SOURCE_DIR}/include/parser/m_parsing_table_def.h ${CMAKE_SOURCE_DIR}/lib/parser/m_parsing_table.c
  )
endif()

add_library(mlr
  error/error.c
  lexer/terminal.c
  lexer/lexer.c
  parser/node_type.c
  parser/ast.c
  parser/astdump.c
  parser/grammar.c
  parser/m_parsing_table.c
  parser/parser.c
  sema/type.c
  sema/analyzer.c
  sema/eval.c
  sema/sema_context.c
  sema/frontend.c
  sema/frontend_llvm.c
  codegen/backend.c
  codegen/type_size_info.c
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
  error/error.c
  lexer/terminal.c
  lexer/lexer.c
  parser/node_type.c
  parser/ast.c
  parser/astdump.c
  parser/grammar.c
  parser/m_parsing_table.c
  parser/parser.c
  sema/type.c
  sema/analyzer.c
  sema/eval.c
  sema/sema_context.c
  sema/frontend.c
  sema/frontend_llvm.c
  codegen/backend.c
  codegen/type_size_info.c
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
  compiler/engine_wasm.c
)

target_include_directories(mlrl PUBLIC
  ${LLVM_INCLUDE_DIRS}
)
