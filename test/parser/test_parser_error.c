/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "parser/parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "lexer/init.h"
#include <stdio.h>

TEST(test_parser_error, char_literal)
{
    frontend_init();
    char test_code[] = "x = '";
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    ASSERT_EQ(0, block);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit();
}

int test_parser_error()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_error_char_literal);
    return UNITY_END();
}
