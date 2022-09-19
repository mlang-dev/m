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
#include "parser/grammar.h"
#include "sema/sema_context.h"
#include "gtest/gtest.h"
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
    sprintf(module_ir, R"(; ModuleID = '%s'
source_filename = "%s"
target datalayout = "%s"
target triple = "%s"
%s)",
        module_name, module_name, LLVMGetDataLayoutStr(module), LLVMGetDefaultTargetTriple(), ir_string);
}

void validate_m_code_with_ir_code(const char *m_code, const char *ir_code)
{
    char module_ir[1024 * 4];
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, module_name);
    make_module_ir(cg->module, module_name, ir_code, module_ir);
    struct ast_node *block;
    block = parse_code(engine->fe->parser, m_code);
    char *ir_string = emit_ir_string(cg, block);
    ASSERT_STREQ(module_ir, ir_string);
    //ast_node_free(block);
    free_ir_string(ir_string);
    engine_free(engine);
}
