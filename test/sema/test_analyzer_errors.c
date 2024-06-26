/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * c Unit tests for type inference and semantic analsysis errors
 */
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "sema/frontend.h"
#include "app/error.h"
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

    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    struct error_report *er = get_last_error_report(fe->sema_context);
    ASSERT_STREQ("function is not defined.", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, type_mismatch)
{
    char test_code[] = "\n\
let x:int = \"string\"\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node* node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct error_report *er = get_last_error_report(fe->sema_context);
    ASSERT_STREQ("variable type not matched with literal constant.", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, no_struct_type_found)
{
    char test_code[] = "\n\
let x = 3\n\
x.y\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_EXPECT_ADT_TYPE, er->error_code);
    ASSERT_STREQ("The left hand is expected to be an ADT type.", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, no_array_type_found)
{
    char test_code[] = "\n\
let x = 3\n\
x[3]\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_EXPECT_ADT_TYPE, er->error_code);
    ASSERT_STREQ("The left hand is expected to be an ADT type.", er->error_msg);
    node_free(block);
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
    ASSERT_STREQ("x is not defined", er->error_msg);
    node_free(block);
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
    ASSERT_STREQ("x is not defined", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, int_to_float_mismatch)
{
    char test_code[] = "\n\
let mut x = 10\n\
x = 10.0\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_EQ(EC_TYPES_DO_NOT_MATCH, er->error_code);
    ASSERT_STREQ("types do not match", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_mutable)
{
    char test_code[] = "\n\
let x = 10\n\
x = 20\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_TRUE(er);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("x is immutable, it can't be mutated", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_assignable)
{
    char test_code[] = "\n\
let x = 10\n\
x += 20\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_TRUE(er);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("x is immutable, it can't be mutated", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, id_not_inc)
{
    char test_code[] = "\n\
let x = 10\n\
x ++\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_TRUE(er);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("x is immutable, it can't be mutated", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer_error, struct_member_immutable)
{
    char test_code[] = "\n\
struct Point2D = x:f32, y:f32\n\
let mut p = Point2D { 1.0, 2.0 }\n\
p.x = (f32)10.0\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct error_report* er = get_last_error_report(fe->sema_context);
    ASSERT_TRUE(er);
    ASSERT_EQ(EC_IMMUTABLE_ASSIGNMENT, er->error_code);
    ASSERT_STREQ("p.x is immutable, it can't be mutated", er->error_msg);
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer_errors(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_error_fun_no_found);
    RUN_TEST(test_analyzer_error_type_mismatch);
    RUN_TEST(test_analyzer_error_no_struct_type_found);
    RUN_TEST(test_analyzer_error_no_array_type_found);
    RUN_TEST(test_analyzer_error_id_not_defined);
    RUN_TEST(test_analyzer_error_id_not_defined_unary);
    RUN_TEST(test_analyzer_error_int_to_float_mismatch);
    RUN_TEST(test_analyzer_error_id_not_mutable);
    RUN_TEST(test_analyzer_error_id_not_assignable);
    RUN_TEST(test_analyzer_error_id_not_inc);
    RUN_TEST(test_analyzer_error_struct_member_immutable);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
