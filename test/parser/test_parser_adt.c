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

TEST(test_parser_adt, tuple_type)
{
    char test_code[] = "\n\
type Point2D = int * int\n\
var point:Point2D";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *type_node = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(TYPE_NODE, type_node->node_type);
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("point", string_get(node->var->var->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

int test_parser_adt()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_adt_tuple_type);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
