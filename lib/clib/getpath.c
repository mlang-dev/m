#include "clib/util.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
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

char *get_exec_path()
{
#if defined(__APPLE__)
    int ret;
    pid_t pid;
    pid = getpid();
    ret = proc_pidpath(pid, exec_path, sizeof(exec_path));
    if (ret <= 0) {
        fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
        fprintf(stderr, "    %s\n", strerror(errno));
    } else {
        // printf("proc %d: %s\n", pid, exec_path);
    }
#elif defined(__linux__)
    readlink("/proc/self/exe", exec_path, sizeof(exec_path));
#elif defined(_WIN32)
    int file_len;
    file_len = GetModuleFileNameA(NULL, exec_path, sizeof(exec_path));
    if (file_len == 0) {
        fprintf(stderr, "failed to get exec_path\n");
    }
#endif
    reduce(exec_path);
    return exec_path;
}