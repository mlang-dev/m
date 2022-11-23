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
    ASSERT_EQ(Sum, node->adt_type->kind);
    ASSERT_EQ(BLOCK_NODE, node->adt_type->body->node_type);
    node = *(struct ast_node **)array_front(&node->adt_type->body->block->nodes);
    ASSERT_EQ(UNION_TYPE_ITEM_NODE, node->node_type);
    ASSERT_EQ(UntaggedUnion, node->union_type_item_node->kind);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser_union, block_definition)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
union XY = \n\
    | x:int \n\
    | y:float\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNION_NODE, node->node_type);
    ASSERT_EQ(Sum, node->adt_type->kind);
    ASSERT_EQ(BLOCK_NODE, node->adt_type->body->node_type);
    node = *(struct ast_node **)array_front(&node->adt_type->body->block->nodes);
    ASSERT_EQ(UNION_TYPE_ITEM_NODE, node->node_type);
    ASSERT_EQ(UntaggedUnion, node->union_type_item_node->kind);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser_union, untagged_union)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
union XY = \n\
    | x:int \n\
    | y:float";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNION_NODE, node->node_type);
    ASSERT_EQ(Sum, node->adt_type->kind);
    ASSERT_EQ(BLOCK_NODE, node->adt_type->body->node_type);
    node = *(struct ast_node **)array_front(&node->adt_type->body->block->nodes);
    ASSERT_EQ(UNION_TYPE_ITEM_NODE, node->node_type);
    ASSERT_EQ(UntaggedUnion, node->union_type_item_node->kind);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser_union, enum_union)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
union XY = \n\
    | Immutable \n\
    | Mutable";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNION_NODE, node->node_type);
    ASSERT_EQ(Sum, node->adt_type->kind);
    ASSERT_EQ(BLOCK_NODE, node->adt_type->body->node_type);
    node = *(struct ast_node **)array_front(&node->adt_type->body->block->nodes);
    ASSERT_EQ(UNION_TYPE_ITEM_NODE, node->node_type);
    ASSERT_EQ(EnumTagOnly, node->union_type_item_node->kind);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser_union, enum_union_assign_tag)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
union XY = \n\
    | Immutable = 0\n\
    | Mutable";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNION_NODE, node->node_type);
    ASSERT_EQ(Sum, node->adt_type->kind);
    ASSERT_EQ(BLOCK_NODE, node->adt_type->body->node_type);
    node = *(struct ast_node **)array_front(&node->adt_type->body->block->nodes);
    ASSERT_EQ(UNION_TYPE_ITEM_NODE, node->node_type);
    ASSERT_EQ(EnumTagValue, node->union_type_item_node->kind);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser_union, tagged_union)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
union XY = \n\
    | Rectangle(width:f64, height:f64) \n\
    | Square(f64) \n\
    | Circle(radius:int)";

    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(UNION_NODE, node->node_type);
    ASSERT_EQ(Sum, node->adt_type->kind);
    ASSERT_EQ(BLOCK_NODE, node->adt_type->body->node_type);
    node = *(struct ast_node **)array_front(&node->adt_type->body->block->nodes);
    ASSERT_EQ(UNION_TYPE_ITEM_NODE, node->node_type);
    ASSERT_EQ(TaggedUnion, node->union_type_item_node->kind);
    node_free(block);
    frontend_deinit(fe);
}

int test_parser_union()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_union_one_line_definition);
    RUN_TEST(test_parser_union_block_definition);
    RUN_TEST(test_parser_union_untagged_union);
    RUN_TEST(test_parser_union_enum_union);
    RUN_TEST(test_parser_union_tagged_union);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
