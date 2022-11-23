/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/ast.h"
#include "test.h"
#include <stdio.h>


TEST(test_ast, node_type_names)
{
    struct node_type_name *ntns = get_node_type_names();
    for(size_t i = 0; i < TOTAL_NODE + 1; i++){
        ASSERT_EQ(i, ntns[i].node_type);
    }
}

int test_ast()
{
    UNITY_BEGIN();
    RUN_TEST(test_ast_node_type_names);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
