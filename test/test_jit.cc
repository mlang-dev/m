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

TEST(testJIT, testNumber)
{
    char test_code[] = R"(
  10
  10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node1 = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node1);
    auto node2 = *(exp_node**)array_back(&block->nodes);
    result = eval_exp(jit, node2);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(10, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testNegNumber)
{
    char test_code[] = R"(
  -10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node1 = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(-10, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testPositiveNumber)
{
    char test_code[] = R"(
  +10
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node1 = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(10, result.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testChar)
{
    char test_code[] = R"(
  'c'
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node1 = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_CHAR, result.type);
    ASSERT_EQ('c', result.c_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testString)
{
    char test_code[] = R"(
  "hello"
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node1 = *(exp_node**)array_front(&block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_STRING, result.type);
    ASSERT_STREQ("hello", result.s_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testTypeError)
{
    char test_code[] = R"(
  10 + 10.0
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    eval_statement(jit, node);
    ASSERT_EQ(0, node->type);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testGlobalVar)
{
    char test_code[] = R"(
y=100
y
)";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    eval_statement(jit, node);
    ASSERT_EQ(100, eval_exp(jit, node1).i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testGlobalVarString)
{
    char test_code[] = R"(
y="hello"
y
)";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    eval_statement(jit, node);
    ASSERT_STREQ("hello", eval_exp(jit, node1).s_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testGlobalVarAssignTwice)
{
    char test_code[] = R"(
y=100
y=200
y
)";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node1 = *(exp_node**)array_front(&block->nodes);
    auto node2 = *(exp_node**)array_get(&block->nodes, 1);
    auto node3 = *(exp_node**)array_get(&block->nodes, 2);
    eval_statement(jit, node1);
    eval_statement(jit, node2);
    ASSERT_EQ(200, eval_exp(jit, node3).i_value); 
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testIdFunc)
{
    char test_code[] = R"(
  f x = x
  f 10.0
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_back(&block->nodes);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(10.0, result.d_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testSquareFunc)
{
    char test_code[] = R"(
  f x = x * x
  f 10.0
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(100.0, result.d_value);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    auto node2 = *(exp_node**)array_get(&block->nodes, 2);
    eval_statement(jit, node);
    eval_result result1 = eval_exp(jit, node1);
    eval_result result2 = eval_exp(jit, node2);
    ASSERT_EQ(5, result1.i_value);
    ASSERT_EQ(0, result2.i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testForLoopFunc)
{
    char test_code[] = R"(
    forloop n = 
        for i in 1..n
            i
    forloop 4
  )";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_back(&block->nodes);
    eval_statement(jit, node);
    eval_result result1 = eval_exp(jit, node1);
    ASSERT_EQ(0, result1.i_value);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    auto node1 = *(exp_node**)array_get(&block->nodes, 1);
    auto node2 = *(exp_node**)array_get(&block->nodes, 2);
    eval_statement(jit, node);
    eval_statement(jit, node1);
    ASSERT_EQ(-100, eval_exp(jit, node2).i_value);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    for (int i = 0; i < 3; i++) {
        auto node = *(exp_node**)array_get(&block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(exp_node**)array_get(&block->nodes, 3);
    ASSERT_EQ(-100, eval_exp(jit, node3).i_value);
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
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto end = 3;
    for (int i = 0; i < end; i++) {
        auto node = *(exp_node**)array_get(&block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(exp_node**)array_get(&block->nodes, 3);
    ASSERT_EQ(-100, eval_exp(jit, node3).i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testLocalStringFunc)
{
    char test_code[] = R"(
to_string () = 
  x = "hello"
  y = x
  y
to_string()
)";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto end = 1;
    for (int i = 0; i < end; i++) {
        auto node = *(exp_node**)array_get(&block->nodes, i);
        eval_statement(jit, node);
    }
    auto node1 = *(exp_node**)array_get(&block->nodes, end);
    ASSERT_STREQ("hello", eval_exp(jit, node1).s_value);
    jit_free(jit);
    env_free(env);
}


TEST(testJIT, testPrintfFunc)
{
    char test_code[] = R"(
printf "hello"
)";
    menv* env = create_env_for_string(test_code);
    JIT* jit = build_jit(env);
    block_node* block = parse_block(env->parser, 0, 0, 0);
    auto end = 1;
    testing::internal::CaptureStdout();
    for (int i = 0; i < end; i++) {
        auto node = *(exp_node**)array_get(&block->nodes, i);
        eval_statement(jit, node);
    }
    auto msg = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("hello5\n", msg.c_str());
    jit_free(jit);
    env_free(env);
}
