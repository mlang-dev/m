/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for logical operators JIT
 */
#include "compiler/repl.h"
#include "parser/m_parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITRelational, testLessThan)
{
    const char test_code[] = R"(
  10<11
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    env_free(env);
}

TEST(testJITRelational, testLessThanWrong)
{
    char test_code[] = R"(
  11<10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    env_free(env);
}

TEST(testJITRelational, testGreaterThan)
{
    char test_code[] = R"(
  11>10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    env_free(env);
}

TEST(testJITRelational, testGreaterThanWrong)
{
    char test_code[] = R"(
  10>11
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testEqual)
{
    char test_code[] = R"(
  10==10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testEqualNot)
{
    char test_code[] = R"(
  10==11
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testNotEqualTrue)
{
    char test_code[] = R"(
  10!=11
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testNotEqualFalse)
{
    char test_code[] = R"(
  10!=10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testLETrueL)
{
    char test_code[] = R"(
  9<=10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testLETrueE)
{
    char test_code[] = R"(
  10<=10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testLEFalse)
{
    char test_code[] = R"(
  11<=10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testGETrueL)
{
    char test_code[] = R"(
  10>=9
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testGETrueE)
{
    char test_code[] = R"(
  10>=10
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITRelational, testGEFalse)
{
    char test_code[] = R"(
  10>=11
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}
