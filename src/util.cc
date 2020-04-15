/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * utilities functions
 */
#include "util.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::string id_name = "a";
void reset_id_name(std::string idname)
{
    id_name = idname;
}

std::string _inc_str(std::string id)
{
    if (id == "")
        return id;
    char ch = id.back();
    id.pop_back();
    if (ch == 'z') {
        ch = 'a';
        id = _inc_str(id);
    } else
        ch++;
    id.push_back(ch);
    return id;
}

std::string get_id_name()
{
    std::string str = id_name;
    id_name = _inc_str(id_name);
    if (id_name.back() == 'a' && all_of(id_name.begin(), id_name.end(), [](char x) { return x == 'a'; }))
        id_name.push_back('a');
    return str;
}

static char alpha_nums[36];
static bool alpha_nums_init = false;

std::string vector_to_string(std::vector<std::string>& array)
{
    std::ostringstream imploded;
    copy(array.begin(), array.end(),
        std::ostream_iterator<std::string>(imploded, " "));
    return imploded.str();
}

std::string char_to_string(char c)
{
    std::string s(1, c);
    return s;
}

int random(int min, int max)
{
    return min + (rand() % static_cast<int>(max - min + 1));
}

std::string make_unique_name(const char* name)
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
        int j = random(0, 35);
        s[i] = alpha_nums[j];
    }
    std::string str = s;
    std::string name_str = name;
    return name_str + "-" + str;
}

void* log(LogLevel level, const char* string_format, ...)
{
    va_list args;
    char format[512];
    sprintf(format, "%s: %s\n", LogLevelString[level], string_format);
    va_start(args, string_format);
    vfprintf(stderr, format, args);
    va_end(args);
    return 0;
}

std::vector<std::string> split(std::string str, char separator)
{
    std::vector<std::string> strings;
    std::istringstream f(str);
    std::string s;
    while (getline(f, s, separator)) {
        strings.push_back(s);
    }
    return strings;
}

std::string format(const char* string_format, ...)
{
    va_list args;
    char data[512];
    va_start(args, string_format);
    vsprintf(data, string_format, args);
    va_end(args);
    return data;
}

std::string get_filename(const char* fullfilename)
{
    std::string fn = fullfilename;
    auto pos = fn.find_last_of('.');
    return fn.substr(0, pos);
}

bool is_new_line(int ch)
{
    return ch == '\r' || ch == '\n';
}

