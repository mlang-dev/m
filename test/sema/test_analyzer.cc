/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "codegen/llvm/cg_llvm.h"
#include "compiler/engine.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testAnalyzer, testIntVariable)
{
    char test_code[] = "x = 11";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_INT, node->var->init_value->type->type);
    ASSERT_EQ(KIND_OPER, node->type->kind);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testDoubleVariable)
{
    char test_code[] = "x = 11.0";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_DOUBLE, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testBoolVariable)
{
    char test_code[] = "x = true";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_BOOL, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testCharVariable)
{
    char test_code[] = "x = 'c'";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_CHAR, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("char", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStringVariable)
{
    char test_code[] = "x = \"hello world!\"";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_STRING, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("string", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testCallNode)
{
    char test_code[] = "printf \"hello\"";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(CALL_NODE, node->node_type);
    ASSERT_EQ(TYPE_INT, node->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testDoubleIntLiteralError)
{
    char test_code[] = "x = 11.0 + 10";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var_name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(0, node->type);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testGreaterThan)
{
    char test_code[] = R"(
11>10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testIdentityFunc)
{
    reset_id_name("a");
    char test_code[] = "let id x = x";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("id", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("a -> a", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testIntIntFunc)
{
    char test_code[] = "let f x = x + 10";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testDoubleDoubleFunc)
{
    char test_code[] = "let f x = x + 10.0";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("double -> double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testBoolFunc)
{
    char test_code[] = "let f x = !x";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool -> bool", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testMultiParamFunc)
{
    char test_code[] = "let avg x y = (x + y) / 2.0";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("avg", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(3, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("double * double -> double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testRecursiveFunc)
{
    char test_code[] = R"(
let factorial n = 
  if n < 2 then n
  else n * factorial (n-1)
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("factorial", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testForLoopFunc)
{
    char test_code[] = R"(
# using for loop
let loopprint n = 
  for i in 0..n
    printf "%d" i
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> ()", string_get(&type_str));
    ast_node *forn = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(TYPE_INT, get_type(forn->forloop->step->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->forloop->start->type));
    ASSERT_EQ(TYPE_BOOL, get_type(forn->forloop->end->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->forloop->body->type));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testForDoubleTypeLoopFunc)
{
    char test_code[] = R"(
# using for loop
let loopprint n:double = 
  for i in 0.0..1.0..n
    printf "%d" i
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("double -> ()", string_get(&type_str));
    ast_node *forn = *(ast_node **)array_front(&node->func->body->block->nodes);
    ASSERT_EQ(TYPE_DOUBLE, get_type(forn->forloop->step->type));
    ASSERT_EQ(TYPE_DOUBLE, get_type(forn->forloop->start->type));
    ASSERT_EQ(TYPE_BOOL, get_type(forn->forloop->end->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->forloop->body->type));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testLocalVariableFunc)
{
    char test_code[] = R"(
# using for loop
let distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
)";
    reset_id_name("a");
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("distance", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(5, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("double * double * double * double -> double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testLocalStringFunc)
{
    char test_code[] = R"(
let to_string () = 
  x = "hello"
  y = x
  y
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("to_string", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(1, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("() -> string", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testVariadicFunc)
{
    char test_code[] = R"(
let var_func ... = 0
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("var_func", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(true, node->func->func_type->ft->is_variadic);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("... -> int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testPrintfFunc)
{
    char test_code[] = R"(
printf "%d" 100
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(CALL_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStructLikeType)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_EQ(TYPE_STRUCT, node->type->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    engine_free(engine);
}

TEST(testAnalyzer, testFunctionTypeAnnotation)
{
    char test_code[] = R"(
let print x:int = printf "%d" x
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("print", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testFunctionTypeAnnotationWithParentheses)
{
    char test_code[] = R"(
let prt x:int = printf "%d" x
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("prt", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testFunctionTypeAnnotationWithReturnType)
{
    char test_code[] = R"(
let prt:int x:int = printf "%d" x
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("prt", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    auto var = (type_oper *)node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testVariableWithScope)
{
    char test_code[] = R"(
x = 10
let getx()=
    x = 1.3
    x
getx()
x
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(4, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    /*fun definition*/
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> double", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 3);
    ASSERT_EQ(IDENT_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testRedefinitionInTheSameScropeIsNotAllowed)
{
    char test_code[] = R"(
x = 10.0
x = 10
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    /*fun definition*/
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("type mismatch", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStructTypeVariables)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
xy:Point2D = Point2D(0.0, 0.0)
xy.x
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStructTypeVariablesNewForm)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
xy = Point2D(10.0, 20.0)
xy.x
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStructTypeLocalVariables)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
let getx()=
    xy:Point2D = Point2D(10.0, 0.0)
    xy.x
getx()
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> double", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStructTypeLocalVariablesNewForm)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
let getx()=
    xy = Point2D(10.0, 0.0)
    xy.x
getx()
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> double", string_get(&type_str));
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("double", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStructTypeReturn)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
let getx()=
    xy:Point2D = Point2D(10.0, 0.0)
    xy
z = getx()
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    /*variable node*/
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;

    /*initial value is a call expression*/
    ASSERT_EQ(CALL_NODE, var->var->init_value->node_type);
    type_str = to_string(var->var->init_value->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    type_str = to_string(var->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));

    /*verify variable xy in inner function is out of scope*/
    symbol xy = to_symbol("xy");
    ASSERT_EQ(false, has_symbol(&cg->base.sema_context->decl_2_typexps, xy));
    ast_node_free(block);
    engine_free(engine);
}


TEST(testAnalyzer, testStructTypeReturnNewForm)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
let getx()=
    xy = Point2D(10.0, 0.0)
    xy
z = getx()
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    /*variable node*/
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(VAR_NODE, node->node_type);
    struct ast_node *var = node;

    /*initial value is a call expression*/
    ASSERT_EQ(CALL_NODE, var->var->init_value->node_type);
    type_str = to_string(var->var->init_value->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    type_str = to_string(var->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));

    /*verify variable xy in inner function is out of scope*/
    symbol xy = to_symbol("xy");
    ASSERT_EQ(false, has_symbol(&cg->base.sema_context->decl_2_typexps, xy));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testStructTypeReturnNoNamed)
{
    char test_code[] = R"(
struct Point2D = x:double, y:double
let get_point() = Point2D(10.0, 0.0)
let z() = get_point()
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    //1. type definition
    auto node = *(ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    //2. function definition
    node = *(ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    //3. function definition again
    node = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testReturnValueFlag)
{
    char test_code[] = R"(
let getx()=
    x = 10
    y = x + 1
    y
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    /*validate fun definition*/
    auto node = *(ast_node **)array_get(&block->block->nodes, 0);
    auto type_str = to_string(node->type);
    ASSERT_STREQ("() -> int", string_get(&type_str));

    /*validate inside functions*/
    auto fun = (ast_node *)node;
    auto var_x = *(ast_node **)array_get(&fun->func->body->block->nodes, 0);
    auto var_y = *(ast_node **)array_get(&fun->func->body->block->nodes, 1);
    ASSERT_EQ(false, var_x->is_ret);
    ASSERT_EQ(true, var_y->is_ret);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, testReturnExpression)
{
    char test_code[] = R"(
let getx()=
    x = 10
    x + 1
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    /*validate fun definition*/
    auto node = *(ast_node **)array_get(&block->block->nodes, 0);
    auto type_str = to_string(node->type);
    ASSERT_STREQ("() -> int", string_get(&type_str));

    /*validate inside functions*/
    auto fun = (ast_node *)node;
    auto var_x = *(ast_node **)array_get(&fun->func->body->block->nodes, 0);
    auto exp = *(ast_node **)array_get(&fun->func->body->block->nodes, 1);
    ASSERT_EQ(false, var_x->is_ret);
    ASSERT_EQ(BINARY_NODE, exp->node_type);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzer, ref_type_variable)
{
    char test_code[] = R"(
x = 10
y = &x
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, "test");
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    struct ast_node* x = *(struct ast_node **)array_get(&block->block->nodes, 0);
    struct ast_node* y = *(struct ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_EQ(TYPE_INT, x->type->type);
    ASSERT_FALSE(x->type->is_ref);
    ASSERT_EQ(TYPE_INT, y->type->type);
    ast_node_free(block);
    engine_free(engine);
}
