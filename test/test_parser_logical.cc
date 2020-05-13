/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser logical operators
 */
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>


TEST(testParserLogicalOperator, testOrOp)
{
    char test_code[] = "true || false";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (binary_node*)node;
    ASSERT_STREQ("||", string_get(&bin->op));
    parser_free(parser);
}

TEST(testParserLogicalOperator, testAndOp)
{
    char test_code[] = "true && false";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (binary_node*)node;
    ASSERT_STREQ("&&", string_get(&bin->op));
    parser_free(parser);
}

TEST(testParserLogicalOperator, testNotOp)
{
    char test_code[] = "! true";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(UNARY_NODE, node->node_type);
    auto bin = (binary_node*)node;
    ASSERT_STREQ("!", string_get(&bin->op));
    parser_free(parser);
}
