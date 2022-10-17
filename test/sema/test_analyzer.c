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
#include <stdio.h>

TEST(test_analyzer, ref_type_variable)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
x = 10\n\
y = &x";
    struct parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_get(&block->block->nodes, 0);
    struct ast_node* y = *(struct ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_EQ(0, x->type->ref_type);
    ASSERT_EQ(TYPE_REF, y->type->type);
    ASSERT_TRUE(y->type->ref_type);
    ast_node_free(block);
    parser_free(parser);
    frontend_deinit(fe);
}

int test_analyzer()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_ref_type_variable);
    return UNITY_END();
}
