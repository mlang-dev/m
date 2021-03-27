#include <stdio.h>
#ifdef _WIN32
#include "win/unistd.h" 
#else
#include <unistd.h>
#endif

#define MALLOC(_ptr, _size) \
do { \
 if (NULL == (_ptr = malloc(_size))) { \
    fprintf(stderr, "Failed to allocate memory. %s\n", strerror(errno)); \
    exit(EXIT_FAILURE); \
 } \
} while(0)

char* get_exec_path();