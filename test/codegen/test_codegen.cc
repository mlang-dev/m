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

TEST_F(testCodeGenerator, testStructTypeDefIR)
{
    char test_code[] = "m = 10";
    env* env = env_new(false);
    create_ir_module(env->cg, "test_ir");
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    printf("%s", ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

