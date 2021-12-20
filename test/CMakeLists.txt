cmake_minimum_required(VERSION 3.16)

project(mtest, VERSION 0.0.9 LANGUAGES CXX)

IF (EMSCRIPTEN)
  # exclude LLVM codegen testing
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
    lexer/test_token.c
    parser/test_g_parser.c
    parser/test_grammar.c
    codegen/test_wat_codegen.c
    app/test_mwat.c
  )

  set_target_properties(mtest PROPERTIES LINKER_LANGUAGE C
                                      SUFFIX ".wasm")
  target_link_libraries(mtest PRIVATE mlr unity
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

    lexer/test_m_lexer.cc
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
  )

  if(WIN32)
    set(clang_lib libclang)
  else()
    set(clang_lib clang)
  endif(WIN32)

  if(MSVC)
      set_property(TARGET mtest APPEND PROPERTY LINK_FLAGS /DEBUG)
  endif()

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
  if(WASMTEST)
    add_custom_command(
      TARGET mtest
      POST_BUILD
      COMMAND wasmtime mtest.wasm
    )
  endif()
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
  parser/test_g_parser.c
  parser/test_grammar.c
  lexer/test_token.c
  codegen/test_wat_codegen.c
  app/test_mwat.c
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

