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
    block_node* block = parse_block(parser, nullptr);
    ASSERT_EQ(BINARY_NODE, block->nodes[0]->node_type);
    auto node = (binary_node*)block->nodes[0];
    ASSERT_STREQ("<", node->op.data);
    parser_free(parser);
}

TEST(testParserLogical, testComparisonGreaterThan)
{
    char test_code[] = "11 > 10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, nullptr);
    ASSERT_EQ(BINARY_NODE, block->nodes[0]->node_type);
    auto node = (binary_node*)block->nodes[0];
    ASSERT_STREQ(">", node->op.data);
    parser_free(parser);
}

TEST(testParserLogical, testComparisonEqual)
{
    char test_code[] = "11==10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, nullptr);
    ASSERT_EQ(BINARY_NODE, block->nodes[0]->node_type);
    auto node = (binary_node*)block->nodes[0];
    ASSERT_STREQ("==", node->op.data);
    parser_free(parser);
}

TEST(testParserLogical, testComparisonGE)
{
    char test_code[] = "11>=10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, nullptr);
    ASSERT_EQ(BINARY_NODE, block->nodes[0]->node_type);
    auto node = (binary_node*)block->nodes[0];
    ASSERT_STREQ(">=", node->op.data);
    parser_free(parser);
}

TEST(testParserLogical, testComparisonLE)
{
    char test_code[] = "11<=10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, nullptr);
    ASSERT_EQ(BINARY_NODE, block->nodes[0]->node_type);
    auto node = (binary_node*)block->nodes[0];
    ASSERT_STREQ("<=", node->op.data);
    parser_free(parser);
}
