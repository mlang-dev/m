#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libproc.h>
#include <unistd.h>

char exec_path[PROC_PIDPATHINFO_MAXSIZE];

char SEP = '/';

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
    pid_t pid; 

    pid = getpid();
    ret = proc_pidpath (pid, exec_path, sizeof(exec_path));
    if ( ret <= 0 ) {
        fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
        fprintf(stderr, "    %s\n", strerror(errno));
    } else {
        reduce(exec_path);
        //printf("proc %d: %s\n", pid, exec_path);
    }
    return exec_path;
}