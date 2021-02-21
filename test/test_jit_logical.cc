/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for logical operators JIT
 */
#include "parser.h"
#include "repl.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITLogical, testOrTrueTrue)
{
    const char test_code[] = R"(
  true || true
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testOrTrueFalse)
{
    const char test_code[] = R"(
  true || false
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testOrFalseFalse)
{
    const char test_code[] = R"(
  false || false
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testAndTrueTrue)
{
    const char test_code[] = R"(
  true && true
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testAndTrueFalse)
{
    const char test_code[] = R"(
  true && false
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testAndFalseFalse)
{
    const char test_code[] = R"(
  false && false
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testNotFalse)
{
    const char test_code[] = R"(
  ! false
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testNotTrue)
{
    const char test_code[] = R"(
  ! true
  )";
    env* env = env_new(false);
    JIT* jit = build_jit(env);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    env_free(env);
}
