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
    auto parser = parser_new(false);
    FILE* file = open_file_from_string(test_code);
    block_node* block = parse_file_object(parser, "test", file);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node* literal = (struct literal_node*)node->init_value;
    ASSERT_EQ(11, literal->int_val);
    parser_free(parser);
}

TEST(testParser, testVariableWithType)
{
    char test_code[] = "x:int = 11";
    auto parser = parser_new(false);
    FILE* file = open_file_from_string(test_code);
    block_node* block = parse_file_object(parser, "test", file);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(TYPE_INT, node->base.annotated_type->type);
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node* literal = (struct literal_node*)node->init_value;
    ASSERT_EQ(11, literal->int_val);
    parser_free(parser);
}

TEST(testParser, testBool)
{
    char test_code[] = "x = true";
    auto parser = parser_new(false);
    FILE* file = open_file_from_string(test_code);
    block_node* block = parse_file_object(parser, "test", file);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node* literal = (struct literal_node*)node->init_value;
    ASSERT_EQ(true, literal->bool_val);
    parser_free(parser);
}

TEST(testParser, testChar)
{
    char test_code[] = "x = 'c'";
    auto parser = parser_new(false);
    FILE* file = open_file_from_string(test_code);
    block_node* block = parse_file_object(parser, "test", file);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node* literal = (struct literal_node*)node->init_value;
    ASSERT_EQ('c', literal->char_val);
    parser_free(parser);
}

TEST(testParser, testString)
{
    char test_code[] = "x = \"hello world!\"";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node* literal = (struct literal_node*)node->init_value;
    ASSERT_STREQ("hello world!", literal->str_val);
    parser_free(parser);
}

TEST(testParser, testBlockVariableNameWithUnderlyingLine)
{
    char test_code[] = "m_x = 11";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("m_x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    parser_free(parser);
}

TEST(testParser, testBlockIdFunction)
{
    char test_code[] = R"(
f x = x
f 10
)";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
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
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
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
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
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
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
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
  for i in 3..n
    print i
  )";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(function_node**)array_front(&block->nodes);
    for_node* body_node = *(for_node**)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("loopprint", string_get(&node->prototype->name));
    ASSERT_EQ(FOR_NODE, body_node->base.node_type);
    ASSERT_EQ(TYPE_INT, body_node->start->annotated_type->type);
    ASSERT_EQ(TYPE_INT, body_node->step->annotated_type->type);
    ASSERT_EQ(BINARY_NODE, body_node->end->node_type);
    ASSERT_EQ(3, ((literal_node*)body_node->start)->int_val);
    parser_free(parser);
}

TEST(testParser, testVariableInFunction)
{
    char test_code[] = R"(distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
  )";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
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
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto func = *(function_node**)array_front(&block->nodes);
    auto body_node = *(exp_node**)array_front(&func->body->nodes);
    ASSERT_STREQ("avg", string_get(&func->prototype->name));
    ASSERT_STREQ("BINARY_NODE", node_type_strings[body_node->node_type]);
    parser_free(parser);
}

TEST(testParser, testUnaryOperatorOverloadFunction)
{
    char test_code[] = R"((|>) x = 0 - x # unary operator overloading
  )";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(FUNCTION_NODE, node->node_type);
    function_node* func = (function_node*)node;
    ASSERT_STREQ("unary|>", string_get(&func->prototype->name));
    parser_free(parser);
}

TEST(testParser, testSimpleUnaryOperatorOverloadFunction)
{
    char test_code[] = "unary|> x = 0 - x # unary operator overloading";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(FUNCTION_NODE, node->node_type);
    function_node* func = (function_node*)node;
    ASSERT_STREQ("unary|>", string_get(&func->prototype->name));
    parser_free(parser);
}

