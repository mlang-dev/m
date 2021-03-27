#pragma once
#include <stdio.h>
#include "parser/parser.h"

FILE* open_file_from_string(const char* code);
file_tokenizer* create_tokenizer_for_string(const char* str);