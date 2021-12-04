/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "codegen/env.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

class testParser : public TestBase {
};

TEST_F(testParser, testStructVarDecl)
{
    char test_code[] = R"(
type Point2D = x:double y:double
point:Point2D
)";
    ;
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    block_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(var_node **)array_back(&block->nodes);
    ASSERT_EQ(2, array_size(&block->nodes));
    ASSERT_STREQ("point", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(0, node->init_value);
    env_free(env);
}

TEST_F(testParser, testVarDecl)
{
    char test_code[] = "x:int";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    block_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(var_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(0, node->init_value);
    env_free(env);
}

TEST_F(testParser, testBlockVariable)
{
    char test_code[] = "x = 11";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    block_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(var_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node *literal = (struct literal_node *)node->init_value;
    ASSERT_EQ(11, literal->int_val);
    env_free(env);
}

TEST_F(testParser, testVariableWithType)
{
    char test_code[] = "x:int = 11";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    block_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(var_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(TYPE_INT, node->base.annotated_type_enum);
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node *literal = (struct literal_node *)node->init_value;
    ASSERT_EQ(11, literal->int_val);
    env_free(env);
}

TEST_F(testParser, testBool)
{
    char test_code[] = "x = true";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    block_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(var_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node *literal = (struct literal_node *)node->init_value;
    ASSERT_EQ(true, literal->bool_val);
    env_free(env);
}

TEST_F(testParser, testChar)
{
    char test_code[] = "x = 'c'";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    block_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(var_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node *literal = (struct literal_node *)node->init_value;
    ASSERT_EQ('c', literal->char_val);
    env_free(env);
}

TEST_F(testParser, testString)
{
    char test_code[] = "x = \"hello world!\"";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(var_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    struct literal_node *literal = (struct literal_node *)node->init_value;
    ASSERT_STREQ("hello world!", literal->str_val);
    env_free(env);
}

TEST_F(testParser, testBlockVariableNameWithUnderlyingLine)
{
    char test_code[] = "m_x = 11";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(var_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("m_x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(LITERAL_NODE, node->init_value->node_type);
    env_free(env);
}

TEST_F(testParser, testBlockIdFunction)
{
    char test_code[] = R"(
f x = x
f 10
)";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(function_node **)array_front(&block->nodes);
    auto body_node = *(exp_node **)array_front(&node->body->nodes);
    ASSERT_EQ(2, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(node->prototype->name));
    ASSERT_EQ(IDENT_NODE, body_node->node_type);
    env_free(env);
}

TEST_F(testParser, testBlockBinaryFunction)
{
    char test_code[] = "f x = x * x";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(function_node **)array_front(&block->nodes);
    auto body_node = *(exp_node **)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(node->prototype->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    env_free(env);
}

TEST_F(testParser, testBlockBinaryFunctionName)
{
    char test_code[] = "f_sq x = x * x";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(function_node **)array_front(&block->nodes);
    auto body_node = *(exp_node **)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f_sq", string_get(node->prototype->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    env_free(env);
}

TEST_F(testParser, testFacIfCondition)
{
    char test_code[] = R"(fac n = 
    if n< 2 n
    else n * fac (n-1))";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(function_node **)array_front(&block->nodes);
    auto body_node = *(exp_node **)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("fac", string_get(node->prototype->name));
    ASSERT_EQ(CONDITION_NODE, body_node->node_type);
    env_free(env);
}

TEST_F(testParser, testForLoop)
{
    char test_code[] = R"(loopprint n = 
  for i in 3..n
    print i
  )";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(function_node **)array_front(&block->nodes);
    for_node *body_node = *(for_node **)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->prototype->name));
    ASSERT_EQ(FOR_NODE, body_node->base.node_type);
    ASSERT_EQ(TYPE_INT, body_node->start->annotated_type_enum);
    ASSERT_EQ(TYPE_INT, body_node->step->annotated_type_enum);
    ASSERT_EQ(BINARY_NODE, body_node->end->node_type);
    ASSERT_EQ(3, ((literal_node *)body_node->start)->int_val);
    env_free(env);
}

TEST_F(testParser, testVariableInFunction)
{
    char test_code[] = R"(distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
  )";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(function_node **)array_front(&block->nodes);
    auto body = *(exp_node **)array_front(&node->body->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("distance", string_get(node->prototype->name));
    ASSERT_EQ(VAR_NODE, body->node_type);
    env_free(env);
}

TEST_F(testParser, testAvgFunction)
{
    char test_code[] = R"(
avg x y = (x + y) / 2
    )";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto func = *(function_node **)array_front(&block->nodes);
    auto body_node = *(exp_node **)array_front(&func->body->nodes);
    ASSERT_STREQ("avg", string_get(func->prototype->name));
    ASSERT_STREQ("BINARY_NODE", node_type_strings[body_node->node_type]);
    env_free(env);
}

//TEST_F(testParser, testUnaryOperatorOverloadFunction)
//{
//    char test_code[] = R"((|>) x = 0 - x # unary operator overloading
//  )";
//    auto env = env_new(false);
//    block_node *block = parse_string(env->parser, "test", test_code);
//    auto node = *(exp_node **)array_front(&block->nodes);
//    ASSERT_EQ(FUNCTION_NODE, node->node_type);
//    function_node *func = (function_node *)node;
//    ASSERT_STREQ("unary|>", string_get(func->prototype->name));
//    env_free(env);
//}
//
//TEST_F(testParser, testSimpleUnaryOperatorOverloadFunction)
//{
//    char test_code[] = "unary|> x = 0 - x # unary operator overloading";
//    auto env = env_new(false);
//    block_node *block = parse_string(env->parser, "test", test_code);
//    auto node = *(exp_node **)array_front(&block->nodes);
//    ASSERT_EQ(FUNCTION_NODE, node->node_type);
//    function_node *func = (function_node *)node;
//    ASSERT_STREQ("unary|>", string_get(func->prototype->name));
//    env_free(env);
//}
//
//TEST_F(testParser, testSimpleBinaryOperatorOverloadFunction)
//{
//    char test_code[] = "(|>)10 x y = y < x # binary operator overloading";
//    auto env = env_new(false);
//    block_node *block = parse_string(env->parser, "test", test_code);
//    auto node = *(exp_node **)array_front(&block->nodes);
//    ASSERT_STREQ("FUNCTION_NODE", node_type_strings[node->node_type]);
//    function_node *func = (function_node *)node;
//    ASSERT_STREQ("binary|>", string_get(func->prototype->name));
//    env_free(env);
//}

TEST_F(testParser, testVariadicFunction)
{
    char test_code[] = "f x ... = 10";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_STREQ("FUNCTION_NODE", node_type_strings[node->node_type]);
    function_node *func = (function_node *)node;
    ASSERT_EQ(true, func->prototype->is_variadic);
    env_free(env);
}

TEST_F(testParser, testVariadicFunctionInvalidPosition)
{
    testing::internal::CaptureStderr();
    char test_code[] = "f ... x = 10";
    auto env = env_new(false);
    parse_string(env->parser, "", test_code);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:7: no parameter allowed after variadic\n", error.c_str());
    env_free(env);
}

TEST_F(testParser, testLocalStringFunc)
{
    char test_code[] = R"(
to_string () = 
  x = "hello"
  y = x
  y
)";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(FUNCTION_NODE, node->node_type);
    function_node *fun = (function_node *)node;
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("to_string", string_get(fun->prototype->name));
    env_free(env);
}

TEST_F(testParser, testPrototypeNode)
{
    char test_code[] = "extern printf(format:string ...):int";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(PROTOTYPE_NODE, node->node_type);
    prototype_node *proto = (prototype_node *)node;
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("printf", string_get(proto->name));
    env_free(env);
}

TEST_F(testParser, testPrototypeNodeEmptyArg)
{
    char test_code[] = "extern print():int";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(PROTOTYPE_NODE, node->node_type);
    prototype_node *proto = (prototype_node *)node;
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(0, array_size(&proto->fun_params));
    ASSERT_STREQ("print", string_get(proto->name));
    ASSERT_STREQ("int", string_get(proto->base.annotated_type_name));
    env_free(env);
}

TEST_F(testParser, testStructsType)
{
    char test_code[] = R"(
type Point2D = 
  x:double
  y:double
)";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    type_node *type = (type_node *)node;
    ASSERT_STREQ("Point2D", string_get(type->name));
    ASSERT_EQ(2, array_size(&type->body->nodes));
    struct var_node *var1 = *(struct var_node **)array_front(&type->body->nodes);
    struct var_node *var2 = *(struct var_node **)array_back(&type->body->nodes);
    ASSERT_EQ(VAR_NODE, var1->base.node_type);
    ASSERT_EQ(VAR_NODE, var2->base.node_type);
    ASSERT_STREQ("x", string_get(var1->var_name));
    ASSERT_STREQ("y", string_get(var2->var_name));
    env_free(env);
}

TEST_F(testParser, testStructsTypeOneLine)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy:Point2D = 10.0 20.0
)";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(2, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    type_node *type = (type_node *)node;
    ASSERT_STREQ("Point2D", string_get(type->name));
    ASSERT_EQ(2, array_size(&type->body->nodes));
    struct var_node *var1 = *(struct var_node **)array_front(&type->body->nodes);
    struct var_node *var2 = *(struct var_node **)array_back(&type->body->nodes);
    ASSERT_EQ(VAR_NODE, var1->base.node_type);
    ASSERT_EQ(VAR_NODE, var2->base.node_type);
    ASSERT_STREQ("x", string_get(var1->var_name));
    ASSERT_STREQ("y", string_get(var2->var_name));

    //struct type variable assignement
    auto var_node = *(struct var_node **)array_back(&block->nodes);
    ASSERT_EQ(VAR_NODE, var_node->base.node_type);
    ASSERT_EQ(TYPE_VALUE_NODE, var_node->init_value->node_type);
    auto value_node = (struct type_value_node *)var_node->init_value;
    auto value1 = *(struct literal_node **)array_front(&value_node->body->nodes);
    auto value2 = *(struct literal_node **)array_back(&value_node->body->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->base.node_type);
    ASSERT_EQ(LITERAL_NODE, value2->base.node_type);
    ASSERT_EQ(10.0, value1->double_val);
    ASSERT_EQ(20.0, value2->double_val);
    env_free(env);
}

