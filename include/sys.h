#ifdef _WIN32
#include "win/unistd.h" 
#else
#include <unistd.h>p
#endif

char* get_exec_path();