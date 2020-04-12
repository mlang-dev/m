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
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_parser(parser);
}

TEST(testJITLogical, testLessThanWrong)
{
    char test_code[] = R"(
  11<10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(0, result);
    destroy_parser(parser);
}

TEST(testJITLogical, testGreaterThan)
{
    char test_code[] = R"(
  11>10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_parser(parser);
}

TEST(testJITLogical, testGreaterThanWrong)
{
    char test_code[] = R"(
  10>11
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(0, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testEqual)
{
    char test_code[] = R"(
  10==10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testEqualNot)
{
    char test_code[] = R"(
  10==11
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(0, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testNotEqualTrue)
{
    char test_code[] = R"(
  10!=11
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testNotEqualFalse)
{
    char test_code[128] = R"(
  10!=10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(0, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testLETrueL)
{
    char test_code[128] = R"(
  9<=10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testLETrueE)
{
    char test_code[] = R"(
  10<=10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testLEFalse)
{
    char test_code[] = R"(
  11<=10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(0, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testGETrueL)
{
    char test_code[] = R"(
  10>=9
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testGETrueE)
{
    char test_code[] = R"(
  10>=10
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(1, result);
    destroy_jit(jit);
}

TEST(testJITLogical, testGEFalse)
{
    char test_code[] = R"(
  10>=11
  )";
    auto parser = create_parser_for_string(test_code);
    JIT* jit = build_jit(parser);
    block_node* block = parse_block(parser, nullptr);
    auto result = eval_exp(jit, block->nodes[0]);
    ASSERT_EQ(0, result);
    destroy_jit(jit);
}