TEST_F(testParser, testStructsTypeOneLineAnotherTypeConstructor)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy = Point2D 10.0 20.0
)";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(2, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    type_node *type = (type_node *)node;
    ASSERT_STREQ("Point2D", string_get(type->name));
    ASSERT_EQ(2, array_size(&type->body->nodes));
    struct var_node *var1 = *(struct var_node **)array_front(&type->body->nodes);
    struct var_node *var2 = *(struct var_node **)array_back(&type->body->nodes);
    ASSERT_EQ(VAR_NODE, var1->base.node_type);
    ASSERT_EQ(VAR_NODE, var2->base.node_type);
    ASSERT_STREQ("x", string_get(var1->var_name));
    ASSERT_STREQ("y", string_get(var2->var_name));

    //struct type variable assignement
    auto var_node = *(struct var_node **)array_back(&block->nodes);
    ASSERT_EQ(VAR_NODE, var_node->base.node_type);
    ASSERT_EQ(TYPE_VALUE_NODE, var_node->init_value->node_type);
    auto value_node = (struct type_value_node *)var_node->init_value;
    auto value1 = *(struct literal_node **)array_front(&value_node->body->nodes);
    auto value2 = *(struct literal_node **)array_back(&value_node->body->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->base.node_type);
    ASSERT_EQ(LITERAL_NODE, value2->base.node_type);
    ASSERT_EQ(10.0, value1->double_val);
    ASSERT_EQ(20.0, value2->double_val);
    env_free(env);
}

