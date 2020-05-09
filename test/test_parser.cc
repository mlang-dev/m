/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testParser, testBlockVariable)
{
    char test_code[] = "x = 11";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(NUMBER_NODE, node->init_value->node_type);
    parser_free(parser);
}

TEST(testParser, testBool)
{
    char test_code[] = "x = true";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(NUMBER_NODE, node->init_value->node_type);
    parser_free(parser);
}

TEST(testParser, testBlockVariableNameWithUnderlyingLine)
{
    char test_code[] = "m_x = 11";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("m_x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(NUMBER_NODE, node->init_value->node_type);
    parser_free(parser);
}

TEST(testParser, testBlockIdFunction)
{
    char test_code[] = R"(
f x = x
f 10
)";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(function_node**)array_front(&block->nodes);
    auto body_node = *(exp_node**)array_front(&node->body->nodes);
    ASSERT_EQ(2, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(IDENT_NODE, body_node->node_type);
    parser_free(parser);
}

TEST(testParser, testBlockBinaryFunction)
{
    char test_code[] = "f x = x * x";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(function_node**)array_front(&block->nodes);
    auto body_node = *(exp_node**)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    parser_free(parser);
}

TEST(testParser, testBlockBinaryFunctionName)
{
    char test_code[] = "f_sq x = x * x";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(function_node**)array_front(&block->nodes);
    auto body_node = *(exp_node**)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f_sq", string_get(&node->prototype->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    parser_free(parser);
}

TEST(testParser, testFacIfCondition)
{
    char test_code[] = R"(fac n = 
    if n< 2 n
    else n * fac (n-1))";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(function_node**)array_front(&block->nodes);
    auto body_node = *(exp_node**)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("fac", string_get(&node->prototype->name));
    ASSERT_EQ(CONDITION_NODE, body_node->node_type);
    parser_free(parser);
}

TEST(testParser, testForLoop)
{
    char test_code[] = R"(loopprint n = 
  for i in 0..n
    print i
  )";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(function_node**)array_front(&block->nodes);
    auto body_node = *(exp_node**)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("loopprint", string_get(&node->prototype->name));
    ASSERT_EQ(FOR_NODE, body_node->node_type);
    parser_free(parser);
}

TEST(testParser, testVariableInFunction)
{
    char test_code[] = R"(distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
  )";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(function_node**)array_front(&block->nodes);
    auto body = *(exp_node**)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("distance", string_get(&node->prototype->name));
    ASSERT_EQ(VAR_NODE, body->node_type);
    parser_free(parser);
}

TEST(testParser, testAvgFunction)
{
    char test_code[] = R"(
avg x y = (x + y) / 2
    )";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto func = *(function_node**)array_front(&block->nodes);
    auto body_node = *(exp_node**)array_front(&func->body->nodes);
    ASSERT_STREQ("avg", string_get(&func->prototype->name));
    ASSERT_STREQ("BINARY_NODE", NodeTypeString[body_node->node_type]);
    parser_free(parser);
}

TEST(testParser, testUnaryOperatorOverloadFunction)
{
    char test_code[] = R"((|>) x = 0 - x # unary operator overloading
  )";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(FUNCTION_NODE, node->node_type);
    function_node* func = (function_node*)node;
    ASSERT_STREQ("unary|>", string_get(&func->prototype->name));
    parser_free(parser);
}

TEST(testParser, testSimpleUnaryOperatorOverloadFunction)
{
    char test_code[] = "unary|> x = 0 - x # unary operator overloading";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(FUNCTION_NODE, node->node_type);
    function_node* func = (function_node*)node;
    ASSERT_STREQ("unary|>", string_get(&func->prototype->name));
    parser_free(parser);
}

TEST(testParser, testSimpleBinaryOperatorOverloadFunction)
{
    char test_code[] = "(|>)10 x y = y < x # binary operator overloading";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_STREQ("FUNCTION_NODE", NodeTypeString[node->node_type]);
    function_node* func = (function_node*)node;
    ASSERT_STREQ("binary|>", string_get(&func->prototype->name));
    parser_free(parser);
}
