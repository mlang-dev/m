# This file sets the basic flags for the mlang compiler
if(NOT CMAKE_M_COMPILE_OBJECT)
    set(CMAKE_M_COMPILE_OBJECT "<CMAKE_M_COMPILER> -c -o <OBJECT> <SOURCE>")
endif()
set(CMAKE_M_INFORMATION_LOADED 1)