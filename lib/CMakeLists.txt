cmake_minimum_required(VERSION 3.16)

if (WASM)
  include_directories(
    ${CMAKE_SOURCE_DIR}/include
  )
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
    clib/regex.c
 #   clib/win/getopt.c
 #   clib/win/libfmemopen.c
 #   clib/getpath.c
  )

  add_library(mlr 
    lexer/token.c
    lexer/lexer.c
    lexer/init.c
    parser/ast.c
    parser/astdump.c
    parser/m_parsing_table.c
    parser/parser.c
    parser/grammar.c
    sema/type.c
    sema/sema_context.c
    sema/analyzer.c
    codegen/wasm/wasm_codegen.c
    wasm/sys.c
  )
else()
  find_package(LLVM REQUIRED CONFIG)
  message(STATUS "Found LLVM Include: ${LLVM_INCLUDE_DIRS}")

  include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
  )
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

  add_executable(pgen
    lexer/token.c
    lexer/lexer.c
    lexer/init.c
    sema/type.c
    parser/ast.c
    parser/grammar.c
    parser/lalr_parser_generator.c
    pgen/pgen.c
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
    tool/cmodule.c
    lexer/token.c
    lexer/lexer.c
    lexer/init.c
    parser/ast.c
    parser/astdump.c
    parser/grammar.c
    parser/m_parsing_table.c
    parser/parser.c
    sema/type.c
    sema/analyzer.c
    sema/sema_context.c
    codegen/env.c
    codegen/type_size_info.c
    codegen/llvm/abi_arg_info.c
    codegen/llvm/fun_info.c
    codegen/llvm/target_info.c
    codegen/llvm/x86_64_abi.c
    codegen/llvm/winx86_64_abi.c
    codegen/llvm/llvm_codegen.c
    codegen/llvm/cg_fun.c
    codegen/llvm/cg_var.c
    codegen/llvm/cg_call.c
    codegen/llvm/ir_arg_info.c
    codegen/llvm/compute_fun_info.c
    codegen/llvm/ir_api.c
    codegen/wasm/wasm_codegen.c
    compiler/repl.c
    compiler/jit.c
    compiler/compiler.c
  )
endif()