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

TEST(test_analyzer_union, member_type_float)
{
    char test_code[] = "\n\
union XorY = x:f64 | y:int\n\
xy = XorY{0.0}\n\
xy.x\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_union, member_type_int)
{
    char test_code[] = "\n\
union XorY = x:f64 | y:int\n\
xy:XorY = {0.0}\n\
xy.y\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("XorY", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_union, enum_type_int)
{
    char test_code[] = "\n\
union Color = Red | Green | Blue\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *union_node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(union_node->type);
    ASSERT_STREQ("Color", string_get(&type_str));
    ASSERT_EQ(UNION_NODE, union_node->node_type);
    ASSERT_EQ(3, array_size(&union_node->adt_type->body->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&union_node->adt_type->body->block->nodes);
    ASSERT_EQ(to_symbol("Red"), node->union_type_item_node->tag);
    ASSERT_EQ(0, node->union_type_item_node->tag_repr);
    node = *(struct ast_node **)array_get(&union_node->adt_type->body->block->nodes, 1);
    ASSERT_EQ(to_symbol("Green"), node->union_type_item_node->tag);
    ASSERT_EQ(1, node->union_type_item_node->tag_repr);
    node = *(struct ast_node **)array_get(&union_node->adt_type->body->block->nodes, 2);
    ASSERT_EQ(to_symbol("Blue"), node->union_type_item_node->tag);
    ASSERT_EQ(2, node->union_type_item_node->tag_repr);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_union, enum_type_int_manual_set)
{
    char test_code[] = "\n\
union Color = Red | Green = 10 | Blue\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *union_node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(union_node->type);
    ASSERT_STREQ("Color", string_get(&type_str));
    ASSERT_EQ(UNION_NODE, union_node->node_type);
    ASSERT_EQ(3, array_size(&union_node->adt_type->body->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&union_node->adt_type->body->block->nodes);
    ASSERT_EQ(to_symbol("Red"), node->union_type_item_node->tag);
    ASSERT_EQ(0, node->union_type_item_node->tag_repr);
    node = *(struct ast_node **)array_get(&union_node->adt_type->body->block->nodes, 1);
    ASSERT_EQ(to_symbol("Green"), node->union_type_item_node->tag);
    ASSERT_EQ(10, node->union_type_item_node->tag_repr);
    node = *(struct ast_node **)array_get(&union_node->adt_type->body->block->nodes, 2);
    ASSERT_EQ(to_symbol("Blue"), node->union_type_item_node->tag);
    ASSERT_EQ(11, node->union_type_item_node->tag_repr);
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_union()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_union_member_type_float);
    RUN_TEST(test_analyzer_union_member_type_int);
    RUN_TEST(test_analyzer_union_enum_type_int);
    RUN_TEST(test_analyzer_union_enum_type_int_manual_set);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
