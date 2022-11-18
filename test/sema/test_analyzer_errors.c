/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * c Unit tests for type inference and semantic analsysis errors
 */
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "sema/frontend.h"
#include "error/error.h"
#include "compiler/engine.h"
#include "tutil.h"
#include <stdio.h>
#include "test.h"


TEST(test_analyzer_error, fun_no_found)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "no_exist_function_call ()";
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);

    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    struct error_report *er = get_last_error_report(fe->sema_context);
    ASSERT_STREQ("function is not defined.", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, type_mismatch)
{
    char test_code[] = "\n\
x:int = \"string\"\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node* node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct error_report *er = get_last_error_report(fe->sema_context);
    ASSERT_STREQ("variable type not matched with literal constant.", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, no_struct_type_found)
{
    char test_code[] = "\n\
x = 3\n\
x.y\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_EXPECT_STRUCT_TYPE, er->error_code);
    ASSERT_STREQ("The left side of the dot is expected to be a struct type.", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, no_array_type_found)
{
    char test_code[] = "\n\
x = 3\n\
x[3]\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_EXPECT_ARRAY_TYPE, er->error_code);
    ASSERT_STREQ("The left side of the bracket is expected to be an array type.", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_defined)
{
    char test_code[] = "\n\
x + 3\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_IDENT_NOT_DEFINED, er->error_code);
    ASSERT_STREQ("id: x is not defined", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_defined_unary)
{
    char test_code[] = "\n\
-x\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_IDENT_NOT_DEFINED, er->error_code);
    ASSERT_STREQ("id: x is not defined", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_mutable)
{
    char test_code[] = "\n\
x = 10\n\
x = 20\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("id: x is immutable, it can't be mutated", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_assignable)
{
    char test_code[] = "\n\
x = 10\n\
x += 20\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("id: x is immutable, it can't be mutated", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_inc)
{
    char test_code[] = "\n\
x = 10\n\
x ++\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("id: x is immutable, it can't be mutated", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, struct_member_immutable)
{
    char test_code[] = "\n\
struct Point2D = x:f32, y:f32\n\
var p = Point2D(1.0, 2.0)\n\
p.x = 10.0\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("id: x is immutable, it can't be mutated", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, int_to_float)
{
    char test_code[] = "\n\
var x = 10\n\
x = \"10.0\"\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_TYPES_DO_NOT_MATCH, er->error_code);
    ASSERT_STREQ("types do not match", er->error_msg);
    ast_node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_errors()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_error_fun_no_found);
    RUN_TEST(test_analyzer_error_type_mismatch);
    RUN_TEST(test_analyzer_error_no_struct_type_found);
    RUN_TEST(test_analyzer_error_no_array_type_found);
    RUN_TEST(test_analyzer_error_id_not_defined);
    RUN_TEST(test_analyzer_error_id_not_defined_unary);
    RUN_TEST(test_analyzer_error_id_not_mutable);
    RUN_TEST(test_analyzer_error_id_not_assignable);
    RUN_TEST(test_analyzer_error_id_not_inc);
    RUN_TEST(test_analyzer_error_struct_member_immutable);
    RUN_TEST(test_analyzer_error_int_to_float);
    return UNITY_END();
}
