cmake_minimum_required(VERSION 3.16)

project(mtest, VERSION 0.0.9 LANGUAGES CXX)

IF (EMSCRIPTEN)

  include_directories(
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/include
  )


  add_executable(mtest
    clib/test_symbol.cc
    clib/test_symboltable.cc
    clib/test_math.cc
    clib/test_string.cc
    clib/test_array.c
    clib/test_queue.cc
    clib/test_stack.cc
    clib/test_hashtable.cc
    clib/test_hashset.cc
    clib/test_util.cc
    parser/test_tok.cc
    parser/test_g_parser.cc
    parser/test_grammar.cc
    codegen/test_wat_codegen.cc
    app/test_mwat.cc
  )

  TARGET_LINK_LIBRARIES(mtest gtest gtest_main mlr
  )


  enable_testing()
  include(GoogleTest)
  gtest_discover_tests(mtest)
  
elseif (WASM)

  include_directories(
    ${CMAKE_SOURCE_DIR}/include
  )


  add_executable(mtest
    clib/test_array.c
    # clib/test_symbol.cc
    # clib/test_symboltable.cc
    # clib/test_math.cc
    # clib/test_string.cc
    # clib/test_queue.cc
    # clib/test_stack.cc
    # clib/test_hashtable.cc
    # clib/test_hashset.cc
    # clib/test_util.cc
    # parser/test_tok.cc
    # parser/test_g_parser.cc
    # parser/test_grammar.cc
    # codegen/test_wasm_codegen.cc
    # app/test_mwat.cc
  )

  set_target_properties(mtest PROPERTIES LINKER_LANGUAGE C
                                      SUFFIX ".wasm")
  target_link_libraries(mtest PRIVATE mlr
    ${CMAKE_SOURCE_DIR}/extern/wasi-libc/sysroot/lib/wasm32-wasi/libc.a
    --target=wasm32
    -nostdlib
    -v
    -Wl,--no-entry,--strip-all,--export-dynamic
  )

else()

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
    clib/test_symbol.cc
    clib/test_symboltable.cc
    clib/test_math.cc
    clib/test_string.cc
    clib/test_queue.cc
    clib/test_stack.cc
    clib/test_hashtable.cc
    clib/test_hashset.cc
    clib/test_util.cc
    parser/test_tok.cc
    parser/test_g_parser.cc
    parser/test_grammar.cc

    lexer/test_lexer.cc
    parser/test_parser.cc
    parser/test_parser_logical.cc
    parser/test_parser_relational.cc
    parser/test_astdump.cc
    sema/test_analyzer.cc
    sema/test_analyzer_errors.cc
    compiler/test_jit_relational.cc
    compiler/test_jit_logical.cc
    compiler/test_jit.cc
    tool/test_cmodule.cc
    codegen/test_type_size_info.cc
    codegen/test_cg_var.cc
    codegen/test_cg_fun_call.cc
    codegen/test_wat_codegen.cc
    app/test_mwat.cc
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

endif()

if(WASM)
  add_custom_command(
    TARGET mtest
    POST_BUILD
    COMMAND wasmtime mtest.wasm
  )
else()
  include_directories(
    ${CMAKE_SOURCE_DIR}/unity
  )
  add_executable(mctest
  test.c
  clib/test_array.c
  )

  TARGET_LINK_LIBRARIES(mctest mlr unity
  )

  add_custom_command(
    TARGET mctest
    POST_BUILD
    COMMAND mctest
  )
  
  add_custom_command(
    TARGET mtest
    POST_BUILD
    COMMAND mtest
  )
endif()

