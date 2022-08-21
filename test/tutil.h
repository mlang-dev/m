#pragma once
#include <stdio.h>

extern const char *module_name;

FILE *open_file_from_string(const char *code);
void make_module_ir(void* mod, const char *module_name, const char *ir_string, char *module_ir);
void validate_m_code_with_ir_code(const char *m_code, const char *ir_code);
