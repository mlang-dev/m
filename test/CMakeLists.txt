cmake_minimum_required(VERSION 3.16)

project(mtest, VERSION 0.0.9 LANGUAGES CXX)

if (WASM)
  include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/unity
    ${CMAKE_SOURCE_DIR}/test
  )

  add_executable(mtest
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
    parser/test_parser_error.c    
    parser/test_grammar.c
    sema/test_analyzer.c
    codegen/test_type_size_info.c
    codegen/wasm/test_wasm_codegen.c
  )

  set_target_properties(mtest PROPERTIES LINKER_LANGUAGE C
                                      SUFFIX ".wasm")
  target_link_libraries(mtest PRIVATE mlrw clib unity
    ${CMAKE_SOURCE_DIR}/extern/wasi-libc/sysroot/lib/wasm32-wasi/libc.a
    ${CMAKE_SOURCE_DIR}/lib/wasi/libclang_rt.builtins-wasm32.a
    --target=wasm32
    -nostdlib
    -g
    -v
    -Wl,--no-entry,--strip-all,--export-dynamic
  )

  add_custom_command(TARGET mtest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/mtest.wasm ${CMAKE_SOURCE_DIR}/docs
  )

else()

  find_package(LLVM REQUIRED CONFIG)
  execute_process(COMMAND llvm-config --libnames
                  OUTPUT_VARIABLE llvm_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND llvm-config --system-libs
                  OUTPUT_VARIABLE sys_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT ${sys_libraries} STREQUAL "")
    string(REPLACE " -llibxml2.tbd" "" sys_libraries ${sys_libraries})
    string(REPLACE " " ";" sys_libraries ${sys_libraries})
  endif()
  string(REPLACE " " ";" llvm_libraries ${llvm_libraries})
  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
  message(STATUS "Found LLVM libraries ${llvm_libraries}")
  message(STATUS "Found SYS libraries ${sys_libraries}")
  add_definitions(${LLVM_DEFINITIONS})

  include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/include
  )

  link_directories(
    ${LLVM_INCLUDE_DIRS}/../lib
  )

  add_executable(mtest
    tutil.cc
    parser/test_astdump.cc
    sema/test_analyzer.cc
    sema/test_analyzer_errors.cc
    compiler/test_jit_relational.cc
    compiler/test_jit_logical.cc
    compiler/test_jit.cc
    tool/test_cmodule.cc
    codegen/llvm/test_cg_var.cc
    codegen/llvm/test_cg_fun_call.cc
  )

  if(WIN32)
    set(clang_lib libclang)
  else()
    set(clang_lib clang)
  endif(WIN32)

  if(MSVC)
      set_property(TARGET mtest APPEND PROPERTY LINK_FLAGS /DEBUG)
  endif()

  TARGET_LINK_LIBRARIES(mtest gtest gtest_main mlrl clib
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

endif()

if(WASM)
else()
  include_directories(
    ${CMAKE_SOURCE_DIR}/unity
  )
  add_executable(mctest
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
  parser/test_parser_error.c
  parser/test_grammar.c
  sema/test_analyzer.c
  codegen/test_type_size_info.c
  codegen/wasm/test_wasm_codegen.c
  )

  TARGET_LINK_LIBRARIES(mctest mlrw clib unity
  )

  # add_custom_command(
  #   TARGET mctest
  #   POST_BUILD
  #   COMMAND mctest
  # )

  add_custom_command(
    TARGET mtest
    POST_BUILD
    COMMAND mtest
  )
endif()
