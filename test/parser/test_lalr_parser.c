/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "parser/lalr_parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "lexer/init.h"
#include <stdio.h>

TEST(test_lalr_parser, var_decl)
{
    frontend_init();
    char test_code[] = "\n\
type Point2D = x:double y:double\n\
point:Point2D";
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node* node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_EQ(BLOCK_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&node->block->nodes));
    node = *(struct ast_node **)array_back(&node->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(0, node->var->init_value);
    ASSERT_STREQ("point", string_get(node->var->var_name));
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, int_type)
{
    frontend_init();
    char test_code[] = "x:int";
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    block = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(0, node->var->init_value);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, var_init)
{
    frontend_init();
    char test_code[] = "x = 11";
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    block = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, var_init_name_with_underlying)
{
    char test_code[] = "m_x = 11";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    block = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("m_x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, var_type_init)
{
    frontend_init();
    char test_code[] = "x:int = 11";
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    block = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("int", string_get(node->annotated_type_name));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, bool_init)
{
    frontend_init();
    char test_code[] = "x = true";
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    block = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(true, literal->liter->bool_val);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, char_init)
{
    char test_code[] = "x = 'c'";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    block = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ('c', literal->liter->char_val);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, string_init)
{
    char test_code[] = "x = \"hello world!\"";
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    block = *(struct ast_node **)array_front(&block->block->nodes);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_STREQ("hello world!", literal->liter->str_val);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST(test_lalr_parser, id_func)
{
/*     char test_code[] = "\n\
f x = x \n\
f 10";*/
    frontend_init();
    // struct lalr_parser *parser = parser_new();
    // struct ast_node *block = parse_code(parser, test_code);
    // struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    // struct ast_node *body_node = *(struct ast_node **)array_front(&node->func->body->block->nodes);
    // ASSERT_EQ(2, array_size(&block->block->nodes));
    // ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    // ASSERT_EQ(IDENT_NODE, body_node->node_type);
    // lalr_parser_free(parser);
    frontend_deinit();
}

int test_lr_parser()
{
    UNITY_BEGIN();
    RUN_TEST(test_lalr_parser_var_decl);
    RUN_TEST(test_lalr_parser_int_type);
    RUN_TEST(test_lalr_parser_var_init);
    RUN_TEST(test_lalr_parser_var_init_name_with_underlying);
    RUN_TEST(test_lalr_parser_var_type_init);
    RUN_TEST(test_lalr_parser_bool_init);
    RUN_TEST(test_lalr_parser_char_init);
    RUN_TEST(test_lalr_parser_string_init);
    RUN_TEST(test_lalr_parser_id_func);
    return UNITY_END();
}
