/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "analyzer.h"
#include "codegen.h"
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include "env.h"
#include <stdio.h>

TEST(testAnalyzer, testIntVariable)
{
    char test_code[] = "x = 11";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_INT, node->init_value->type->type);
    ASSERT_EQ(KIND_VAR, node->base.type->kind);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testDoubleVariable)
{
    char test_code[] = "x = 11.0";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_DOUBLE, node->init_value->type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testBoolVariable)
{
    char test_code[] = "x = true";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_BOOL, node->init_value->type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("bool", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testCharVariable)
{
    char test_code[] = "x = 'c'";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_CHAR, node->init_value->type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("char", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testStringVariable)
{
    char test_code[] = "x = \"hello world!\"";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_STRING, node->init_value->type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("string", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testCallNode)
{
    char test_code[] = "printf \"hello\"";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(call_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(CALL_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_INT, node->base.type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testDoubleIntLiteralError)
{
    char test_code[] = "x = 11.0 + 10";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(0, node->base.type);
    env_free(env);
}

TEST(testAnalyzer, testGreaterThan)
{
    char test_code[] = R"(
  11>10
  )";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    analyze_and_generate_code(env, (exp_node*)block);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testIdentityFunc)
{
    reset_id_name("a");
    char test_code[] = "id x = x";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("id", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("a -> a", string_get(&type_str));    
    env_free(env);
}

TEST(testAnalyzer, testIntIntFunc)
{
    char test_code[] = "f x = x + 10";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testDoubleDoubleFunc)
{
    char test_code[] = "f x = x + 10.0";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double -> double", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testBoolFunc)
{
    char test_code[] = "f x = !x";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("bool -> bool", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testMultiParamFunc)
{
    char test_code[] = "avg x y = (x + y) / 2.0";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("avg", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(3, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double * double -> double", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testRecursiveFunc)
{
    char test_code[] = R"(
factorial n = 
  if n < 2 then n
  else n * factorial (n-1)
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("factorial", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testForLoopFunc)
{
    char test_code[] = R"(
# using for loop
loopprint n = 
  for i in 0..n
    printf "%d" i
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> ()", string_get(&type_str));
    for_node* forn = *(for_node**)array_front(&node->body->nodes);
    ASSERT_EQ(TYPE_INT, get_type(forn->step->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->start->type));
    ASSERT_EQ(TYPE_BOOL, get_type(forn->end->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->body->type));
    env_free(env);
}

TEST(testAnalyzer, testLocalVariableFunc)
{
    char test_code[] = R"(
# using for loop
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
)";
    reset_id_name("a");
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("distance", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(5, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double * double * double * double -> double", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testLocalStringFunc)
{
    char test_code[] = R"(
to_string () = 
  x = "hello"
  y = x
  y
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("to_string", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(1, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("() -> string", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testVariadicFunc)
{
    char test_code[] = R"(
var_func ... = 0
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("var_func", string_get(node->prototype->name));
    ASSERT_EQ(true, node->prototype->is_variadic);
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("... -> int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testPrintfFunc)
{
    char test_code[] = R"(
printf "%d" 100
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(CALL_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testStructLikeType)
{
    char test_code[] = R"(
type Point2D = x:double y:double
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testFunctionTypeAnnotation)
{
    char test_code[] = R"(
    print x:int = printf "%d" x
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("print", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testFunctionTypeAnnotationWithParentheses)
{
    char test_code[] = R"(
    prt (x:int) = printf "%d" x
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("prt", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testFunctionTypeAnnotationWithReturnType)
{
    char test_code[] = R"(
    prt (x:int):int = printf "%d" x
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("prt", string_get(node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testStructTypeVariables)
{
    char test_code[] = R"(
type Point2D = x:double y:double
xy:Point2D = 0.0 0.0
xy.x
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    ASSERT_EQ(3, array_size(&block->nodes));
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(exp_node**)array_front(&block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(exp_node**)array_get(&block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(exp_node**)array_get(&block->nodes, 2);
    ASSERT_EQ(IDENT_NODE, node->node_type);
    struct ident_node* id_node = (struct ident_node*)node;
    ASSERT_STREQ("xy.x", string_get(id_node->name));
    type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testStructTypeLocalVariables)
{
    char test_code[] = R"(
type Point2D = x:double y:double
getx()=
    xy:Point2D = 10.0 0.0
    xy.x
getx()
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    ASSERT_EQ(3, array_size(&block->nodes));
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(exp_node**)array_front(&block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(exp_node**)array_get(&block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> double", string_get(&type_str));
    node = *(exp_node**)array_get(&block->nodes, 2);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    env_free(env);
}

TEST(testAnalyzer, testStructTypeReturn)
{
    char test_code[] = R"(
type Point2D = x:double y:double
getx()=
    xy:Point2D = 10.0 0.0
    xy
z = getx()
)";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    ASSERT_EQ(3, array_size(&block->nodes));
    analyze_and_generate_code(env, (exp_node*)block);
    auto node = *(exp_node**)array_front(&block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(exp_node**)array_get(&block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    /*variable node*/
    node = *(exp_node**)array_get(&block->nodes, 2);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct var_node* var = (struct var_node*)node;

    /*initial value is a call expression*/
    ASSERT_EQ(CALL_NODE, var->init_value->node_type);
    type_str = to_string(var->init_value->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    type_str = to_string(var->base.type);
    ASSERT_STREQ("Point2D", string_get(&type_str));

    /*verify variable xy in inner function is out of scope*/
    symbol xy = to_symbol("xy");
    ASSERT_EQ(false, has_symbol(&env->venv, xy));
    env_free(env);
}
