cmake_minimum_required(VERSION 3.16)

if (EMSCRIPTEN OR WASM)
  include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/extern/binaryen/src
  )

  add_library(mlr 
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
 #   clib/win/getopt.c
 #   clib/win/libfmemopen.c
 #   clib/getpath.c
    parser/ast_node.c
    parser/tok.c
    parser/parser.c
    parser/grammar.c
    parser/m_grammar.c
    codegen/wat_codegen.c
    wasm/mwat.c
    wasm/sys.c
    wasm/wasm.c
  )
else()
  find_package(LLVM REQUIRED CONFIG)
  message(STATUS "Found LLVM Include: ${LLVM_INCLUDE_DIRS}")

  include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
  )

  add_library(mlr 
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
    clib/getpath.c
    tool/cmodule.c
    lexer/lexer.c
    lexer/keyword.c
    parser/ast.c
    parser/ast_node.c
    parser/tok.c
    parser/parser.c
    parser/astdump.c
    parser/grammar.c
    parser/m_parser.c
    parser/m_grammar.c
    sema/type.c
    sema/analyzer.c
    sema/sema_context.c
    codegen/type_size_info.c
    codegen/abi_arg_info.c
    codegen/fun_info.c
    codegen/target_info.c
    codegen/x86_64_abi.c
    codegen/winx86_64_abi.c
    codegen/llvm_codegen.c
    codegen/wat_codegen.c
    codegen/cg_fun.c
    codegen/cg_var.c
    codegen/cg_call.c
    codegen/ir_arg_info.c
    codegen/env.c
    codegen/compute_fun_info.c
    codegen/ir_api.c
    compiler/repl.c
    compiler/jit.c
    compiler/compiler.c
    wasm/mwat.c
  )
endif()