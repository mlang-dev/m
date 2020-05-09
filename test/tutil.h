#pragma once
#include <stdio.h>
#include "parser.h"
#include "env.h"

parser* create_parser_for_string(const char* str);
menv* create_env_for_string(const char* str);
file_tokenizer* create_tokenizer_for_string(const char* str);