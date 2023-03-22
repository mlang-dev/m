find_program(
    CMAKE_M_COMPILER 
        NAMES "m" 
        HINTS "${CMAKE_SOURCE_DIR}"
        DOC "m compiler" 
)
mark_as_advanced(CMAKE_M_COMPILER)

set(CMAKE_M_SOURCE_FILE_EXTENSIONS m;M)
set(CMAKE_M_OUTPUT_EXTENSION .o)
set(CMAKE_M_COMPILER_ENV_VAR "M")

# Configure variables set in this file for fast reload later on
configure_file(${CMAKE_CURRENT_LIST_DIR}/CMakeMCompiler.cmake.in
               ${CMAKE_PLATFORM_INFO_DIR}/CMakeMCompiler.cmake)