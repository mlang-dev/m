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

TEST(test_parser, int_type)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "let x:int";
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(BLOCK_NODE, block->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(TYPE_ITEM_NODE, node->var->is_of_type->node_type);
    ASSERT_EQ(0, node->var->init_value);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, var_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "let x = 11";
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, mut_var_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "let mut x = 11";
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(Mutable, node->var->mut);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, immut_var_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "let x = 11";
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(Immutable, node->var->mut);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, var_init_name_with_underlying)
{
    char test_code[] = "let m_x = 11";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("m_x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, int_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "let x:int = 11";
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("int", string_get(node->var->is_of_type->type_item_node->type_name));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(11, literal->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, bool_init)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "let x = True";
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ(true, literal->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, char_init)
{
    char test_code[] = "let x = 'c'";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_EQ('c', literal->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, string_init)
{
    char test_code[] = "let x = \"hello world!\"";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("x", string_get(node->var->var->ident->name));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(LITERAL_NODE, node->var->init_value->node_type);
    struct ast_node *literal = node->var->init_value;
    ASSERT_STREQ("hello world!", literal->liter->str_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, id_func)
{
    char test_code[] = "\n\
def f(x): x\n\
f(10) ";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *func = array_front_ptr(&block->block->nodes);
    struct ast_node *call = array_back_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&func->func->body->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(func->func->func_type->ft->name));
    ASSERT_EQ(IDENT_NODE, body_node->node_type);
    ASSERT_EQ(CALL_NODE, call->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, id_func_with_return)
{
    char test_code[] = "\n\
def f(x): return x\n\
f(10) ";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *func = array_front_ptr(&block->block->nodes);
    struct ast_node *call = array_back_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&func->func->body->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(func->func->func_type->ft->name));
    ASSERT_EQ(JUMP_NODE, body_node->node_type);
    ASSERT_EQ(CALL_NODE, call->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, binary_exp_func)
{
    char test_code[] = "def f(x): x * x";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, func_with_new_line)
{
    char test_code[] = "\n\
def f(x): \n\
    x * x";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("f", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(BINARY_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, distance_function)
{
    char test_code[] = "\n\
def distance(x1, y1, x2, y2): \n\
  let xx = (x1-x2) * (x1-x2) \n\
  let yy = (y1-y2) * (y1-y2) \n\
  |/ (xx + yy)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_first = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(FUNC_NODE, node->node_type);
    ASSERT_STREQ("distance", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(VAR_NODE, body_first->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, for_loop)
{
    char test_code[] = "\n\
def loopprint(n): \n\
  for i in 3..n: \n\
    print(i)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FOR_NODE, body_node->node_type);
    ASSERT_EQ(0, body_node->forloop->range->range->step);
    ASSERT_EQ(IDENT_NODE, body_node->forloop->range->range->end->node_type);
    ASSERT_EQ(3, ((struct ast_node *)body_node->forloop->range->range->start)->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, for_loop_with_step)
{
    char test_code[] = "\n\
def loopprint(n): \n\
  for i in 3..2..n: \n\
    print(i)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("loopprint", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(FOR_NODE, body_node->node_type);
    ASSERT_EQ(2, body_node->forloop->range->range->step->liter->int_val);
    ASSERT_EQ(IDENT_NODE, body_node->forloop->range->range->end->node_type);
    ASSERT_EQ(3, ((struct ast_node *)body_node->forloop->range->range->start)->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, avg_function)
{
    char test_code[] = "def avg(x, y): (x + y) / 2";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *func = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&func->func->body->block->nodes);
    ASSERT_STREQ("avg", string_get(func->func->func_type->ft->name));
    ASSERT_STREQ("BINARY_NODE", node_type_strings[body_node->node_type]);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, if_condition)
{
    char test_code[] = "\n\
def fac(n): \n\
    if n< 2: n \n\
    else: n * fac (n-1)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_one_line)
{
    char test_code[] = "\n\
def fac(n): \n\
    if n< 2: n else: n * fac (n-1)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_one_block)
{
    char test_code[] = "\n\
def fac(n): \n\
    if n< 2: \n\
        n \n\
    else: n * fac (n-1)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_two_blocks)
{
    char test_code[] = "\n\
def fac(n): \n\
    if n< 2: \n\
        n \n\
    else: \n\
        n * fac (n-1)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_three_blocks)
{
    char test_code[] = "\n\
def fac(n): \n\
    if n< 2: \n\
        n \n\
    elif n == 2:\n\
        n + 1\n\
    else: \n\
        n * fac (n-1)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, if_condition_no_else)
{
    char test_code[] = "\n\
def fac(n): \n\
    if n< 2: n \n\
    n * fac (n-1)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("fac", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(IF_NODE, body_node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, pattern_match)
{
    char test_code[] = "\n\
def pm(n): \n\
    match n with\n\
    | 0 -> 100\n\
    | 1 -> 300\n\
    | _ -> 400\n\
pm(1)";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct ast_node *body_node = array_front_ptr(&node->func->body->block->nodes);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    ASSERT_STREQ("pm", string_get(node->func->func_type->ft->name));
    ASSERT_EQ(MATCH_NODE, body_node->node_type);
    ASSERT_EQ(BLOCK_NODE, body_node->match->match_cases->node_type);
    node = array_back_ptr(&block->block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, pattern_match_variable)
{
    char test_code[] = "\n\
let n = 10\n\
let y = \n\
    match n with\n\
    | 0 -> 100\n\
    | 1 -> 300\n\
    | _ -> 400\n\
y";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(3, array_size(&block->block->nodes));
    struct ast_node *var = array_get_ptr(&block->block->nodes, 1);
    ASSERT_EQ(VAR_NODE, var->node_type);
    ASSERT_EQ(MATCH_NODE, var->var->init_value->node_type);
    struct ast_node *ident = array_back_ptr(&block->block->nodes);
    ASSERT_EQ(IDENT_NODE, ident->node_type);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser, local_string_function)
{
    char test_code[] = "\n\
def to_string (): \n\
  let x = \"hello\" \n\
  let y = x \n\
  y";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("to_string", string_get(node->func->func_type->ft->name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, variadic_function)
{
    char test_code[] = "def f(x, ...): 10";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(FUNC_NODE, node->node_type);
    ASSERT_EQ(true, node->func->func_type->ft->is_variadic);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, func_type)
{
    char test_code[] = "fun printf(format:string, ...) -> None";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_STREQ("printf", string_get(node->ft->name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, func_type_no_param)
{
    char test_code[] = "fun print () -> int";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(0, array_size(&node->ft->params->block->nodes));
    ASSERT_STREQ("print", string_get(node->ft->name));
    ASSERT_STREQ("int", string_get(node->ft->ret_type_item_node->type_item_node->type_name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, func_type_no_param_no_return)
{
    char test_code[] = "fun print ()->None";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(0, array_size(&node->ft->params->block->nodes));
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_STREQ("print", string_get(node->ft->name));
    ASSERT_EQ(BuiltinType, node->ft->ret_type_item_node->type_item_node->kind);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, import_fun_type)
{
    char test_code[] = "from sys import fun print () -> None";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    ASSERT_STREQ("sys", string_get(node->import->from_module));
    node = node->import->import;
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    ASSERT_EQ(0, array_size(&node->ft->params->block->nodes));
    ASSERT_STREQ("print", string_get(node->ft->name));
    ASSERT_EQ(BuiltinType, node->ft->ret_type_item_node->type_item_node->kind);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, import_memory_init)
{
    char test_code[] = "from sys import memory 2";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    node = node->import->import;
    ASSERT_EQ(MEMORY_NODE, node->node_type);
    ASSERT_EQ(2, node->memory->initial->liter->int_val);
    ASSERT_EQ(0, node->memory->max);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, import_memory_init_max)
{
    char test_code[] = "from sys import memory 2, 10";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    node = node->import->import;
    ASSERT_EQ(MEMORY_NODE, node->node_type);
    ASSERT_EQ(2, node->memory->initial->liter->int_val);
    ASSERT_EQ(10, node->memory->max->liter->int_val);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, import_global)
{
    char test_code[] = "from sys import __stack_pointer:int";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(IMPORT_NODE, node->node_type);
    node = node->import->import;
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("__stack_pointer", string_get(node->var->var->ident->name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, ref_type)
{
    char test_code[] = "let ri:&int";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_STREQ("int", string_get(node->var->is_of_type->type_item_node->val_node->type_name));
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, create_ref_variable)
{
    char test_code[] = "let ri = &i";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(UNARY_NODE, node->var->init_value->node_type);
    ASSERT_EQ(OP_BAND, node->var->init_value->unop->opcode);
    node_free(block);
    
    frontend_deinit(fe);
}

TEST(test_parser, create_deref_variable)
{
    char test_code[] = "let ri = *i";
    struct frontend *fe = frontend_init();
    
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(UNARY_NODE, node->var->init_value->node_type);
    ASSERT_EQ(OP_STAR, node->var->init_value->unop->opcode);
    node_free(block);
     
    frontend_deinit(fe);
}


TEST(test_parser, array_variable)
{
    char test_code[] = "let a = [10,20]";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(ARRAY_INIT_NODE, node->var->init_value->node_type);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser, onebytwo_array_variable)
{
    char test_code[] = "let a = [[10,20]]";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(ARRAY_INIT_NODE, node->var->init_value->node_type);
    ASSERT_EQ(1, array_size(&node->var->init_value->array_init->block->nodes));
    struct ast_node *row_node = array_front_ptr(&node->var->init_value->array_init->block->nodes);
    ASSERT_EQ(ARRAY_INIT_NODE, row_node->node_type);
    ASSERT_EQ(2, array_size(&row_node->array_init->block->nodes));
    struct ast_node *cell_node = array_front_ptr(&row_node->array_init->block->nodes);
    ASSERT_EQ(LITERAL_NODE, cell_node->node_type);
    ASSERT_EQ(10, cell_node->liter->int_val);
    cell_node = array_back_ptr(&row_node->array_init->block->nodes);
    ASSERT_EQ(LITERAL_NODE, cell_node->node_type);
    ASSERT_EQ(20, cell_node->liter->int_val);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser, twobytwo_array_variable)
{
    char test_code[] = "let a = [[10,20], [30, 40]]";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(1, array_size(&block->block->nodes));
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(ARRAY_INIT_NODE, node->var->init_value->node_type);
    ASSERT_EQ(2, array_size(&node->var->init_value->array_init->block->nodes));
    //first row
    struct ast_node *row_node = array_front_ptr(&node->var->init_value->array_init->block->nodes);
    ASSERT_EQ(ARRAY_INIT_NODE, row_node->node_type);
    ASSERT_EQ(2, array_size(&row_node->array_init->block->nodes));
    struct ast_node *cell_node = array_front_ptr(&row_node->array_init->block->nodes);
    ASSERT_EQ(LITERAL_NODE, cell_node->node_type);
    ASSERT_EQ(10, cell_node->liter->int_val);
    cell_node = array_back_ptr(&row_node->array_init->block->nodes);
    ASSERT_EQ(LITERAL_NODE, cell_node->node_type);
    ASSERT_EQ(20, cell_node->liter->int_val);

    //second row
    row_node = array_back_ptr(&node->var->init_value->array_init->block->nodes);
    ASSERT_EQ(ARRAY_INIT_NODE, row_node->node_type);
    ASSERT_EQ(2, array_size(&row_node->array_init->block->nodes));
    cell_node = array_front_ptr(&row_node->array_init->block->nodes);
    ASSERT_EQ(LITERAL_NODE, cell_node->node_type);
    ASSERT_EQ(30, cell_node->liter->int_val);
    cell_node = array_back_ptr(&row_node->array_init->block->nodes);
    ASSERT_EQ(LITERAL_NODE, cell_node->node_type);
    ASSERT_EQ(40, cell_node->liter->int_val);
    node_free(block);
    frontend_deinit(fe);
}

TEST(test_parser, multiple_statements_on_one_line)
{
    char test_code[] = "let a = 10; let b = 20";
    struct frontend *fe = frontend_init();
    struct ast_node *block = parse_code(fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(to_symbol("a"), node->var->var->ident->name);

    node = array_back_ptr(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    ASSERT_EQ(to_symbol("b"), node->var->var->ident->name);
    node_free(block);
    frontend_deinit(fe);
}

int test_parser()
{
    UNITY_BEGIN();
    RUN_TEST(test_parser_int_type);
    RUN_TEST(test_parser_var_init);
    RUN_TEST(test_parser_mut_var_init);
    RUN_TEST(test_parser_immut_var_init);
    RUN_TEST(test_parser_var_init_name_with_underlying);
    RUN_TEST(test_parser_int_init);
    RUN_TEST(test_parser_bool_init);
    RUN_TEST(test_parser_char_init);
    RUN_TEST(test_parser_string_init);
    RUN_TEST(test_parser_id_func);
    RUN_TEST(test_parser_id_func_with_return);
    RUN_TEST(test_parser_binary_exp_func);
    RUN_TEST(test_parser_func_with_new_line);
    RUN_TEST(test_parser_distance_function);
    RUN_TEST(test_parser_for_loop);
    RUN_TEST(test_parser_for_loop_with_step);
    RUN_TEST(test_parser_avg_function);
    RUN_TEST(test_parser_if_condition);
    RUN_TEST(test_parser_if_condition_one_line);
    RUN_TEST(test_parser_if_condition_one_block);
    RUN_TEST(test_parser_if_condition_two_blocks);
    RUN_TEST(test_parser_if_condition_three_blocks);
    RUN_TEST(test_parser_if_condition_no_else);
    RUN_TEST(test_parser_pattern_match);
    RUN_TEST(test_parser_pattern_match_variable);
    RUN_TEST(test_parser_local_string_function);
    RUN_TEST(test_parser_variadic_function);
    RUN_TEST(test_parser_func_type);
    RUN_TEST(test_parser_func_type_no_param);
    RUN_TEST(test_parser_func_type_no_param_no_return);
    RUN_TEST(test_parser_import_fun_type);
    RUN_TEST(test_parser_import_memory_init);
    RUN_TEST(test_parser_import_memory_init_max);
    RUN_TEST(test_parser_import_global);
    RUN_TEST(test_parser_ref_type);
    RUN_TEST(test_parser_create_ref_variable);
    RUN_TEST(test_parser_create_deref_variable);
    RUN_TEST(test_parser_array_variable);
    RUN_TEST(test_parser_onebytwo_array_variable);
    RUN_TEST(test_parser_twobytwo_array_variable);
    RUN_TEST(test_parser_multiple_statements_on_one_line);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
