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

#include "clib/array.h"
#include "clib/string.h"

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
void join_path(char *destination, const char *path1, const char *path2);

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#ifdef __cplusplus
}
#endif

#endif
