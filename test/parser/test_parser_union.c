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

TEST(test_parser_union, one_line_definition)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "union XY = x:int | y:float";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNION_NODE, node->node_type);
    node_free(block);
    frontend_deinit(fe);
}

int test_parser_union()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_union_one_line_definition);
    return UNITY_END();
}
