/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lr parser for alexpression
 */
#include "parser/parser.h"
#include "parser/astdump.h"
#include "sema/frontend.h"
#include "sema/analyzer.h"
#include "test.h"
#include <stdio.h>

TEST(test_parser_expr, arithmetic_simple_add)
{
    const char test_code[] = "1+2";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    analyze(fe->sema_context, ast);
    string dump_str = dump(fe->sema_context, ast);
    ASSERT_STREQ("(1+2)", string_get(&dump_str));
    node_free(ast);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, arithmetic_simple_mult)
{
    const char test_code[] = "1*2";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    analyze(fe->sema_context, ast);
    string dump_str = dump(fe->sema_context, ast);
    ASSERT_STREQ("(1*2)", string_get(&dump_str));
    node_free(ast);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, arithmetic_exp_neg)
{
    const char test_code[] = "-1+2";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    analyze(fe->sema_context, ast);
    string dump_str = dump(fe->sema_context, ast);
    ASSERT_STREQ("(-1+2)", string_get(&dump_str));
    node_free(ast);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, arithmetic_exp)
{
    const char test_code[] = "1+2*3";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    analyze(fe->sema_context, ast);
    string dump_str = dump(fe->sema_context, ast);
    ASSERT_STREQ("(1+(2*3))", string_get(&dump_str));
    node_free(ast);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, arithmetic_exp2)
{
    const char test_code[] = "1+2*3**4";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    analyze(fe->sema_context, ast);
    string dump_str = dump(fe->sema_context, ast);
    ASSERT_STREQ("(1+(2*(3**4)))", string_get(&dump_str));
    node_free(ast);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, arithmetic_exp_parentheses)
{
    const char test_code[] = "(1+2)*3";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    analyze(fe->sema_context, ast);
    string dump_str = dump(fe->sema_context, ast);
    ASSERT_STREQ("((1+2)*3)", string_get(&dump_str));

    node_free(ast);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, logical_or)
{
    char test_code[] = "true or false";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("or", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, logical_and)
{
    char test_code[] = "true and false";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("and", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, logical_not)
{
    char test_code[] = "not true";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(UNARY_NODE, node->node_type);
    ASSERT_STREQ("not", get_opcode(node->unop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, lt)
{
    char test_code[] = "10 < 11";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("<", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, gt)
{
    char test_code[] = "11 > 10";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_EQ(OP_GT, node->binop->opcode);
    ASSERT_STREQ(">", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, eq)
{
    char test_code[] = "11==10";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("==", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, neq)
{
    char test_code[] = "11!=10";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("!=", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, ge)
{
    char test_code[] = "11>=10";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ(">=", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_parser_expr, le)
{
    char test_code[] = "11<=10";
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("<=", get_opcode(node->binop->opcode));
    node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

int test_parser_expr(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_expr_arithmetic_simple_add);
    RUN_TEST(test_parser_expr_arithmetic_simple_mult);
    RUN_TEST(test_parser_expr_arithmetic_exp_neg);
    RUN_TEST(test_parser_expr_arithmetic_exp);
    RUN_TEST(test_parser_expr_arithmetic_exp2);
    RUN_TEST(test_parser_expr_arithmetic_exp_parentheses);
    RUN_TEST(test_parser_expr_logical_or);
    RUN_TEST(test_parser_expr_logical_and);
    RUN_TEST(test_parser_expr_logical_not);
    RUN_TEST(test_parser_expr_lt);
    RUN_TEST(test_parser_expr_le);
    RUN_TEST(test_parser_expr_gt);
    RUN_TEST(test_parser_expr_ge);
    RUN_TEST(test_parser_expr_eq);
    RUN_TEST(test_parser_expr_neq);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
