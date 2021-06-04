#pragma once
#include "parser/parser.h"
#include <llvm-c/Core.h>
#include <stdio.h>

extern const char *module_name;

FILE *open_file_from_string(const char *code);
tokenizer *create_tokenizer_for_string(const char *str);
void make_module_ir(LLVMModuleRef module, const char *module_name, const char *ir_string, char *module_ir);
void validate_m_code_with_ir_code(const char *m_code, const char *ir_code);