#ifndef __UTIL_H__
#define __UTIL_H__

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "win/unistd.h"
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MALLOC(_ptr, _size)                                                      \
    do {                                                                         \
        if (NULL == (_ptr = malloc(_size))) {                                    \
            fprintf(stderr, "Failed to allocate memory. %s\n", strerror(errno)); \
            exit(EXIT_FAILURE);                                                  \
        }                                                                        \
    } while (0)

char *get_exec_path();

bool is_power_of2_64(uint64_t Value);

#ifdef __cplusplus
}
#endif

#endif
