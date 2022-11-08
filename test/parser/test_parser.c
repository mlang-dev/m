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
#include <stdio.h>

TEST(test_parser, int_type)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "x:int";
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(0, node->var->init_value);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, var_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "x = 11";
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, var_init_name_with_underlying)
{
    char test_code[] = "m_x = 11";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("m_x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, var_struct_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "x:int = 11";
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("int", string_get(node->annotated_type_name));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, bool_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "x = true";
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(true, literal->liter->int_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, char_init)
{
    char test_code[] = "x = 'c'";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ('c', literal->liter->int_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, string_init)
{
    char test_code[] = "x = \"hello world!\"";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_STREQ("hello world!", literal->liter->str_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, id_func)
{
    char test_code[] = "\n\
let f x = x\n\
f 10 ";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *func = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *call = *(struct ast_node **)array_back(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&func->func->body->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(func->func->func_type->ft->name));
    ASSERT_EQ(IDENT_NODE, body_node->node_type);
    ASSERT_EQ(CALL_NODE, call->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, binary_exp_func)
{
    char test_code[] = "let f x = x * x";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, func_with_new_line)
{
    char test_code[] = "\n\
let f x = \n\
    x * x";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, distance_function)
{
    char test_code[] = "\n\
let distance x1 y1 x2 y2 = \n\
  xx = (x1-x2) * (x1-x2) \n\
  yy = (y1-y2) * (y1-y2) \n\
  sqrt (xx + yy)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_first = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    ASSERT_STREQ("distance", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(VAR_NODE, body_first->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, for_loop)
{
    char test_code[] = "\n\
let loopprint n = \n\
  for i in 3..n \n\
    print i";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FOR_NODE, body_node->node_type);
    ASSERT_EQ(0, body_node->forloop->range->range->step);
    ASSERT_EQ(BINARY_NODE, body_node->forloop->range->range->end->node_type);
    ASSERT_EQ(3, ((struct ast_node *)body_node->forloop->range->range->start)->liter->int_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, for_loop_with_step)
{
    char test_code[] = "\n\
let loopprint n = \n\
  for i in 3..2..n \n\
    print i";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FOR_NODE, body_node->node_type);
    ASSERT_EQ(2, body_node->forloop->range->range->step->liter->int_val);
    ASSERT_EQ(BINARY_NODE, body_node->forloop->range->range->end->node_type);
    ASSERT_EQ(3, ((struct ast_node *)body_node->forloop->range->range->start)->liter->int_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, avg_function)
{
    char test_code[] = "let avg x y = (x + y) / 2";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *func = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&func->func->body->block->nodes);
    ASSERT_STREQ("avg", string_get(func->func->func_type->ft->name));
    ASSERT_STREQ("BINARY_NODE", node_type_strings[body_node->node_type]);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, if_condition)
{
    char test_code[] = "\n\
let fac n = \n\
    if n< 2 then n \n\
    else n * fac (n-1)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_one_line)
{
    char test_code[] = "\n\
let fac n = \n\
    if n< 2 then n else n * fac (n-1)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_one_block)
{
    char test_code[] = "\n\
let fac n = \n\
    if n< 2 then \n\
        n \n\
    else n * fac (n-1)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_two_blocks)
{
    char test_code[] = "\n\
let fac n = \n\
    if n< 2 then \n\
        n \n\
    else \n\
        n * fac (n-1)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_no_else)
{
    char test_code[] = "\n\
let fac n = \n\
    if n< 2 then n \n\
    n * fac (n-1)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, local_string_function)
{
    char test_code[] = "\n\
let to_string () = \n\
  x = \"hello\" \n\
  y = x \n\
  y";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("to_string", string_get(node->func->func_type->ft->name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, variadic_function)
{
    char test_code[] = "let f x ... = 10";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    ASSERT_EQ(true, node->func->func_type->ft->is_variadic);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, func_type)
{
    char test_code[] = "fun printf format:string ... -> ()";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("printf", string_get(node->ft->name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, func_type_no_param)
{
    char test_code[] = "fun print () -> int";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(0, array_size(&node->ft->params->block->nodes));
    ASSERT_STREQ("print", string_get(node->ft->name));
    ASSERT_STREQ("int", string_get(node->annotated_type_name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, func_type_no_param_no_return)
{
    char test_code[] = "fun print ()->()";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(0, array_size(&node->ft->params->block->nodes));
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_STREQ("print", string_get(node->ft->name));
    ASSERT_STREQ("()", string_get(node->annotated_type_name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, type_decl)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
point:Point2D";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(0, node->var->init_value);
    ASSERT_STREQ("point", string_get(node->var->var_name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, type_decl2)
{
    char test_code[] = "\n\
struct Point2D = \n\
  x:f64 \n\
  y:f64";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->struct_def->name));
    ASSERT_EQ(2, array_size(&node->struct_def->body->block->nodes));
    struct ast_node *var1 = *(struct ast_node **)array_front(&node->struct_def->body->block->nodes);
    struct ast_node *var2 = *(struct ast_node **)array_back(&node->struct_def->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var_name));
    ASSERT_STREQ("y", string_get(var2->var->var_name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, type_var_init)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
xy = Point2D(10.0, 20.0)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->struct_def->name));
    ASSERT_EQ(2, array_size(&node->struct_def->body->block->nodes));
    struct ast_node *var1 = *(struct ast_node **)array_front(&node->struct_def->body->block->nodes);
    struct ast_node *var2 = *(struct ast_node **)array_back(&node->struct_def->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var_name));
    ASSERT_STREQ("y", string_get(var2->var->var_name));

    // struct type variable assignement
    struct ast_node *var = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, var->node_type);
    ASSERT_EQ(STRUCT_INIT_NODE, var->var->init_value->node_type);
    struct ast_node *value_node = var->var->init_value;
    struct ast_node *value1 = *(struct ast_node **)array_front(&value_node->struct_init->body->block->nodes);
    struct ast_node *value2 = *(struct ast_node **)array_back(&value_node->struct_init->body->block->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->node_type);
    ASSERT_EQ(LITERAL_NODE, value2->node_type);
    ASSERT_EQ(10.0, value1->liter->double_val);
    ASSERT_EQ(20.0, value2->liter->double_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, func_returns_struct_init)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
let get_point() = Point2D(10.0, 20.0)";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));

    // 1. first line is to define type
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->struct_def->name));
    ASSERT_EQ(2, array_size(&node->struct_def->body->block->nodes));
    struct ast_node *var1 = *(struct ast_node **)array_front(&node->struct_def->body->block->nodes);
    struct ast_node *var2 = *(struct ast_node **)array_back(&node->struct_def->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var_name));
    ASSERT_STREQ("y", string_get(var2->var->var_name));

    // 2. second line is to define a one line function
    struct ast_node *fun_node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(FUNC_NODE, fun_node->node_type);
    ASSERT_EQ(1, array_size(&fun_node->func->body->block->nodes));
    struct ast_node *tv_node = *(struct ast_node **)array_back(&fun_node->func->body->block->nodes);
    ASSERT_EQ(STRUCT_INIT_NODE, tv_node->node_type);
    struct ast_node *value1 = *(struct ast_node **)array_front(&tv_node->struct_init->body->block->nodes);
    struct ast_node *value2 = *(struct ast_node **)array_back(&tv_node->struct_init->body->block->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->node_type);
    ASSERT_EQ(LITERAL_NODE, value2->node_type);
    ASSERT_EQ(10.0, value1->liter->double_val);
    ASSERT_EQ(20.0, value2->liter->double_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, use_type_field)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
xy:Point2D = Point2D(0.0, 0.0) \n\
xy.x";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;
    ASSERT_STREQ("xy", string_get(var->var->var_name));
    ASSERT_STREQ("Point2D", string_get(var->annotated_type_name));
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    ASSERT_EQ(IDENT_NODE, node->index->object->node_type);
    ASSERT_EQ(IDENT_NODE, node->index->index->node_type);
    ASSERT_STREQ("xy", string_get(node->index->object->ident->name));
    ASSERT_STREQ("x", string_get(node->index->index->ident->name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, member_field_assignment)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
xy:Point2D = Point2D(0.0, 0.0) \n\
xy.x = 10.0";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;
    ASSERT_STREQ("xy", string_get(var->var->var_name));
    ASSERT_STREQ("Point2D", string_get(var->annotated_type_name));
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->binop->lhs->node_type);
    ASSERT_EQ(LITERAL_NODE, node->binop->rhs->node_type);
    ASSERT_EQ(OP_ASSIGN, node->binop->opcode);
    node = node->binop->lhs;
    ASSERT_STREQ("xy", string_get(node->index->object->ident->name));
    ASSERT_STREQ("x", string_get(node->index->index->ident->name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, import_fun_type)
{
    char test_code[] = "from sys import fun print () -> ()";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    ASSERT_STREQ("sys", string_get(node->import->from_module));
    node = node->import->import;
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(0, array_size(&node->ft->params->block->nodes));
    ASSERT_STREQ("print", string_get(node->ft->name));
    ASSERT_STREQ("()", string_get(node->annotated_type_name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, import_memory_init)
{
    char test_code[] = "from sys import memory 2";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    node = node->import->import;
    ASSERT_EQ(MEMORY_NODE, node->node_type);
    ASSERT_EQ(2, node->memory->initial->liter->int_val);
    ASSERT_EQ(0, node->memory->max);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, import_memory_init_max)
{
    char test_code[] = "from sys import memory 2, 10";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    node = node->import->import;
    ASSERT_EQ(MEMORY_NODE, node->node_type);
    ASSERT_EQ(2, node->memory->initial->liter->int_val);
    ASSERT_EQ(10, node->memory->max->liter->int_val);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, import_global)
{
    char test_code[] = "from sys import __stack_pointer:int";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    node = node->import->import;
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("__stack_pointer", string_get(node->var->var_name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, ref_type)
{
    char test_code[] = "ri:&int";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_TRUE(node->is_ref_annotated);
    ASSERT_STREQ("int", string_get(node->annotated_type_name));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, create_ref_variable)
{
    char test_code[] = "ri = &i";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(UNARY_NODE, node->var->init_value->node_type);
    ASSERT_EQ(OP_BAND, node->var->init_value->unop->opcode);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser, create_deref_variable)
{
    char test_code[] = "ri = *i";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(UNARY_NODE, node->var->init_value->node_type);
    ASSERT_EQ(OP_STAR, node->var->init_value->unop->opcode);
    ast_node_free(block);
    parser_free(parser); 
    frontend_deinit(fe);
}


TEST(test_parser, array_variable)
{
    char test_code[] = "a = [10,20]";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LIST_COMP_NODE, node->var->init_value->node_type);
    ast_node_free(block);
    parser_free(parser); 
    frontend_deinit(fe);
}

int test_parser()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_int_type);
    RUN_TEST(test_parser_var_init);
    RUN_TEST(test_parser_var_init_name_with_underlying);
    RUN_TEST(test_parser_var_struct_init);
    RUN_TEST(test_parser_bool_init);
    RUN_TEST(test_parser_char_init);
    RUN_TEST(test_parser_string_init);
    RUN_TEST(test_parser_id_func);
    RUN_TEST(test_parser_binary_exp_func);
    RUN_TEST(test_parser_func_with_new_line);
    RUN_TEST(test_parser_distance_function);
    RUN_TEST(test_parser_for_loop);
    RUN_TEST(test_parser_for_loop_with_step);
    RUN_TEST(test_parser_avg_function);
    RUN_TEST(test_parser_if_condition);
    RUN_TEST(test_parser_if_condition_one_line);
    RUN_TEST(test_parser_if_condition_one_block);
    RUN_TEST(test_parser_if_condition_two_blocks);
    RUN_TEST(test_parser_if_condition_no_else);
    RUN_TEST(test_parser_local_string_function);
    RUN_TEST(test_parser_variadic_function);
    RUN_TEST(test_parser_func_type);
    RUN_TEST(test_parser_func_type_no_param);
    RUN_TEST(test_parser_func_type_no_param_no_return);
    RUN_TEST(test_parser_type_decl);
    RUN_TEST(test_parser_type_decl2);
    RUN_TEST(test_parser_type_var_init);
    RUN_TEST(test_parser_func_returns_struct_init);
    RUN_TEST(test_parser_use_type_field);
    RUN_TEST(test_parser_member_field_assignment);
    RUN_TEST(test_parser_import_fun_type);
    RUN_TEST(test_parser_import_memory_init);
    RUN_TEST(test_parser_import_memory_init_max);
    RUN_TEST(test_parser_import_global);
    RUN_TEST(test_parser_ref_type);
    RUN_TEST(test_parser_create_ref_variable);
    RUN_TEST(test_parser_create_deref_variable);
    RUN_TEST(test_parser_array_variable);
    return UNITY_END();
}
