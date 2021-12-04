/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser logical operators
 */
#include "codegen/env.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

class testParserRelational : public TestBase {
};

TEST_F(testParserRelational, testComparisonLessThan)
{
    char test_code[] = "10 < 11";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (ast_node *)node;
    ASSERT_STREQ("<", string_get(bin->binop->op));
    env_free(env);
}

TEST_F(testParserRelational, testComparisonGreaterThan)
{
    char test_code[] = "11 > 10";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ(">", string_get(node->binop->op));
    env_free(env);
}

TEST_F(testParserRelational, testComparisonEqual)
{
    char test_code[] = "11==10";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("==", string_get(node->binop->op));
    env_free(env);
}

TEST_F(testParserRelational, testComparisonGE)
{
    char test_code[] = "11>=10";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ(">=", string_get(node->binop->op));
    env_free(env);
}

TEST_F(testParserRelational, testComparisonLE)
{
    char test_code[] = "11<=10";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("<=", string_get(node->binop->op));
    env_free(env);
}
