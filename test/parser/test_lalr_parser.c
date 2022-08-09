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
    /*
    char test_code[] = "\n\
type Point2D = x:double y:double\n\
point:Point2D";
    struct lalr_parser *parser = parser_new();
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node* node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_STREQ("point", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(0, node->var->init_value);
    */
    frontend_deinit();
}

int test_lr_parser()
{
    UNITY_BEGIN();
    RUN_TEST(test_lalr_parser_var_decl);
    return UNITY_END();
}
