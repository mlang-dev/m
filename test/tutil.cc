/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Utility functions for testing
 */
#include "tutil.h"
#include "clib/win/libfmemopen.h"
#include "codegen/llvm/cg_llvm.h"
#include "compiler/engine.h"
#include "compiler/compiler.h"
#include "sema/sema_context.h"
#include "gtest/gtest.h"
#include "test_env.h"
#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>

const char *module_name = "test_ir";

FILE *open_file_from_string(const char *code)
{
    char *file_content = (char *)code;
    return fmemopen(file_content, strlen(file_content), "r");
}

void make_module_ir(void* mod, const char *module_name, const char *ir_string, char *module_ir)
{
    LLVMModuleRef module = (LLVMModuleRef)mod;
    const char *target_triple = LLVMGetDefaultTargetTriple();
    sprintf(module_ir, R"(; ModuleID = '%s'
source_filename = "%s"
target datalayout = "%s"
target triple = "%s"
%s)",
        module_name, module_name, LLVMGetDataLayoutStr(module), target_triple, ir_string);
    free((void*)target_triple);
}

void validate_m_code_with_ir_code(struct engine* engine, const char *m_code, const char *expected_ir_code)
{
    char expected_module_ir[1024 * 4];
    void* module = create_ir_module(engine->be->cg, module_name);
    make_module_ir(module, module_name, expected_ir_code, expected_module_ir);
    struct ast_node *block = parse_code(engine->fe->parser, m_code);
    char *ir_string = engine->emit_ir_string(engine->be->cg, block);
    ASSERT_STREQ(expected_module_ir, ir_string);
    node_free(block);
    free_ir_string(ir_string);
}
