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
    //type Point2D = x:double y:double
    char test_code[] = "type Point2D = x:double y:double";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, block, "test_ir");
    printf("%s", ir_string);
    free_ir_string(ir_string);
    env_free(env);
}
