#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "sys.h"
#ifdef __APPLE__
#include <libproc.h>
char SEP = '/';
#elif defined(_WIN32)
#include <windows.h>
char SEP = '\\';
#elif defined(__linux__)
char SEP = '/';
#include <linux/limits.h>
#else
char SEP = '/';
#endif

char exec_path[PATH_MAX];

static void reduce(char *dir)
{
    size_t i = strlen(dir);
    while (i > 0 && dir[i] != SEP) {
        --i;
    }
    dir[i] = '\0';
}

char* get_exec_path(){
    int ret;
    #if defined(__APPLE__)
    pid_t pid; 
    pid = getpid();
    ret = proc_pidpath (pid, exec_path, sizeof(exec_path));
    if ( ret <= 0 ) {
        fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
        fprintf(stderr, "    %s\n", strerror(errno));
    } else {
        //printf("proc %d: %s\n", pid, exec_path);
    }
    #elif defined(__linux__)
    readlink("/proc/self/exe", exec_path, sizeof(exec_path));    
    #elif defined(_WIN32)
        TCHAR buffer[MAX_PATH];
        ret = GetModuleFileName(NULL, buffer, sizeof(buffer));
        if (ret == 0){
            fprintf(stderr, "failed to get exec_path\n");
        }else{
            wcstombs(exec_path, buffer, sizeof(exec_path));
           //    wprintf("get exec path wsprintf: %s\n", exec_path);
        }
    #endif
    reduce(exec_path);
    return exec_path;
}