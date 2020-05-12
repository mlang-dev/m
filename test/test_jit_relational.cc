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

TEST(testJITLogical, testLessThan)
{
    const char test_code[] = R"(
  10<11
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testLessThanWrong)
{
    char test_code[] = R"(
  11<10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testGreaterThan)
{
    char test_code[] = R"(
  11>10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    env_free(env);
}

TEST(testJITLogical, testGreaterThanWrong)
{
    char test_code[] = R"(
  10>11
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testEqual)
{
    char test_code[] = R"(
  10==10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testEqualNot)
{
    char test_code[] = R"(
  10==11
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testNotEqualTrue)
{
    char test_code[] = R"(
  10!=11
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testNotEqualFalse)
{
    char test_code[128] = R"(
  10!=10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testLETrueL)
{
    char test_code[128] = R"(
  9<=10
  )";
     menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testLETrueE)
{
    char test_code[] = R"(
  10<=10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testLEFalse)
{
    char test_code[] = R"(
  11<=10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testGETrueL)
{
    char test_code[] = R"(
  10>=9
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testGETrueE)
{
    char test_code[] = R"(
  10>=10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testGEFalse)
{
    char test_code[] = R"(
  10>=11
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    jit_free(jit);
    env_free(env);
}
