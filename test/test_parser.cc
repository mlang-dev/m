#include "gtest/gtest.h"
#include "parser.h"
#include "tutil.h"
#include <stdio.h>


TEST(testParser, testBlockVariable){
  char test_code[] = "x = 11";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.node_type);
  ASSERT_EQ(NUMBER_NODE, node->init_value->node_type);
  destroy_parser(parser);
}

TEST(testParser, testBlockVariableNameWithUnderlyingLine){
  char test_code[] = "m_x = 11";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("m_x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.node_type);
  ASSERT_EQ(NUMBER_NODE, node->init_value->node_type);
  destroy_parser(parser);
}

TEST(testParser, testBlockIdFunction){
  char test_code[] = R"(
f x = x
f 10
)";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(2, block->nodes.size());
  ASSERT_STREQ("f", node->prototype->name.c_str());
  ASSERT_EQ(IDENT_NODE, body->node_type);
  auto app = node->body->nodes[1];
  destroy_parser(parser);
}

TEST(testParser, testBlockBinaryFunction){
  char test_code[] = "f x = x * x";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("f", node->prototype->name.c_str());
  ASSERT_EQ(BINARY_NODE, body->node_type);
  destroy_parser(parser);
}

TEST(testParser, testBlockBinaryFunctionName){
  char test_code[] = "f_sq x = x * x";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("f_sq", node->prototype->name.c_str());
  ASSERT_EQ(BINARY_NODE, body->node_type);
  destroy_parser(parser);
}

TEST(testParser, testFacIfCondition){
  char test_code[] = R"(fac n = 
    if n< 2 n
    else n * fac (n-1))";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("fac", node->prototype->name.c_str());
  ASSERT_EQ(CONDITION_NODE, body->node_type);
  destroy_parser(parser);
}

TEST(testParser, testForLoop){
  char test_code[] = R"(loopprint n = 
  for i in 0..n
    print i
  )";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("loopprint", node->prototype->name.c_str());
  ASSERT_EQ(FOR_NODE, body->node_type);
  destroy_parser(parser);
}

TEST(testParser, testVariableInFunction){
  char test_code[] = R"(distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
  )";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("distance", node->prototype->name.c_str());
  ASSERT_EQ(VAR_NODE, body->node_type);
  destroy_parser(parser);
}

TEST(testParser, testAvgFunction){
  char test_code[] = R"(
avg x y = (x + y) / 2
    )";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  function_node* func = (function_node*)block->nodes[0];
  ASSERT_STREQ("avg", func->prototype->name.c_str());
  ASSERT_STREQ("BINARY_NODE", NodeTypeString[func->body->nodes[0]->node_type]);
  destroy_parser(parser);
}

TEST(testParser, testUnaryOperatorOverloadFunction){
  char test_code[] = R"((|>) x = 0 - x # unary operator overloading
  )";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  exp_node* node = block->nodes[0];
  ASSERT_EQ(FUNCTION_NODE, node->node_type);
  function_node* func = (function_node*)node;
  ASSERT_STREQ("unary|>", func->prototype->name.c_str());
  destroy_parser(parser);
}

TEST(testParser, testSimpleUnaryOperatorOverloadFunction){
  char test_code[] = "unary|> x = 0 - x # unary operator overloading";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  exp_node* node = block->nodes[0];
  ASSERT_EQ(FUNCTION_NODE, node->node_type);
  function_node* func = (function_node*)node;
  ASSERT_STREQ("unary|>", func->prototype->name.c_str());
  destroy_parser(parser);
}

TEST(testParser, testSimpleBinaryOperatorOverloadFunction){
  char test_code[] = "(|>)10 x y = y < x # binary operator overloading";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  exp_node* node = block->nodes[0];
  ASSERT_STREQ("FUNCTION_NODE", NodeTypeString[node->node_type]);
  function_node* func = (function_node*)node;
  ASSERT_STREQ("binary|>", func->prototype->name.c_str());
  destroy_parser(parser);
}
