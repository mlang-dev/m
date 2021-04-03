/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for code generating
 */
#include "gtest/gtest.h"
#include <stdio.h>
#include "test_base.h"
#include "sema/env.h"
#include "sema/analyzer.h"
#include "compiler/compiler.h"

class testCodeGenerator : public TestBase {};
const char *module_name = "test_ir";

void _make_module_ir(LLVMModuleRef module, const char *ir_string, char *module_ir)
{
    sprintf(module_ir, R"(; ModuleID = '%s'
source_filename = "%s"
target datalayout = "%s"
target triple = "%s"

%s
)", module_name, module_name, LLVMGetDataLayoutStr(module), LLVMGetDefaultTargetTriple(), ir_string);
}

TEST_F(testCodeGenerator, testGlobalVar)
{
    char module_ir[1024];
    char test_code[] = "m = 10";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    _make_module_ir(env->cg->module, "@m = externally_initialized global i32 0", module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

