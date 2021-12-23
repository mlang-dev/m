/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "parser/m_grammar.h"
#include "parser/lalr_parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "lexer/init.h"
#include <stdio.h>

TEST(test_lalr_parser, var_decl)
{
    char test_code[] = "x:int";
    frontend_init();
    struct lalr_parser *parser = lalr_parser_new(get_m_grammar());
    struct ast_node *block = parse_text(parser, test_code);
    // struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    // ASSERT_EQ(1, array_size(&block->block->nodes));
    // ASSERT_STREQ("x", string_get(node->var->var_name));
    // ASSERT_EQ(VAR_NODE, node->node_type);
    // ASSERT_EQ(0, node->var->init_value);
    lalr_parser_free(parser);
    frontend_deinit();
}
/*
TEST_F(testParser, testBlockVariable)
{
    char test_code[] = "x = 11";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    ast_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    env_free(env);
}

TEST_F(testParser, testVariableWithType)
{
    char test_code[] = "x:int = 11";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    ast_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_INT, node->annotated_type_enum);
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    env_free(env);
}

TEST_F(testParser, testBool)
{
    char test_code[] = "x = true";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    ast_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(true, literal->liter->bool_val);
    env_free(env);
}

TEST_F(testParser, testChar)
{
    char test_code[] = "x = 'c'";
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    ast_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ('c', literal->liter->char_val);
    env_free(env);
}

TEST_F(testParser, testString)
{
    char test_code[] = "x = \"hello world!\"";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_STREQ("hello world!", literal->liter->str_val);
    env_free(env);
}

TEST_F(testParser, testBlockVariableNameWithUnderlyingLine)
{
    char test_code[] = "m_x = 11";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("m_x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    env_free(env);
}

TEST_F(testParser, testBlockIdFunction)
{
    char test_code[] = R"(
f x = x
f 10
)";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto body_node = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IDENT_NODE, body_node->node_type);
    env_free(env);
}

TEST_F(testParser, testBlockBinaryFunction)
{
    char test_code[] = "f x = x * x";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto body_node = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    env_free(env);
}

TEST_F(testParser, testBlockBinaryFunctionName)
{
    char test_code[] = "f_sq x = x * x";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto body_node = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f_sq", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    env_free(env);
}

TEST_F(testParser, testFacIfCondition)
{
    char test_code[] = R"(fac n = 
    if n< 2 n
    else n * fac (n-1))";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto body_node = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
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
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ast_node *body_node = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FOR_NODE, body_node->node_type);
    ASSERT_EQ(TYPE_INT, body_node->forloop->start->annotated_type_enum);
    ASSERT_EQ(TYPE_INT, body_node->forloop->step->annotated_type_enum);
    ASSERT_EQ(BINARY_NODE, body_node->forloop->end->node_type);
    ASSERT_EQ(3, ((ast_node *)body_node->forloop->start)->liter->int_val);
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
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto body = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("distance", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(VAR_NODE, body->node_type);
    env_free(env);
}

TEST_F(testParser, testAvgFunction)
{
    char test_code[] = R"(
avg x y = (x + y) / 2
    )";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto func = *(ast_node **)array_front(&block->block->nodes);
    auto body_node = *(ast_node **)array_front(&func->func->body->block->nodes);
    ASSERT_STREQ("avg", string_get(func->func->func_type->ft->name));
    ASSERT_STREQ("BINARY_NODE", node_type_strings[body_node->node_type]);
    env_free(env);
}

TEST_F(testParser, testVariadicFunction)
{
    char test_code[] = "f x ... = 10";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_STREQ("FUNCTION_NODE", node_type_strings[node->node_type]);
    ast_node *func = (ast_node *)node;
    ASSERT_EQ(true, func->func->func_type->ft->is_variadic);
    env_free(env);
}

TEST_F(testParser, testVariadicFunctionInvalidPosition)
{
    testing::internal::CaptureStderr();
    char test_code[] = "f ... x = 10";
    auto env = env_new(false);
    parse_string(env->parser, "", test_code);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:7: no parameter allowed after variadic\nerror: unknown token: TOKEN_ASSIGN\n", error.c_str());
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
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNCTION_NODE, node->node_type);
    ast_node *fun = (ast_node *)node;
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("to_string", string_get(fun->func->func_type->ft->name));
    env_free(env);
}

TEST_F(testParser, testPrototypeNode)
{
    char test_code[] = "extern printf(format:string ...):int";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("printf", string_get(node->ft->name));
    env_free(env);
}

TEST_F(testParser, testPrototypeNodeEmptyArg)
{
    char test_code[] = "extern print():int";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(0, array_size(&node->ft->fun_params));
    ASSERT_STREQ("print", string_get(node->ft->name));
    ASSERT_STREQ("int", string_get(node->annotated_type_name));
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
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->type_def->name));
    ASSERT_EQ(2, array_size(&node->type_def->body->block->nodes));
    struct ast_node *var1 = *(struct ast_node **)array_front(&node->type_def->body->block->nodes);
    struct ast_node *var2 = *(struct ast_node **)array_back(&node->type_def->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var_name));
    ASSERT_STREQ("y", string_get(var2->var->var_name));
    env_free(env);
}

TEST(test_lalr_parser, struct_type)
{
    char test_code[] = R"(
type Point2D = x:double y:double
point:Point2D
)";
    ;
    auto env = env_new(false);
    FILE *file = open_file_from_string(test_code);
    ast_node *block = parse_file_object(env->parser, "test", file);
    auto node = *(ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_STREQ("point", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(0, node->var->init_value);
    env_free(env);
}

TEST_F(testParser, testStructsTypeOneLine)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy:Point2D = 10.0 20.0
)";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->type_def->name));
    ASSERT_EQ(2, array_size(&node->type_def->body->block->nodes));
    struct ast_node *var1 = *(struct ast_node **)array_front(&node->type_def->body->block->nodes);
    struct ast_node *var2 = *(struct ast_node **)array_back(&node->type_def->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var_name));
    ASSERT_STREQ("y", string_get(var2->var->var_name));

    //struct type variable assignement
    auto var = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, var->node_type);
    ASSERT_EQ(TYPE_VALUE_NODE, var->var->init_value->node_type);
    auto value_node = var->var->init_value;
    auto value1 = *(struct ast_node **)array_front(&value_node->type_value->body->block->nodes);
    auto value2 = *(struct ast_node **)array_back(&value_node->type_value->body->block->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->node_type);
    ASSERT_EQ(LITERAL_NODE, value2->node_type);
    ASSERT_EQ(10.0, value1->liter->double_val);
    ASSERT_EQ(20.0, value2->liter->double_val);
    env_free(env);
}

TEST_F(testParser, testStructsTypeOneLineAnotherTypeConstructor)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy = Point2D 10.0 20.0
)";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->type_def->name));
    ASSERT_EQ(2, array_size(&node->type_def->body->block->nodes));
    struct ast_node *var1 = *(struct ast_node **)array_front(&node->type_def->body->block->nodes);
    struct ast_node *var2 = *(struct ast_node **)array_back(&node->type_def->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var_name));
    ASSERT_STREQ("y", string_get(var2->var->var_name));

    //struct type variable assignement
    auto var = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, var->node_type);
    ASSERT_EQ(TYPE_VALUE_NODE, var->var->init_value->node_type);
    auto value_node = var->var->init_value;
    auto value1 = *(struct ast_node **)array_front(&value_node->type_value->body->block->nodes);
    auto value2 = *(struct ast_node **)array_back(&value_node->type_value->body->block->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->node_type);
    ASSERT_EQ(LITERAL_NODE, value2->node_type);
    ASSERT_EQ(10.0, value1->liter->double_val);
    ASSERT_EQ(20.0, value2->liter->double_val);
    env_free(env);
}

TEST_F(testParser, testStructsTypeOneLineReturnTypeConstructor)
{
    char test_code[] = R"(
type Point2D = x:double y:double
get_point() = Point2D 10.0 20.0
)";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));

    //1. first line is to define type
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(TYPE_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->type_def->name));
    ASSERT_EQ(2, array_size(&node->type_def->body->block->nodes));
    struct ast_node *var1 = *(struct ast_node **)array_front(&node->type_def->body->block->nodes);
    struct ast_node *var2 = *(struct ast_node **)array_back(&node->type_def->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var_name));
    ASSERT_STREQ("y", string_get(var2->var->var_name));

    //2. second line is to define a one line function
    auto fun_node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(FUNCTION_NODE, fun_node->node_type);
    ASSERT_EQ(1, array_size(&fun_node->func->body->block->nodes));
    auto tv_node = *(struct ast_node **)array_back(&fun_node->func->body->block->nodes);
    ASSERT_EQ(TYPE_VALUE_NODE, tv_node->node_type);
    auto value1 = *(struct ast_node **)array_front(&tv_node->type_value->body->block->nodes);
    auto value2 = *(struct ast_node **)array_back(&tv_node->type_value->body->block->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->node_type);
    ASSERT_EQ(LITERAL_NODE, value2->node_type);
    ASSERT_EQ(10.0, value1->liter->double_val);
    ASSERT_EQ(20.0, value2->liter->double_val);
    env_free(env);
}

TEST_F(testParser, testStructsTypeDefAndDecl)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy:Point2D = 0.0 0.0
xy.x
)";
    auto env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;
    ASSERT_STREQ("xy", string_get(var->var->var_name));
    ASSERT_STREQ("Point2D", string_get(var->annotated_type_name));
    ASSERT_EQ(TYPE_EXT, var->annotated_type_enum);
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(IDENT_NODE, node->node_type);
    struct ast_node *id_node = node;
    ASSERT_STREQ("xy.x", string_get(id_node->ident->name));
    ASSERT_EQ(2, array_size(&id_node->ident->member_accessors));
    ASSERT_STREQ("xy", string_get(*(symbol *)array_front(&id_node->ident->member_accessors)));
    ASSERT_STREQ("x", string_get(*(symbol *)array_back(&id_node->ident->member_accessors)));
    env_free(env);
}
*/

int test_lalr_parser()
{
    UNITY_BEGIN();
    RUN_TEST(test_lalr_parser_var_decl);
    return UNITY_END();
}