TEST_F(testParser, testStructsTypeOneLineReturnTypeConstructor)
{
    char test_code[] = R"(
type Point2D = x:double y:double
get_point() = Point2D 10.0 20.0
)";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    ASSERT_EQ(2, array_size(&block->nodes));

    //1. first line is to define type
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(TYPE_NODE, node->node_type);
    type_node *type = (type_node *)node;
    ASSERT_STREQ("Point2D", string_get(type->name));
    ASSERT_EQ(2, array_size(&type->body->nodes));
    struct var_node *var1 = *(struct var_node **)array_front(&type->body->nodes);
    struct var_node *var2 = *(struct var_node **)array_back(&type->body->nodes);
    ASSERT_EQ(VAR_NODE, var1->base.node_type);
    ASSERT_EQ(VAR_NODE, var2->base.node_type);
    ASSERT_STREQ("x", string_get(var1->var_name));
    ASSERT_STREQ("y", string_get(var2->var_name));

    //2. second line is to define a one line function
    auto fun_node = *(struct function_node **)array_back(&block->nodes);
    ASSERT_EQ(FUNCTION_NODE, fun_node->base.node_type);
    ASSERT_EQ(1, array_size(&fun_node->body->nodes));
    auto type_value_node = *(struct type_value_node **)array_back(&fun_node->body->nodes);
    ASSERT_EQ(TYPE_VALUE_NODE, type_value_node->base.node_type);
    auto value1 = *(struct literal_node **)array_front(&type_value_node->body->nodes);
    auto value2 = *(struct literal_node **)array_back(&type_value_node->body->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->base.node_type);
    ASSERT_EQ(LITERAL_NODE, value2->base.node_type);
    ASSERT_EQ(10.0, value1->double_val);
    ASSERT_EQ(20.0, value2->double_val);
    env_free(env);
}

