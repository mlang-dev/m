/*
 * util.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for utility functions
 */
#ifndef __MLANG_UTIL_H__
#define __MLANG_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "clib/array.h"
#include "clib/string.h"
#include <stdlib.h>
#ifdef _WIN32
#include "clib/win/unistd.h"
#else
#include <unistd.h>
#endif
#include <errno.h>

#define ENABLE_DEBUG_LOG 0
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_ENUM_STRING(STRING) #STRING,
#define FOREACH_LOGLEVEL(ENUM_ITEM) \
    ENUM_ITEM(DEBUG)                \
    ENUM_ITEM(INFO)                 \
    ENUM_ITEM(ERROR)

enum LogLevel {
    FOREACH_LOGLEVEL(GENERATE_ENUM)
};

extern const char *log_level_strings[];

void *log_info(enum LogLevel level, const char *string_format, ...);
string str_format(const char *string_format, ...);
string make_unique_name(const char *root);
//int random(int min, int max);
bool is_new_line(int ch);
string get_id_name();
void reset_id_name(const char *idname);
void print_backtrace(void);
void join_path(char *destination, size_t dst_size, const char *path1, const char *path2);
char *get_basename(char *filename);


#define MMEM_MALLOC malloc
#define MMEM_FREE free
#define MMEM_CALLOC calloc
#define MMEM_REALLOC realloc

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ERROR_MSG_MAX 512
#define MALLOC(_ptr, _size)                                                      \
    do {                                                                         \
        if (NULL == (_ptr = MMEM_MALLOC(_size))) {                                    \
            printf("no enough memory to malloc !\n");                     \
            exit(1);                                                  \
        }                                                                        \
    } while (0)

#define CALLOC(_ptr, _element_count, element_size)                                                      \
    do {                                                                         \
        if (NULL == (_ptr = MMEM_CALLOC(_element_count, element_size))) {                                    \
            printf("no enough memory to calloc !\n");                     \
            exit(1);                                                  \
        }                                                                        \
    } while (0)

#define REALLOC(_ptr, old_mem, _size)                                                      \
    do {                                                                         \
        if (NULL == (_ptr = MMEM_REALLOC(old_mem, _size))) {                                    \
            exit(1);                                                  \
        }                                                                        \
    } while (0)

#define FREE(_ptr) MMEM_FREE(_ptr)

char *get_exec_path();

bool is_power_of2_64(uint64_t Value);

const char *read_text_file(const char *file_path);

#ifdef __cplusplus
}
#endif

#endif
