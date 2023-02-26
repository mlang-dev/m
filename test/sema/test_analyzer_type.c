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
#include "clib/string.h"
#include <stdio.h>

TEST(test_analyzer_type, tuple_type)
{
    char test_code[] = "\n\
type RGB = (u8, u8, u8)\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_EQ(TYPE_STRUCT, node->type->type);
    ASSERT_STREQ("RGB", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_type, tuple_variable)
{
    char test_code[] = "\n\
let x = (100, 200)\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_EQ(TYPE_TUPLE, node->type->type);
    ASSERT_STREQ("int * int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_type()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_type_tuple_type);
    RUN_TEST(test_analyzer_type_tuple_variable);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
