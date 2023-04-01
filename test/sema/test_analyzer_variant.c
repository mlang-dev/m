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

TEST(test_analyzer_variant, member_type_float)
{
    char test_code[] = "\n\
variant XorY = x:f64 | y:int\n\
let xy = XorY{0.0}\n\
xy.x\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_variant, member_type_int)
{
    char test_code[] = "\n\
variant XorY = x:f64 | y:int\n\
let xy:XorY = {0.0}\n\
xy.y\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 1);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(tc, node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_variant, enum_type_int)
{
    char test_code[] = "\n\
variant Color = Red | Green | Blue\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *union_node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, union_node->type);
    ASSERT_STREQ("Color", string_get(&type_str));
    ASSERT_EQ(VARIANT_NODE, union_node->node_type);
    ASSERT_EQ(3, array_size(&union_node->adt_type->body->block->nodes));
    struct ast_node *node = array_front_ptr(&union_node->adt_type->body->block->nodes);
    ASSERT_EQ(to_symbol("Red"), node->variant_type_node->tag);
    ASSERT_EQ(0, node->variant_type_node->tag_repr);
    node = array_get_ptr(&union_node->adt_type->body->block->nodes, 1);
    ASSERT_EQ(to_symbol("Green"), node->variant_type_node->tag);
    ASSERT_EQ(1, node->variant_type_node->tag_repr);
    node = array_get_ptr(&union_node->adt_type->body->block->nodes, 2);
    ASSERT_EQ(to_symbol("Blue"), node->variant_type_node->tag);
    ASSERT_EQ(2, node->variant_type_node->tag_repr);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_variant, enum_type_int_manual_set)
{
    char test_code[] = "\n\
variant Color = Red | Green = 10 | Blue\n\
";
    struct frontend *fe = frontend_init();
    struct type_context *tc = fe->sema_context->tc;
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *union_node = array_front_ptr(&block->block->nodes);
    string type_str = to_string(tc, union_node->type);
    ASSERT_STREQ("Color", string_get(&type_str));
    ASSERT_EQ(VARIANT_NODE, union_node->node_type);
    ASSERT_EQ(3, array_size(&union_node->adt_type->body->block->nodes));
    struct ast_node *node = array_front_ptr(&union_node->adt_type->body->block->nodes);
    ASSERT_EQ(to_symbol("Red"), node->variant_type_node->tag);
    ASSERT_EQ(0, node->variant_type_node->tag_repr);
    node = array_get_ptr(&union_node->adt_type->body->block->nodes, 1);
    ASSERT_EQ(to_symbol("Green"), node->variant_type_node->tag);
    ASSERT_EQ(10, node->variant_type_node->tag_repr);
    node = array_get_ptr(&union_node->adt_type->body->block->nodes, 2);
    ASSERT_EQ(to_symbol("Blue"), node->variant_type_node->tag);
    ASSERT_EQ(11, node->variant_type_node->tag_repr);
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_variant()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_variant_member_type_float);
    RUN_TEST(test_analyzer_variant_member_type_int);
    RUN_TEST(test_analyzer_variant_enum_type_int);
    RUN_TEST(test_analyzer_variant_enum_type_int_manual_set);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
