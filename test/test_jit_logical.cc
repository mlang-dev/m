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
    char test_code[] = R"(
  10<11
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    parser_free(parser);
    env_free(env);
}

TEST(testJITLogical, testLessThanWrong)
{
    char test_code[] = R"(
  11<10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.d_value);
    parser_free(parser);
    env_free(env);
}

TEST(testJITLogical, testGreaterThan)
{
    char test_code[] = R"(
  11>10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    parser_free(parser);
    env_free(env);
}

TEST(testJITLogical, testGreaterThanWrong)
{
    char test_code[] = R"(
  10>11
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testEqual)
{
    char test_code[] = R"(
  10==10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testEqualNot)
{
    char test_code[] = R"(
  10==11
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testNotEqualTrue)
{
    char test_code[] = R"(
  10!=11
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testNotEqualFalse)
{
    char test_code[128] = R"(
  10!=10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testLETrueL)
{
    char test_code[128] = R"(
  9<=10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testLETrueE)
{
    char test_code[] = R"(
  10<=10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testLEFalse)
{
    char test_code[] = R"(
  11<=10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testGETrueL)
{
    char test_code[] = R"(
  10>=9
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testGETrueE)
{
    char test_code[] = R"(
  10>=10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJITLogical, testGEFalse)
{
    char test_code[] = R"(
  10>=11
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.d_value);
    jit_free(jit);
    env_free(env);
}
