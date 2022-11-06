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

TEST(test_analyzer, call_node)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
fun printf __format:string ... -> int\n\
printf \"hello\"\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_back(&block->block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    ASSERT_EQ(TYPE_INT, node->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, ref_type_variable)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
x = 10\n\
y = &x";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_get(&block->block->nodes, 0);
    struct ast_node* y = *(struct ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_TRUE(x->is_addressed);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_EQ(0, x->type->val_type);
    ASSERT_EQ(TYPE_REF, y->type->type);
    ASSERT_EQ(to_symbol("&int"), y->type->name);
    ASSERT_TRUE(y->type->val_type);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, ref_type_func)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
struct AB = re:double, im:double\n\
let update z:&AB =\n\
    z.re = 10.0\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* fun = *(struct ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_EQ(TYPE_FUNCTION, fun->type->type);
    ASSERT_EQ(to_symbol("&AB -> ()"), fun->type->name);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, array_variable)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
a = [10, 20, 30]\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* array = *(struct ast_node **)array_get(&block->block->nodes, 0);
    ASSERT_EQ(TYPE_ARRAY, array->type->type);
    ASSERT_EQ(to_symbol("int[3]"), array->type->name);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, empty_array)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
a = []\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* array = *(struct ast_node **)array_get(&block->block->nodes, 0);
    ASSERT_EQ(TYPE_ARRAY, array->type->type);
    ASSERT_EQ(to_symbol("()[]"), array->type->name);
    ast_node_free(block);
    frontend_deinit(fe);
}

int test_analyzer()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_call_node);
    RUN_TEST(test_analyzer_ref_type_variable);
    RUN_TEST(test_analyzer_ref_type_func);
    RUN_TEST(test_analyzer_array_variable);
    RUN_TEST(test_analyzer_empty_array);
    return UNITY_END();
}
