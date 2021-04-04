#pragma once
#include <stdio.h>
#include "parser/parser.h"
#include <llvm-c/Core.h>

extern const char *module_name;

FILE* open_file_from_string(const char* code);
file_tokenizer* create_tokenizer_for_string(const char* str);
void make_module_ir(LLVMModuleRef module, const char *module_name, const char *ir_string, char *module_ir);