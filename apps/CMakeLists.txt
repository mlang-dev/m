 cmake_minimum_required(VERSION 3.16)

# generating m.wasm
add_executable(mw
  wasm/mw.c
  wasm/libc.c
)

set_target_properties(mw PROPERTIES LINKER_LANGUAGE C
                                    OUTPUT_NAME m
                                    SUFFIX ".wasm")
target_link_libraries(mw PRIVATE wasm_options mlrw clibw
  ${CMAKE_SOURCE_DIR}/extern/wasi-libc/sysroot/lib/wasm32-wasi/libc.a
  ${CMAKE_SOURCE_DIR}/lib/wasi/libclang_rt.builtins-wasm32.a
  --target=wasm32-wasi
  -nostdlib
  -g
  -Wl,-z,stack-size=655360 #set stack size to 640k
  -Wl,--no-entry,--export-dynamic
  -Xlinker --features=mutable-globals
  -Xlinker --export=__stack_pointer
  -Xlinker --export=__heap_base
  -Xlinker --stack-first
  -Xlinker --initial-memory=8388608  #set initial memory to 8MB
  -v
)

message(STATUS "copy ${CMAKE_CURRENT_BINARY_DIR}/wm.wasm ${CMAKE_SOURCE_DIR}/docs")
add_custom_command(TARGET mw POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/m.wasm ${CMAKE_SOURCE_DIR}/docs
)

find_package(LLVM 14 REQUIRED CONFIG)
message(STATUS "Found LLVM BIN ${LLVM_TOOLS_BINARY_DIR}")
execute_process(COMMAND llvm-config --libfiles
                OUTPUT_VARIABLE llvm_libfiles OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --system-libs
                OUTPUT_VARIABLE sys_libraries OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT ${sys_libraries} STREQUAL "")
  string(REPLACE " -llibxml2.tbd" "" sys_libraries ${sys_libraries})
  string(REPLACE " " ";" sys_libraries ${sys_libraries})
endif()
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Found LLVM libraries ${llvm_libfiles}")
message(STATUS "Found LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Found SYS libraries ${sys_libraries}") 

link_directories(
  ${LLVM_INCLUDE_DIRS}/../lib
)

add_executable(m
wasm/mw.c
driver/m.cc
)

target_compile_definitions(m PUBLIC NATIVE_APP=1)

target_include_directories(m PUBLIC
  ${LLVM_INCLUDE_DIRS}
)

set_target_properties(m PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(m PRIVATE 
  mlrl
  clib
  # LD dependencies
  # lldDriver
  # lldCOFF
  # lldELF
  # lldMachO
  # lldMachO2
  # lldMinGW
  # lldWasm
  # lldYAML
  # lldReaderWriter
  # lldCommon
  # lldCore
  ${llvm_libfiles}
  ${sys_libraries}
)

if(APPLE)
  target_link_libraries(m PRIVATE 
    xar
  )
elseif(UNIX)
  target_link_libraries(m PRIVATE 
    pthread
  )
endif()

add_custom_command(
  TARGET m
  POST_BUILD
  COMMAND m -s ${CMAKE_SOURCE_DIR}/src/sys ${CMAKE_SOURCE_DIR}/samples/sample_lib.m
  COMMAND m -s ${CMAKE_SOURCE_DIR}/src/sys ${CMAKE_SOURCE_DIR}/samples/sample.m
  COMMAND cp m ${CMAKE_SOURCE_DIR}/dep/m
  # COMMAND m ${CMAKE_SOURCE_DIR}/samples/helloworld.m
  # MacOS COMMAND ld ${CMAKE_CURRENT_SOURCE_DIR}/sample.o ${CMAKE_CURRENT_SOURCE_DIR}/../build/src/libmlr.a /usr/lib/libc.dylib -o ${CMAKE_CURRENT_SOURCE_DIR}/sample
  # COMMAND ld ${CMAKE_CURRENT_SOURCE_DIR}/sample.o ${CMAKE_CURRENT_SOURCE_DIR}/../build/src/libmlr.a /usr/lib/x86_64-linux-gnu/libc.so -o ${CMAKE_CURRENT_SOURCE_DIR}/sample
)
