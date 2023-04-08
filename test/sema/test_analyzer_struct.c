/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "parser/parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "sema/frontend.h"
#include "sema/analyzer.h"
#include "app/error.h"
#include "app/app.h"
#include "clib/string.h"
#include <stdio.h>

TEST(test_analyzer_struct, def_struct_type)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    string type_str = to_string(tc, node->type);
    ASSERT_EQ(TYPE_STRUCT, node->type->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_struct, var_init)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let xy:Point2D = { 0.0, 0.0 }\n\
xy.x\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_struct, ret_struct_type_direct)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let get_point() = Point2D { 10.0, 0.0 }\n\
let z() = get_point()\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    //1. type definition
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    //2. function definition
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    //3. function definition again
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_struct, ret_struct_type)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let getx()=\n\
    let xy:Point2D = Point2D { 10.0, 0.0 }\n\
    xy\n\
let z = getx()\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*func definition*/
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    /*variable node*/
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;

    /*initial value is a call expression*/
    ASSERT_EQ(CALL_NODE, var->var->init_value->node_type);
    type_str = to_string(tc, var->var->init_value->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    type_str = to_string(tc, var->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));

    /*verify variable xy in inner function is out of scope*/
    symbol xy = to_symbol("xy");
    ASSERT_EQ(false, has_symbol(&fe->sema_context->varname_2_typexprs, xy));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_struct, type_vars)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let xy:Point2D = Point2D { 0.0, 0.0 }\n\
xy.x\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_struct, type_local_var)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let getx()=\n\
    let xy:Point2D = Point2D { 10.0, 0.0 }\n\
    xy.x\n\
getx()\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*func definition*/
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("() -> f64", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}
TEST(test_analyzer_struct, pass_by_ref)
{
    char test_code[] = "\n\
struct Point = x:mut f64, y:f64\n\
let update xy:&Point =\n\
    xy.x = 10.0\n\
let z = Point{100.0, 200.0}\n\
update (&z)\n\
z.x\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(5, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point", string_get(&type_str));
    /*func definition*/
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("&Point -> ()", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(VAR_NODE, node->node_type);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("Point", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_struct()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_struct_def_struct_type);
    RUN_TEST(test_analyzer_struct_var_init);
    RUN_TEST(test_analyzer_struct_ret_struct_type_direct);
    RUN_TEST(test_analyzer_struct_ret_struct_type);
    RUN_TEST(test_analyzer_struct_type_local_var);
    RUN_TEST(test_analyzer_struct_type_vars);
    RUN_TEST(test_analyzer_struct_pass_by_ref);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
