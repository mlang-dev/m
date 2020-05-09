/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser logical operators
 */
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testParserLogical, testComparisonLessThan)
{
    char test_code[] = "10 < 11";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    auto bin = (binary_node*)node;
    ASSERT_STREQ("<", string_get(&bin->op));
    parser_free(parser);
}

TEST(testParserLogical, testComparisonGreaterThan)
{
    char test_code[] = "11 > 10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(binary_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->base.node_type);
    ASSERT_STREQ(">", string_get(&node->op));
    parser_free(parser);
}

TEST(testParserLogical, testComparisonEqual)
{
    char test_code[] = "11==10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(binary_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->base.node_type);
    ASSERT_STREQ("==", string_get(&node->op));
    parser_free(parser);
}

TEST(testParserLogical, testComparisonGE)
{
    char test_code[] = "11>=10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(binary_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->base.node_type);
    ASSERT_STREQ(">=", string_get(&node->op));
    parser_free(parser);
}

TEST(testParserLogical, testComparisonLE)
{
    char test_code[] = "11<=10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(binary_node**)array_front(&block->nodes);
    ASSERT_EQ(BINARY_NODE, node->base.node_type);
    ASSERT_STREQ("<=", string_get(&node->op));
    parser_free(parser);
}
