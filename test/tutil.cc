/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Utility functions for testing
 */
#include <string.h>

#include "tutil.h"

FILE* _open_file(const char* file_name)
{
    char* file_content = (char*)file_name;
    return fmemopen(file_content, strlen(file_content), "r");
}

parser* create_parser_for_string(const char* str)
{
    return parser_new(str, false, _open_file);
}

menv* create_env_for_string(const char* str)
{
    return env_new(str, false, _open_file);
}

file_tokenizer* create_tokenizer_for_string(const char* str)
{
    FILE* file = fmemopen((void*)str, strlen(str), "r");
    return create_tokenizer(file);
}
