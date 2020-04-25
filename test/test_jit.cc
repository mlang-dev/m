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
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(10.0, result);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testSqrtFunc)
{
    char test_code[] = R"(
  f x = x * x
  f 10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(100.0, result);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testIfFunc)
{
    char test_code[] = R"(
  f x = 
    if x < 10 then x
    else 0
  f 5
  f 10
  )";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    auto node2 = *(exp_node**)array_get(&block->nodes, 2);
    eval_statement(jit, node);
    ASSERT_EQ(5.0, eval_exp(jit, node1));
    ASSERT_EQ(0, eval_exp(jit, node2));
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testGloVarFunc)
{
    char test_code[] = R"(
y=100
y
)";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    eval_statement(jit, node);
    ASSERT_EQ(100.0, eval_exp(jit, node1));
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testUnaryFunc)
{
    char test_code[] = R"(
unary- x = 0 - x
y=100
-y
)";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    auto node2 = *(exp_node**)array_get(&block->nodes, 2);
    eval_statement(jit, node);
    eval_statement(jit, node1);
    ASSERT_EQ(-100.0, eval_exp(jit, node2));
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testBinaryFunc)
{
    char test_code[] = R"(
unary- x = 0 - x
binary>10 x y = y < x
z = 100
if z>99 then -z else z
)";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    for (int i = 0; i < 3; i++){
        auto node = *(exp_node**)array_get(&block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(exp_node**)array_get(&block->nodes, 3);
    ASSERT_EQ(-100.0, eval_exp(jit, node3));
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testUnaryBinaryFunc)
{
    char test_code[] = R"(
(-) x = 0 - x # unary operator overloading
(>)10 x y = y < x # binary operator overloading
z = 100
if z>99 then -z else z
)";
    auto parser = create_parser_for_string(test_code);
    menv* env = env_new();
    JIT* jit = build_jit(env, parser);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    auto end = 3;
    for (int i = 0; i < end; i++){
        auto node = *(exp_node**)array_get(&block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(exp_node**)array_get(&block->nodes, 3);
    ASSERT_EQ(-100.0, eval_exp(jit, node3));
    jit_free(jit);
    env_free(env);
}
