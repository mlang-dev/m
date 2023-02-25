/*
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parsing record type
 */
#include "parser/parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "sema/frontend.h"
#include <stdio.h>

TEST(test_parser_type, tuple_type)
{
    char test_code[] = "\n\
type Point2D = (int, int)\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *type_node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_NODE, type_node->node_type);
    ASSERT_STREQ("Point2D", string_get(type_node->type_node->type_name));
    ASSERT_EQ(TYPE_EXPR_ITEM_NODE, type_node->type_node->type_body->node_type);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser_type, tuple_type_variable)
{
    char test_code[] = "\n\
let x  = (10, 20)\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    node_free(block);
    frontend_deinit(fe);
}

int test_parser_type()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_type_tuple_type);
    RUN_TEST(test_parser_type_tuple_type_variable);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
