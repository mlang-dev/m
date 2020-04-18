/*
 * util.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for utility functions
 */
#ifndef __MLANG_UTIL_H__
#define __MLANG_UTIL_H__

#include <string>
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

static const char* LogLevelString[] = {
    FOREACH_LOGLEVEL(GENERATE_ENUM_STRING)
};

void* log(LogLevel level, const char* string_format, ...);
string format(const char* string_format, ...);
std::vector<std::string> split(std::string, char separator);
string make_unique_name(const char* root);
int random(int min, int max);
bool is_new_line(int ch);
std::string vector_to_string(std::vector<std::string>& v);
string get_id_name();
void reset_id_name(const char *idname = "a");

#endif