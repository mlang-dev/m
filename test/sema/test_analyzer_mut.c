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

TEST(test_analyzer, type_immutable)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
let x = 10\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_get(&block->block->nodes, 0);
    ASSERT_FALSE(x->is_addressed);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_EQ(0, x->type->val_type);
    ASSERT_EQ(Immutable, x->type->mut);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, mutable_type)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
var x = 10\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_get(&block->block->nodes, 0);
    ASSERT_FALSE(x->is_addressed);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_EQ(0, x->type->val_type);
    ASSERT_EQ(Mutable, x->var->mut);
    ASSERT_EQ(Mutable, x->type->mut);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, immutable_record_member_type)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
record Point = x:int, y:int\n\
let xy = Point { 10, 20 }\n\
xy.x\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_FALSE(x->is_addressed);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_EQ(0, x->type->val_type);
    ASSERT_EQ(Immutable, x->index->index->ident->var->var->mut);
    ASSERT_EQ(Immutable, x->type->mut);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, mutable_record_member_type)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
record Point = x:mut int, y:int\n\
let xy = Point { 10, 20 }\n\
xy.x\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_FALSE(x->is_addressed);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_EQ(0, x->type->val_type);
    //FIXME: ASSERT_EQ(Mutable, x->index->index->ident->var->var->mut);
    ASSERT_EQ(Mutable, x->type->mut);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, mutable_array_member_type)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
var a = [10]\n\
a[0]\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_FALSE(x->is_addressed);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_EQ(0, x->type->val_type);
    ASSERT_EQ(Mutable, x->type->mut);
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_mut()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_type_immutable);
    RUN_TEST(test_analyzer_mutable_type);
    RUN_TEST(test_analyzer_immutable_record_member_type);
    RUN_TEST(test_analyzer_mutable_record_member_type);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
