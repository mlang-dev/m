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
#include "error/error.h"
#include <stdio.h>

TEST(test_parser_error, char_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "let f x";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(0, block);
    node_free(block);
    frontend_deinit(fe);
}

int test_parser_error()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_error_char_literal);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
