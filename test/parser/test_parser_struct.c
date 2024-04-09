/*
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parsing struct type
 */
#include "parser/parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "sema/frontend.h"
#include <stdio.h>

TEST(test_parser_struct, type_decl)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let point:Point2D";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_back_ptr(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(0, node->var->init_value);
    ASSERT_STREQ("point", string_get(node->var->var->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser_struct, type_decl2)
{
    char test_code[] = "\n\
struct Point2D = \n\
  x:f64 \n\
  y:f64";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_back_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->adt_type->name));
    ASSERT_EQ(2, array_size(&node->adt_type->body->block->nodes));
    struct ast_node *var1 = array_front_ptr(&node->adt_type->body->block->nodes);
    struct ast_node *var2 = array_back_ptr(&node->adt_type->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var->ident->name));
    ASSERT_STREQ("y", string_get(var2->var->var->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser_struct, type_var_init)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
let xy = Point2D{10.0, 20.0}";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->adt_type->name));
    ASSERT_EQ(2, array_size(&node->adt_type->body->block->nodes));
    struct ast_node *var1 = array_front_ptr(&node->adt_type->body->block->nodes);
    struct ast_node *var2 = array_back_ptr(&node->adt_type->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var->ident->name));
    ASSERT_STREQ("y", string_get(var2->var->var->ident->name));

    // struct type variable assignement
    struct ast_node *var = array_back_ptr(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, var->node_type);
    ASSERT_EQ(ADT_INIT_NODE, var->var->init_value->node_type);
    struct ast_node *value_node = var->var->init_value;
    struct ast_node *value1 = array_front_ptr(&value_node->adt_init->body->block->nodes);
    struct ast_node *value2 = array_back_ptr(&value_node->adt_init->body->block->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->node_type);
    ASSERT_EQ(LITERAL_NODE, value2->node_type);
    ASSERT_EQ(10.0, value1->liter->double_val);
    ASSERT_EQ(20.0, value2->liter->double_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser_struct, func_returns_struct_init)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
def get_point(): Point2D{10.0, 20.0}";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));

    // 1. first line is to define type
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    ASSERT_STREQ("Point2D", string_get(node->adt_type->name));
    ASSERT_EQ(2, array_size(&node->adt_type->body->block->nodes));
    struct ast_node *var1 = array_front_ptr(&node->adt_type->body->block->nodes);
    struct ast_node *var2 = array_back_ptr(&node->adt_type->body->block->nodes);
    ASSERT_EQ(VAR_NODE, var1->node_type);
    ASSERT_EQ(VAR_NODE, var2->node_type);
    ASSERT_STREQ("x", string_get(var1->var->var->ident->name));
    ASSERT_STREQ("y", string_get(var2->var->var->ident->name));

    // 2. second line is to define a one line function
    struct ast_node *fun_node = array_back_ptr(&block->block->nodes);
    ASSERT_EQ(FUNC_NODE, fun_node->node_type);
    ASSERT_EQ(1, array_size(&fun_node->func->body->block->nodes));
    struct ast_node *tv_node = array_back_ptr(&fun_node->func->body->block->nodes);
    ASSERT_EQ(ADT_INIT_NODE, tv_node->node_type);
    struct ast_node *value1 = array_front_ptr(&tv_node->adt_init->body->block->nodes);
    struct ast_node *value2 = array_back_ptr(&tv_node->adt_init->body->block->nodes);
    ASSERT_EQ(LITERAL_NODE, value1->node_type);
    ASSERT_EQ(LITERAL_NODE, value2->node_type);
    ASSERT_EQ(10.0, value1->liter->double_val);
    ASSERT_EQ(20.0, value2->liter->double_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser_struct, use_type_field)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
let xy:Point2D = Point2D{0.0, 0.0} \n\
xy.x";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    node = array_get_ptr(&block->block->nodes, 1);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;
    ASSERT_STREQ("xy", string_get(var->var->var->ident->name));
    ASSERT_STREQ("Point2D", string_get(var->var->is_of_type->type_item_node->type_name));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    ASSERT_EQ(IDENT_NODE, node->index->object->node_type);
    ASSERT_EQ(IDENT_NODE, node->index->index->node_type);
    ASSERT_STREQ("xy", string_get(node->index->object->ident->name));
    ASSERT_STREQ("x", string_get(node->index->index->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser_struct, member_field_assignment)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
let mut xy:Point2D = Point2D{0.0, 0.0} \n\
xy.x = 10.0";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    node = array_get_ptr(&block->block->nodes, 1);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;
    ASSERT_STREQ("xy", string_get(var->var->var->ident->name));
    ASSERT_STREQ("Point2D", string_get(var->var->is_of_type->type_item_node->type_name));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(ASSIGN_NODE, node->node_type);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->binop->lhs->node_type);
    ASSERT_EQ(LITERAL_NODE, node->binop->rhs->node_type);
    ASSERT_EQ(OP_ASSIGN, node->binop->opcode);
    node = node->binop->lhs;
    ASSERT_STREQ("xy", string_get(node->index->object->ident->name));
    ASSERT_STREQ("x", string_get(node->index->index->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser_struct, new_delete_struct)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64 \n\
let xy = new Point2D{0.0, 0.0} \n\
del xy";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    node = array_get_ptr(&block->block->nodes, 1);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;
    ASSERT_STREQ("xy", string_get(var->var->var->ident->name));
    node = array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(DEL_NODE, node->node_type);
    ASSERT_EQ(IDENT_NODE, node->del_node->node_type);
    ASSERT_STREQ("xy", string_get(node->del_node->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

int test_parser_struct(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_struct_type_decl);
    RUN_TEST(test_parser_struct_type_decl2);
    RUN_TEST(test_parser_struct_type_var_init);
    RUN_TEST(test_parser_struct_func_returns_struct_init);
    RUN_TEST(test_parser_struct_use_type_field);
    RUN_TEST(test_parser_struct_member_field_assignment);
    RUN_TEST(test_parser_struct_new_delete_struct);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
