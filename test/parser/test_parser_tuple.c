/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parsing record type
 */
#include "parser/parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "sema/frontend.h"
#include <stdio.h>

TEST(test_parser_tuple, var_decl)
{
    char test_code[] = "\n\
let xy = (\"one\", 2)\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(0, node->var->init_value);
    ASSERT_STREQ("point", string_get(node->var->var->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

int test_parser_tuple()
{
    UNITY_BEGIN();
    //RUN_TEST(test_parser_tuple_type_decl);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
