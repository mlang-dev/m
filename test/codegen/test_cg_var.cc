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
#include "tutil.h"

class testCGVar : public TestBase {};

TEST_F(testCGVar, testGlobalVarInt)
{
    char module_ir[1024];
    char test_code[] = "m = 10";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    const char *expected_ir = R"(
@m = global i32 10
)";
    make_module_ir(env->cg->module, module_name, expected_ir, module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

TEST_F(testCGVar, testGlobalVarChar)
{
    char module_ir[1024];
    char test_code[] = "m = 'm'";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    const char *expected_ir = R"(
@m = global i8 109
)";
    make_module_ir(env->cg->module, module_name, expected_ir, module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

TEST_F(testCGVar, testGlobalVarBool)
{
    char module_ir[1024];
    char test_code[] = "m = true";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    const char *expected_ir = R"(
@m = global i1 true
)";
    make_module_ir(env->cg->module, module_name, expected_ir, module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

TEST_F(testCGVar, testGlobalVarDouble)
{
    char module_ir[1024];
    char test_code[] = "m = 10.0";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    const char *expected_ir = R"(
@m = global double 1.000000e+01
)";
    make_module_ir(env->cg->module, module_name, expected_ir, module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

TEST_F(testCGVar, testGlobalVarString)
{
    char module_ir[1024];
    char test_code[] = R"(m = "10.0")";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    const char *expected_ir = R"(
@0 = private unnamed_addr constant [5 x i8] c"10.0\00", align 1
@m = global i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0)
)";
    make_module_ir(env->cg->module, module_name, expected_ir, module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

TEST_F(testCGVar, testGlobalVarStruct)
{
    char module_ir[1024];
    char test_code[] = R"(
type Point2D = x:double y:double
point:Point2D
)";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    const char *expected_ir = R"(
%Point2D = type { double, double }

@point = global %Point2D zeroinitializer
)";
    make_module_ir(env->cg->module, module_name, expected_ir, module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}

TEST_F(testCGVar, testGlobalVarStructInitializer)
{
    char module_ir[1024];
    char test_code[] = R"(
type Point2D = x:int y:int
point:Point2D = 10 20
)";
    env* env = env_new(false);
    create_ir_module(env->cg, module_name);
    const char *expected_ir = R"(
%Point2D = type { i32, i32 }

@point = global %Point2D { i32 10, i32 20 }
)";
    make_module_ir(env->cg->module, module_name, expected_ir, module_ir);
    block_node* block = parse_string(env->parser, "test", test_code);
    char *ir_string = emit_ir_string(env, &block->base);
    ASSERT_STREQ(module_ir, ir_string);
    free_ir_string(ir_string);
    env_free(env);
}
