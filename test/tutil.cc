/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Utility functions for testing
 */
#include <string.h>

#include "tutil.h"

FILE* open_file_from_string(const char* code)
{
    char* file_content = (char*)code;
    return fmemopen(file_content, strlen(file_content), "r");
}

file_tokenizer* create_tokenizer_for_string(const char* str)
{
    FILE* file = fmemopen((void*)str, strlen(str), "r");
    return create_tokenizer(file, "");
}
