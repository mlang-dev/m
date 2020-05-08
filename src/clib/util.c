/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * c util functions
 */
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#include "clib/util.h"

static char id_name[512] = "a";
void reset_id_name(const char *idname)
{
    strcpy(id_name, idname);
}

void _inc_str(string* id)
{
    if (!string_size(id))
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
    const char* data = string_get(str);
    for(size_t i = 0; i<string_size(str); i++){
        if (data[i] != match)
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
    reset_id_name(string_get(&new_id_name));
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

string str_format(const char* string_format, ...)
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

void print_backtrace(void)
{
    int j, nptrs;
#define SIZE 100
    void *buffer[SIZE];
    char **strings;

   nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

   /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

   strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

   for (j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

   free(strings);
}
