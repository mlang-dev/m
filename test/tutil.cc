/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Utility functions for testing
 */
#include <stdio.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>
#include "tutil.h"
#include "libfmemopen.h"

const char *module_name = "test_ir";

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

void make_module_ir(LLVMModuleRef module, const char *module_name, const char *ir_string, char *module_ir)
{
    sprintf(module_ir, R"(; ModuleID = '%s'
source_filename = "%s"
target datalayout = "%s"
target triple = "%s"
%s)", module_name, module_name, LLVMGetDataLayoutStr(module), LLVMGetDefaultTargetTriple(), ir_string);
}
