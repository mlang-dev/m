/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lr parser for alexpression
 */
#include "parser/grammar.h"
#include "parser/lalr_parser.h"
#include "parser/astdump.h"
#include "codegen/wat_codegen.h"
#include "lexer/init.h"
#include "test.h"
#include <stdio.h>

TEST(test_lr_parser_expr, arithmetic_simple_add)
{
    const char test_code[] = "1+2";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1+2)", string_get(&dump_str));
    ast_node_free(ast);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lr_parser_expr, arithmetic_simple_mult)
{
    const char test_code[] = "1*2";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1*2)", string_get(&dump_str));
    ast_node_free(ast);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lr_parser_expr, arithmetic_exp_neg)
{
    const char test_code[] = "-1+2";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(-1+2)", string_get(&dump_str));
    ast_node_free(ast);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lr_parser_expr, arithmetic_exp)
{
    const char test_code[] = "1+2*3";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1+(2*3))", string_get(&dump_str));
    ast_node_free(ast);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lr_parser_expr, arithmetic_exp2)
{
    const char test_code[] = "1+2*3^4";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("(1+(2*(3^4)))", string_get(&dump_str));
    ast_node_free(ast);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lr_parser_expr, arithmetic_exp_parentheses)
{
    const char test_code[] = "(1+2)*3";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string dump_str = dump(ast);
    ASSERT_STREQ("((1+2)*3)", string_get(&dump_str));

    ast_node_free(ast);
    lalr_parser_free(parser);
    frontend_deinit();
}

int test_lr_parser_expr()
{
    UNITY_BEGIN();
    RUN_TEST(test_lr_parser_expr_arithmetic_simple_add);
    RUN_TEST(test_lr_parser_expr_arithmetic_simple_mult);
    RUN_TEST(test_lr_parser_expr_arithmetic_exp_neg);
    RUN_TEST(test_lr_parser_expr_arithmetic_exp);
    RUN_TEST(test_lr_parser_expr_arithmetic_exp2);
    RUN_TEST(test_lr_parser_expr_arithmetic_exp_parentheses);
    return UNITY_END();
}
