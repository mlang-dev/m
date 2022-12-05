/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for pattern matching analysis
 */
#include "parser/parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "sema/frontend.h"
#include "sema/analyzer.h"
#include "clib/string.h"
#include <stdio.h>


TEST(test_analyzer_pm, pattern_match)
{
    char test_code[] = "\n\
let pm x = \n\
    match x with\n\
    | 0 -> 100\n\
    | 1 -> 200\n\
    | _ -> 300\n\
pm 0\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* fun = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(TYPE_FUNCTION, fun->type->type);
    ASSERT_EQ(to_symbol("int -> int"), fun->type->name);
    struct ast_node* call = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(to_symbol("int"), call->type->name);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_pm, pattern_match_variable)
{
    char test_code[] = "\n\
let pm x = \n\
    match x with\n\
    | 0 -> 100\n\
    | 1 -> 200\n\
    | y -> y + 3\n\
pm 0\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* fun = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(TYPE_FUNCTION, fun->type->type);
    ASSERT_EQ(to_symbol("int -> int"), fun->type->name);
    struct ast_node* call = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(to_symbol("int"), call->type->name);
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_pm()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_pm_pattern_match);
    RUN_TEST(test_analyzer_pm_pattern_match_variable);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
