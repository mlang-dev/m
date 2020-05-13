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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    env_free(env);
}
