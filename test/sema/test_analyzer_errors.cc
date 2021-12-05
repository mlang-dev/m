/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "codegen/codegen.h"
#include "codegen/env.h"
#include "parser/m_parser.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testAnalyzerError, testNoFunctionFound)
{
    char test_code[] = R"(
no_exist_function_call ()
)";
    testing::internal::CaptureStderr();
    env *env = env_new(false);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    emit_code(env, (ast_node *)block);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:1: no_exist_function_call not defined\n", error.c_str());
    env_free(env);
}

TEST(testAnalyzerError, testRemError)
{
    char test_code[] = R"(
10+0.3
)";
    testing::internal::CaptureStderr();
    env *env = env_new(false);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    emit_code(env, (ast_node *)block);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :2:1: type not same for binary op: +\n", error.c_str());
    env_free(env);
}

TEST(testAnalyzerError, tesTypeMismatch)
{
    char test_code[] = R"(
x:int = true
)";
    testing::internal::CaptureStderr();
    env *env = env_new(false);
    ast_node *block = parse_string(env->cg->sema_context->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    emit_code(env, (ast_node *)block);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :2:1: variable type not matched with literal constant\n", error.c_str());
    env_free(env);
}
