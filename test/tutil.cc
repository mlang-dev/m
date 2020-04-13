/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Utility functions for testing
 */
#include "tutil.h"

FILE* _open_file(const char* file_name)
{
    char* file_content = (char*)file_name;
    return fmemopen(file_content, strlen(file_content), "r");
}

parser* create_parser_for_string(char* str)
{
    return parser_new(str, false, _open_file);
}

file_tokenizer* create_tokenizer_for_string(char* str)
{
    FILE* file = fmemopen(str, strlen(str), "r");
    return create_tokenizer(file);
}
