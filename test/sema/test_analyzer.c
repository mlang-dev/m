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
    node_free(block);
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
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, ref_type_func)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
struct AB = re:f64, im:f64\n\
let update z:&AB =\n\
    z.re = 10.0\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* fun = *(struct ast_node **)array_get(&block->block->nodes, 1);
    ASSERT_EQ(TYPE_FUNCTION, fun->type->type);
    ASSERT_EQ(to_symbol("&AB -> ()"), fun->type->name);
    node_free(block);
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
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, empty_array)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n\
var a = []\n\
";
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    struct ast_node* array = *(struct ast_node **)array_get(&block->block->nodes, 0);
    ASSERT_EQ(TYPE_ARRAY, array->type->type);
    ASSERT_EQ(to_symbol("()[]"), array->type->name);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, int_variable)
{
    char test_code[] = "x = 11";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_INT, node->var->init_value->type->type);
    ASSERT_EQ(KIND_OPER, node->type->kind);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, double_variable)
{
    char test_code[] = "x = 11.0";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_F64, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, bool_variable)
{
    char test_code[] = "x = true";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_BOOL, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, char_variable)
{
    char test_code[] = "x = 'c'";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_CHAR, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("char", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, string_variable)
{
    char test_code[] = "x = \"hello world!\"";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_STRING, node->var->init_value->type->type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("string", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, double_int_literal_error)
{
    char test_code[] = "x = 11.0 + 10";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(TYPE_F64, node->type->type);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, greater_than)
{
    char test_code[] = "11>10";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    analyze(fe->sema_context, block);
    
    ASSERT_EQ(BINARY_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, identity_function)
{
    reset_id_name("a");
    char test_code[] = "let id x = x";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("id", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("a -> a", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, int_int_fun)
{
    char test_code[] = "let f x = x + 10";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, double_double_fun)
{
    char test_code[] = "let f x = x + 10.0";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("f64 -> f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, bool_fun)
{
    char test_code[] = "let f x = !x";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool -> bool", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, multi_param_fun)
{
    char test_code[] = "let avg x y = (x + y) / 2.0";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("avg", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(3, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("f64 * f64 -> f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, recur_fun)
{
    char test_code[] = "\n\
let factorial n = \n\
  if n < 2 then n\n\
  else n * factorial (n-1)\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("factorial", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, for_loop_fun)
{
    char test_code[] = "\n\
# using for loop\n\
let loopprint n = \n\
  var sum = 0\n\
  for i in 0..n\n\
    sum += i\n\
  sum\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    struct ast_node *forn = *(struct ast_node **)array_get(&node->func->body->block->nodes, 1);
    ASSERT_EQ(FOR_NODE, forn->node_type);
    ASSERT_EQ(TYPE_INT, get_type(forn->forloop->range->range->step->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->forloop->range->range->start->type));
    ASSERT_EQ(TYPE_BOOL, get_type(forn->forloop->range->range->end->type));
    ASSERT_EQ(TYPE_UNIT, get_type(forn->forloop->body->type));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, float_var_loop)
{
    char test_code[] = "\n\
# using for loop\n\
let loopprint n:f64 =\n\
  var sum = 0.0\n\
  for i in 0.0..1.0..n\n\
    sum += i\n\
  sum\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("f64 -> f64", string_get(&type_str));
    struct ast_node *forn = *(struct ast_node **)array_get(&node->func->body->block->nodes, 1);
    ASSERT_EQ(FOR_NODE, forn->node_type);
    ASSERT_EQ(TYPE_F64, get_type(forn->forloop->range->range->step->type));
    ASSERT_EQ(TYPE_F64, get_type(forn->forloop->range->range->start->type));
    ASSERT_EQ(TYPE_BOOL, get_type(forn->forloop->range->range->end->type));
    ASSERT_EQ(TYPE_UNIT, get_type(forn->forloop->body->type));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, local_var_fun)
{
    char test_code[] = "\n\
# using for loop\n\
let distance x1:f64 y1:f64 x2 y2 = \n\
  xx = (x1-x2) * (x1-x2)\n\
  yy = (y1-y2) * (y1-y2)\n\
  |/ (xx + yy)\n\
";
    reset_id_name("a");
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("distance", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(5, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("f64 * f64 * f64 * f64 -> f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, local_string_fun)
{
    char test_code[] = "\n\
let to_string () = \n\
  x = \"hello\"\n\
  y = x\n\
  y\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("to_string", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(1, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("() -> string", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, varadic_fun)
{
    char test_code[] = "\n\
let var_func ... = 0\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("var_func", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(true, node->func->func_type->ft->is_variadic);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("... -> int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, printf_fun)
{
    char test_code[] = "printf \"%d\" 100";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(CALL_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("type mismatch", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, struct_type)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(STRUCT_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_EQ(TYPE_STRUCT, node->type->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, fun_type_annotation)
{
    char test_code[] = "\n\
let inc x:int = x + 1\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("inc", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, fun_type_with_ret_type)
{
    char test_code[] = "\n\
let inc:int x:int = x + 1\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("inc", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    struct type_expr *var = node->type;
    ASSERT_EQ(TYPE_FUNCTION, var->type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, var_in_scope)
{
    char test_code[] = "\n\
x = 10\n\
let getx()=\n\
    y = 1.3\n\
    y\n\
getx()\n\
x\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(4, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    /*fun definition*/
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> f64", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 3);
    ASSERT_EQ(IDENT_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("int", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, struct_type_vars)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
xy:Point2D = Point2D(0.0, 0.0)\n\
xy.x\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(MEMBER_INDEX_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, struct_type_local_var)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let getx()=\n\
    xy:Point2D = Point2D(10.0, 0.0)\n\
    xy.x\n\
getx()\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> f64", string_get(&type_str));
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("f64", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, ret_struct_type)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let getx()=\n\
    xy:Point2D = Point2D(10.0, 0.0)\n\
    xy\n\
z = getx()\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    /*fun definition*/
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    /*variable node*/
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
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
    ASSERT_EQ(false, has_symbol(&fe->sema_context->decl_2_typexprs, xy));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, ret_struct_type_direct)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
let get_point() = Point2D(10.0, 0.0)\n\
let z() = get_point()\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    //1. type definition
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("Point2D", string_get(&type_str));
    //2. function definition
    node = *(struct ast_node **)array_get(&block->block->nodes, 1);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));

    //3. function definition again
    node = *(struct ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    type_str = to_string(node->type);
    ASSERT_STREQ("() -> Point2D", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, array_type_decl)
{
    char test_code[] = "\n\
a:u8[2]\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    string type_str = to_string(node->type);
    ASSERT_STREQ("u8[2]", string_get(&type_str));
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, ret_value_flag)
{
    char test_code[] = "\n\
let getx()=\n\
    x = 10\n\
    y = x + 1\n\
    y\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    /*validate fun definition*/
    struct ast_node *node = *(struct ast_node **)array_get(&block->block->nodes, 0);
    string type_str = to_string(node->type);
    ASSERT_STREQ("() -> int", string_get(&type_str));

    /*validate inside functions*/
    struct ast_node *var_x = *(struct ast_node **)array_get(&node->func->body->block->nodes, 0);
    struct ast_node *var_y = *(struct ast_node **)array_get(&node->func->body->block->nodes, 1);
    ASSERT_EQ(false, var_x->is_ret);
    ASSERT_EQ(true, var_y->is_ret);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_analyzer, ret_expr)
{
    char test_code[] = "\n\
let getx()=\n\
    x = 10\n\
    x + 1\n\
";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    analyze(fe->sema_context, block);
    
    /*validate fun definition*/
    struct ast_node *node = *(struct ast_node **)array_get(&block->block->nodes, 0);
    string type_str = to_string(node->type);
    ASSERT_STREQ("() -> int", string_get(&type_str));

    /*validate inside functions*/
    struct ast_node *var_x = *(struct ast_node **)array_get(&node->func->body->block->nodes, 0);
    struct ast_node *exp = *(struct ast_node **)array_get(&node->func->body->block->nodes, 1);
    ASSERT_EQ(false, var_x->is_ret);
    ASSERT_EQ(BINARY_NODE, exp->node_type);
    node_free(block);
    frontend_deinit(fe);
}

int test_analyzer()
{
    UNITY_BEGIN();
    RUN_TEST(test_analyzer_call_node);
    RUN_TEST(test_analyzer_ref_type_variable);
    RUN_TEST(test_analyzer_ref_type_func);
    RUN_TEST(test_analyzer_array_variable);
    RUN_TEST(test_analyzer_array_type_decl);
    RUN_TEST(test_analyzer_empty_array);
    RUN_TEST(test_analyzer_int_variable);
    RUN_TEST(test_analyzer_double_variable);
    RUN_TEST(test_analyzer_bool_fun);
    RUN_TEST(test_analyzer_bool_variable);
    RUN_TEST(test_analyzer_char_variable);
    RUN_TEST(test_analyzer_double_double_fun);
    RUN_TEST(test_analyzer_double_int_literal_error);
    RUN_TEST(test_analyzer_double_variable);
    RUN_TEST(test_analyzer_empty_array);
    RUN_TEST(test_analyzer_float_var_loop);
    RUN_TEST(test_analyzer_for_loop_fun);
    RUN_TEST(test_analyzer_fun_type_annotation);
    RUN_TEST(test_analyzer_fun_type_with_ret_type);
    RUN_TEST(test_analyzer_greater_than);
    RUN_TEST(test_analyzer_identity_function);
    RUN_TEST(test_analyzer_int_int_fun);
    RUN_TEST(test_analyzer_local_string_fun);
    RUN_TEST(test_analyzer_local_var_fun);
    RUN_TEST(test_analyzer_multi_param_fun);
    RUN_TEST(test_analyzer_printf_fun);
    RUN_TEST(test_analyzer_recur_fun);
    RUN_TEST(test_analyzer_ref_type_func);
    RUN_TEST(test_analyzer_ref_type_variable);
    RUN_TEST(test_analyzer_ret_expr);
    RUN_TEST(test_analyzer_ret_struct_type);
    RUN_TEST(test_analyzer_ret_struct_type_direct);
    RUN_TEST(test_analyzer_ret_value_flag);
    RUN_TEST(test_analyzer_string_variable);
    RUN_TEST(test_analyzer_struct_type);
    RUN_TEST(test_analyzer_struct_type_local_var);
    RUN_TEST(test_analyzer_struct_type_vars);
    RUN_TEST(test_analyzer_var_in_scope);
    RUN_TEST(test_analyzer_varadic_fun);
    return UNITY_END();
}
