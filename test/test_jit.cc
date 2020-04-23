/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for JIT
 */
#include "parser.h"
#include "repl.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJIT, testIdFunc)
{
    char test_code[] = R"(
  f x = x
  f 10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, nullptr);
    eval_statement(jit, block->nodes[0]);
    auto result = eval_exp(jit, block->nodes[1]);
    ASSERT_EQ(10.0, result);
    jit_free(jit);
    env_free(env);
}

// TEST(testJIT, testSqrtFunc)
// {
//     char test_code[] = R"(
//   f x = x * x
//   f 10
//   )";
//     auto parser = create_parser_for_string(test_code);
//     menv* env = env_new();
//     JIT* jit = build_jit(env, parser);
//     block_node* block = parse_block(parser, nullptr);
//     eval_statement(jit, block->nodes[0]);
//     auto result = eval_exp(jit, block->nodes[1]);
//     ASSERT_EQ(100.0, result);
//     jit_free(jit);
//     env_free(env);
// }

// TEST(testJIT, testIfFunc)
// {
//     char test_code[] = R"(
//   f x = 
//     if x < 10 then x
//     else 0
//   f 5
//   f 10
//   )";
//     auto parser = create_parser_for_string(test_code);
//     menv* env = env_new();
//     JIT* jit = build_jit(env, parser);
//     block_node* block = parse_block(parser, nullptr);
//     eval_statement(jit, block->nodes[0]);
//     ASSERT_EQ(5.0, eval_exp(jit, block->nodes[1]));
//     ASSERT_EQ(0, eval_exp(jit, block->nodes[2]));
//     jit_free(jit);
//     env_free(env);
// }

// TEST(testJIT, testGloVarFunc)
// {
//     char test_code[] = R"(
// y=100
// y
// )";
//     auto parser = create_parser_for_string(test_code);
//     menv* env = env_new();
//     JIT* jit = build_jit(env, parser);
//     block_node* block = parse_block(parser, nullptr);
//     eval_statement(jit, block->nodes[0]);
//     ASSERT_EQ(100.0, eval_exp(jit, block->nodes[1]));
//     jit_free(jit);
//     env_free(env);
// }

// TEST(testJIT, testUnaryFunc)
// {
//     char test_code[] = R"(
// unary- x = 0 - x
// y=100
// -y
// )";
//     auto parser = create_parser_for_string(test_code);
//     menv* env = env_new();
//     JIT* jit = build_jit(env, parser);
//     block_node* block = parse_block(parser, nullptr);
//     eval_statement(jit, block->nodes[0]);
//     eval_statement(jit, block->nodes[1]);
//     ASSERT_EQ(-100.0, eval_exp(jit, block->nodes[2]));
//     jit_free(jit);
//     env_free(env);
// }

// TEST(testJIT, testBinaryFunc)
// {
//     char test_code[] = R"(
// unary- x = 0 - x
// binary>10 x y = y < x
// z = 100
// if z>99 then -z else z
// )";
//     auto parser = create_parser_for_string(test_code);
//     menv* env = env_new();
//     JIT* jit = build_jit(env, parser);
//     block_node* block = parse_block(parser, nullptr);
//     for (int i = 0; i < 3; i++)
//         eval_statement(jit, block->nodes[i]);
//     ASSERT_EQ(-100.0, eval_exp(jit, block->nodes[3]));
//     jit_free(jit);
//     env_free(env);
// }

// TEST(testJIT, testUnaryBinaryFunc)
// {
//     char test_code[] = R"(
// (-) x = 0 - x # unary operator overloading
// (>)10 x y = y < x # binary operator overloading
// z = 100
// if z>99 then -z else z
// )";
//     auto parser = create_parser_for_string(test_code);
//     menv* env = env_new();
//     JIT* jit = build_jit(env, parser);
//     block_node* block = parse_block(parser, nullptr);
//     auto end = 3;
//     for (int i = 0; i < end; i++)
//         eval_statement(jit, block->nodes[i]);
//     ASSERT_EQ(-100.0, eval_exp(jit, block->nodes[end]));
//     jit_free(jit);
//     env_free(env);
// }