TEST(testParser, testSimpleBinaryOperatorOverloadFunction)
{
    char test_code[] = "(|>)10 x y = y < x # binary operator overloading";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_STREQ("FUNCTION_NODE", node_type_strings[node->node_type]);
    function_node* func = (function_node*)node;
    ASSERT_STREQ("binary|>", string_get(&func->prototype->name));
    parser_free(parser);
}

TEST(testParser, testVariadicFunction)
{
    char test_code[] = "f x ... = 10";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_STREQ("FUNCTION_NODE", node_type_strings[node->node_type]);
    function_node* func = (function_node*)node;
    ASSERT_EQ(true, func->prototype->is_variadic);
    parser_free(parser);
}

TEST(testParser, testVariadicFunctionInvalidPosition)
{
    testing::internal::CaptureStderr();
    char test_code[] = "f ... x = 10";
    auto parser = parser_new(false);
    parse_string(parser, "", test_code);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:7: no parameter allowed after variadic\n", error.c_str());
    parser_free(parser);
}

TEST(testParser, testLocalStringFunc)
{
    char test_code[] = R"(
to_string () = 
  x = "hello"
  y = x
  y
)";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(FUNCTION_NODE, node->node_type);
    function_node* fun = (function_node*)node;
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("to_string", string_get(&fun->prototype->name));
    parser_free(parser);
}

TEST(testParser, testPrototypeNode)
{
    char test_code[] = "extern printf(format:string ...):int";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(PROTOTYPE_NODE, node->node_type);
    prototype_node* proto = (prototype_node*)node;
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("printf", string_get(&proto->name));
    parser_free(parser);
}

TEST(testParser, testPrototypeNodeEmptyArg)
{
    char test_code[] = "extern print():int";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(PROTOTYPE_NODE, node->node_type);
    prototype_node* proto = (prototype_node*)node;
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(0, array_size(&proto->fun_params));
    ASSERT_STREQ("print", string_get(&proto->name));
    string proto_type = to_string(proto->base.annotated_type);
    ASSERT_STREQ("int", string_get(&proto_type));
    parser_free(parser);
}

TEST(testParser, testStructsType)
{
    char test_code[] = R"(
type Point2D = 
  x:double
  y:double
)";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    type_node* type = (type_node*)node;
    ASSERT_STREQ("Point2D", string_get(&type->name));
    ASSERT_EQ(2, array_size(&type->body->nodes));
    struct var_node* var1 = *(struct var_node**)array_front(&type->body->nodes);
    struct var_node* var2 = *(struct var_node**)array_back(&type->body->nodes); 
    ASSERT_EQ(VAR_NODE, var1->base.node_type);
    ASSERT_EQ(VAR_NODE, var2->base.node_type);
    ASSERT_STREQ("x", string_get(&var1->var_name));
    ASSERT_STREQ("y", string_get(&var2->var_name));
    parser_free(parser);
}

TEST(testParser, testStructsTypeOneLine)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy:Point2D = 0.0 0.0
)";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(2, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    type_node* type = (type_node*)node;
    ASSERT_STREQ("Point2D", string_get(&type->name));
    ASSERT_EQ(2, array_size(&type->body->nodes));
    struct var_node* var1 = *(struct var_node**)array_front(&type->body->nodes);
    struct var_node* var2 = *(struct var_node**)array_back(&type->body->nodes); 
    ASSERT_EQ(VAR_NODE, var1->base.node_type);
    ASSERT_EQ(VAR_NODE, var2->base.node_type);
    ASSERT_STREQ("x", string_get(&var1->var_name));
    ASSERT_STREQ("y", string_get(&var2->var_name));
    parser_free(parser);
}

TEST(testParser, testStructsTypeDefAndDecl)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy:Point2D = 0.0 0.0
)";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(2, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    node = *(exp_node**)array_back(&block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct var_node* var = (struct var_node*)node;
    ASSERT_STREQ("xy", string_get(&var->var_name));
    ASSERT_STREQ("Point2D", string_get(var->base.annotation));
    parser_free(parser);
}
