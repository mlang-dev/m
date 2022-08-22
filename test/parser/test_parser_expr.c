/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lr parser for alexpression
 */
#include "parser/grammar.h"
#include "parser/parser.h"
#include "parser/astdump.h"
#include "codegen/wat_codegen.h"
#include "lexer/init.h"
#include "test.h"
#include <stdio.h>

TEST(test_parser_expr, arithmetic_simple_add)
{
    const char test_code[] = "1+2";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1+2)", string_get(&dump_str));
    ast_node_free(ast);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, arithmetic_simple_mult)
{
    const char test_code[] = "1*2";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1*2)", string_get(&dump_str));
    ast_node_free(ast);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, arithmetic_exp_neg)
{
    const char test_code[] = "-1+2";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(-1+2)", string_get(&dump_str));
    ast_node_free(ast);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, arithmetic_exp)
{
    const char test_code[] = "1+2*3";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1+(2*3))", string_get(&dump_str));
    ast_node_free(ast);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, arithmetic_exp2)
{
    const char test_code[] = "1+2*3**4";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1+(2*(3**4)))", string_get(&dump_str));
    ast_node_free(ast);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, arithmetic_exp_parentheses)
{
    const char test_code[] = "(1+2)*3";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("((1+2)*3)", string_get(&dump_str));

    ast_node_free(ast);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, logical_or)
{
    char test_code[] = "true || false";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("||", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, logical_and)
{
    char test_code[] = "true && false";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("&&", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, logical_not)
{
    char test_code[] = "! true";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNARY_NODE, node->node_type);
    ASSERT_STREQ("!", get_opcode(node->unop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, lt)
{
    char test_code[] = "10 < 11";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("<", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, gt)
{
    char test_code[] = "11 > 10";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_EQ(OP_GT, node->binop->opcode);
    ASSERT_STREQ(">", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, eq)
{
    char test_code[] = "11==10";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("==", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, neq)
{
    char test_code[] = "11!=10";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("!=", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, ge)
{
    char test_code[] = "11>=10";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ(">=", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

TEST(test_parser_expr, le)
{
    char test_code[] = "11<=10";
    frontend_init();
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    ASSERT_STREQ("<=", get_opcode(node->binop->opcode));
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

int test_lr_parser_expr()
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
    return UNITY_END();
}
