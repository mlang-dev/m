/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * c util functions
 */
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "clib/util.h"

static char id_name[512] = "a";
void reset_id_name(const char *idname)
{
    strcpy(id_name, idname);
}

void _inc_str(string* id)
{
    if (id->size==0)
        return;
    char ch = string_pop(id);
    if (ch == 'z') {
        ch = 'a';
        _inc_str(id);
    } else
        ch++;
    string_push(id, ch);
}

bool is_all(string *str, char match)
{
    for(size_t i = 0; i<str->size; i++){
        if (str->data[i] != match)
            return false;
    }
    return true;
}

string get_id_name()
{
    string str;
    string_init_chars(&str, id_name);
    string new_id_name;
    string_init_chars(&new_id_name, id_name);
    _inc_str(&new_id_name);
    if (string_back(&new_id_name) == 'a' && is_all(&new_id_name,  'a'))
        string_push(&new_id_name, 'a');
    reset_id_name(new_id_name.data);
    string_deinit(&new_id_name);
    return str;
}

static char alpha_nums[36];
static bool alpha_nums_init = false;

int get_random(int min, int max)
{
    return min + (rand() % (max - min + 1));
}

string make_unique_name(const char* name)
{
    if (!alpha_nums_init) {
        char c = '0';
        for (int i = 0; i < 10; i++) {
            alpha_nums[i] = c + i;
        }
        c = 'a';
        for (int i = 0; i < 26; i++) {
            alpha_nums[i + 10] = c + i;
        }
        alpha_nums_init = true;
    }
    static int i = 0;
    char s[16];
    for (int i = 0; i < 16; i++) {
        int j = get_random(0, 35);
        s[i] = alpha_nums[j];
    }
    string name_str;
    string_init_chars(&name_str, name);
    string_add_chars(&name_str, "-");
    string_add_chars(&name_str, s);
    return name_str;
}

void* log_info(enum LogLevel level, const char* string_format, ...)
{
    va_list args;
    char format[512];
    sprintf(format, "%s: %s\n", LogLevelString[level], string_format);
    va_start(args, string_format);
    vfprintf(stderr, format, args);
    va_end(args);
    return 0;
}

string format(const char* string_format, ...)
{
    va_list args;
    char data[512];
    va_start(args, string_format);
    vsprintf(data, string_format, args);
    va_end(args);
    string str;
    string_init_chars(&str, data);
    return str;
}

bool is_new_line(int ch)
{
    return ch == '\r' || ch == '\n';
}
