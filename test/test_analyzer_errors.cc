/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "analyzer.h"
#include "codegen.h"
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testAnalyzerError, testNoFunctionFound)
{
    char test_code[] = R"(
no_exist_function_call ()
)";
    testing::internal::CaptureStderr();
    menv* menv = env_new(false);
    block_node* block = parse_string(menv->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:1: no_exist_function_call not defined\n", error.c_str());
    env_free(menv);
}

TEST(testAnalyzerError, testRemError)
{
    char test_code[] = R"(
10+0.3
)";
    testing::internal::CaptureStderr();
    menv* menv = env_new(false);
    block_node* block = parse_string(menv->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :2:1: type not same for binary op: +\n", error.c_str());
    env_free(menv);
}

TEST(testAnalyzerError, tesTypeMismatch)
{
    char test_code[] = R"(
x:int = true
)";
    testing::internal::CaptureStderr();
    menv* menv = env_new(false);
    block_node* block = parse_string(menv->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :2:1: variable type not matched with literal constant\n", error.c_str());
    env_free(menv);
}
