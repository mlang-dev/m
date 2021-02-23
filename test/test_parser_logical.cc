/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser logical operators
 */
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>
#include "test_base.h"

class testParserLogicalOperator : public TestBase {};

TEST_F(testParserLogicalOperator, testOrOp)
{
    char test_code[] = "true || false";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (binary_node*)node;
    ASSERT_STREQ("||", string_get(bin->op));
    parser_free(parser);
}

TEST_F(testParserLogicalOperator, testAndOp)
{
    char test_code[] = "true && false";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (binary_node*)node;
    ASSERT_STREQ("&&", string_get(bin->op));
    parser_free(parser);
}

TEST_F(testParserLogicalOperator, testNotOp)
{
    char test_code[] = "! true";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(UNARY_NODE, node->node_type);
    auto bin = (binary_node*)node;
    ASSERT_STREQ("!", string_get(bin->op));
    parser_free(parser);
}
