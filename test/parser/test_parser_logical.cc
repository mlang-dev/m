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

class testParserLogicalOperator : public TestBase {
};

TEST_F(testParserLogicalOperator, testOrOp)
{
    char test_code[] = "true || false";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (ast_node *)node;
    ASSERT_STREQ("||", get_opcode(bin->binop->opcode));
    env_free(env);
}

TEST_F(testParserLogicalOperator, testAndOp)
{
    char test_code[] = "true && false";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (ast_node *)node;
    ASSERT_STREQ("&&", get_opcode(bin->binop->opcode));
    env_free(env);
}

TEST_F(testParserLogicalOperator, testNotOp)
{
    char test_code[] = "! true";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNARY_NODE, node->node_type);
    auto un = (ast_node *)node;
    ASSERT_STREQ("!", get_opcode(un->unop->opcode));
    env_free(env);
}