//
//TEST_F(testParser, testStructsTypeOneLineCallDirectAccessElement)
//{
//    char test_code[] = R"(
//type Point2D = x:double y:double
//get_point() = Point2D 10.0 20.0
//main() = get_point().x
//)";
//    auto env = env_new(false);
//    block_node *block = parse_string(env->parser, "test", test_code);
//    ASSERT_EQ(3, array_size(&block->nodes));
//
//    //1. first line is to define type
//    auto node = *(exp_node **)array_front(&block->nodes);
//    ASSERT_EQ(TYPE_NODE, node->node_type);
//    type_node *type = (type_node *)node;
//    ASSERT_STREQ("Point2D", string_get(type->name));
//    ASSERT_EQ(2, array_size(&type->body->nodes));
//    struct var_node *var1 = *(struct var_node **)array_front(&type->body->nodes);
//    struct var_node *var2 = *(struct var_node **)array_back(&type->body->nodes);
//    ASSERT_EQ(VAR_NODE, var1->base.node_type);
//    ASSERT_EQ(VAR_NODE, var2->base.node_type);
//    ASSERT_STREQ("x", string_get(var1->var_name));
//    ASSERT_STREQ("y", string_get(var2->var_name));
//
//    //2. second line is to define a one line function
//    auto fun_node = *(struct function_node **)array_back(&block->nodes);
//    ASSERT_EQ(FUNCTION_NODE, fun_node->base.node_type);
//    ASSERT_EQ(1, array_size(&fun_node->body->nodes));
//    auto type_value_node = *(struct type_value_node **)array_back(&fun_node->body->nodes);
//    ASSERT_EQ(TYPE_VALUE_NODE, type_value_node->base.node_type);
//    auto value1 = *(struct literal_node **)array_front(&type_value_node->body->nodes);
//    auto value2 = *(struct literal_node **)array_back(&type_value_node->body->nodes);
//    ASSERT_EQ(LITERAL_NODE, value1->base.node_type);
//    ASSERT_EQ(LITERAL_NODE, value2->base.node_type);
//    ASSERT_EQ(10.0, value1->double_val);
//    ASSERT_EQ(20.0, value2->double_val);
//    env_free(env);
//}

TEST_F(testParser, testStructsTypeDefAndDecl)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy:Point2D = 0.0 0.0
xy.x
)";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(3, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    node = *(exp_node **)array_get(&block->nodes, 1);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct var_node *var = (struct var_node *)node;
    ASSERT_STREQ("xy", string_get(var->var_name));
    ASSERT_STREQ("Point2D", string_get(var->base.annotated_type_name));
    ASSERT_EQ(TYPE_EXT, var->base.annotated_type_enum);
    node = *(exp_node **)array_get(&block->nodes, 2);
    ASSERT_EQ(IDENT_NODE, node->node_type);
    struct ident_node *id_node = (struct ident_node *)node;
    ASSERT_STREQ("xy.x", string_get(id_node->name));
    ASSERT_EQ(2, array_size(&id_node->member_accessors));
    ASSERT_STREQ("xy", string_get(*(symbol *)array_front(&id_node->member_accessors)));
    ASSERT_STREQ("x", string_get(*(symbol *)array_back(&id_node->member_accessors)));
    env_free(env);
}
