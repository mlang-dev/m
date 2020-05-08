/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language ast c code implementation. 
 * 
 */
#include <stdlib.h>
#include <assert.h>

#include "ast.h"


bool is_unary_op(prototype_node* pnode)
{
    return pnode->is_operator && array_size(&pnode->args) == 1;
}

bool is_binary_op(prototype_node* pnode)
{
    return pnode->is_operator && array_size(&pnode->args) == 2;
}

char get_op_name(prototype_node* pnode)
{
    assert(is_unary_op(pnode) || is_binary_op(pnode));
    return string_back(&pnode->name);
}

function_node* create_function_node(prototype_node* prototype,
    block_node* body)
{
    function_node* node = (function_node*)malloc(sizeof(function_node));
    node->base.type = 0;
    node->base.annotated_type = 0;
    node->base.node_type = FUNCTION_NODE;
    node->base.parent = (exp_node*)prototype;
    node->base.loc = prototype->base.loc;
    node->prototype = prototype;
    node->body = body;
    return node;
}

ident_node* create_ident_node(exp_node* parent, source_loc loc, const char *name)
{
    ident_node* node = (ident_node*)malloc(sizeof(ident_node));
    node->base.type = 0;
    node->base.annotated_type = 0;
    node->base.node_type = IDENT_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    return node;
}

num_node* create_double_node(exp_node* parent, source_loc loc, double val)
{
    num_node* node = (num_node*)malloc(sizeof(num_node));
    node->base.node_type = NUMBER_NODE;
    node->base.annotated_type = TYPE_DOUBLE;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    node->double_val = val;
    return node;
}

num_node* _create_int_node(exp_node* parent, source_loc loc, int val, Type type)
{
    num_node* node = (num_node*)malloc(sizeof(num_node));
    node->base.node_type = NUMBER_NODE;
    node->base.annotated_type = type;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    node->double_val = val;
    return node;
}

num_node* create_int_node(exp_node* parent, source_loc loc, int val)
{
    return _create_int_node(parent, loc, val, TYPE_INT);
}

num_node* create_bool_node(exp_node* parent, source_loc loc, int val)
{
    return _create_int_node(parent, loc, val, TYPE_BOOL);
}

var_node* create_var_node(exp_node* parent, source_loc loc, const char *var_name, exp_node* init_value)
{
    var_node* node = (var_node*)malloc(sizeof(var_node));
    node->base.node_type = VAR_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->var_name, var_name);
    node->init_value = init_value;
    return node;
}

call_node* create_call_node(exp_node* parent, source_loc loc, const char *callee,
    array* args)
{
    call_node* node = (call_node*)malloc(sizeof(call_node));
    node->base.node_type = CALL_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->callee, callee);
    array_copy(&node->args, args);
    return node;
}

prototype_node* create_prototype_node_default(exp_node* parent, source_loc loc, const char *name, array* args)
{
    return create_prototype_node(parent, loc, name, args, false, 0, "");
}

prototype_node* create_prototype_node(exp_node* parent, source_loc loc, const char *name,
    array* args,
    bool is_operator, unsigned precedence, const char *op)
{
    prototype_node* node = (prototype_node*)malloc(sizeof(prototype_node));
    node->base.node_type = PROTOTYPE_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    node->args = *args;
    node->is_operator = is_operator;
    node->precedence = precedence;
    string_init_chars(&node->op, op);
    return node;
}

condition_node* create_if_node(exp_node* parent, source_loc loc, exp_node* condition, exp_node* then_node,
    exp_node* else_node)
{
    condition_node* node = (condition_node*)malloc(sizeof(condition_node));
    node->base.node_type = CONDITION_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    node->condition_node = condition;
    node->then_node = then_node;
    node->else_node = else_node;
    return node;
}

unary_node* create_unary_node(exp_node* parent, source_loc loc, const char *op, exp_node* operand)
{
    unary_node* node = (unary_node*)malloc(sizeof(unary_node));
    node->base.node_type = UNARY_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->op, op);
    node->operand = operand;
    return node;
}

binary_node* create_binary_node(exp_node* parent, source_loc loc, const char *op, exp_node* lhs, exp_node* rhs)
{
    binary_node* node = (binary_node*)malloc(sizeof(binary_node));
    node->base.node_type = BINARY_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->op, op);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

for_node* create_for_node(exp_node* parent, source_loc loc, const char *var_name, exp_node* start,
    exp_node* end, exp_node* step, exp_node* body)
{
    for_node* node = (for_node*)malloc(sizeof(for_node));
    node->base.node_type = FOR_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->var_name, var_name);
    node->start = start;
    node->end = end;
    node->step = step;
    node->body = body;
    return node;
}

block_node* create_block_node(exp_node* parent, array *nodes)
{
    block_node* node = (block_node*)malloc(sizeof(block_node));
    node->base.node_type = BLOCK_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = (*(exp_node**)array_front(nodes))->loc;
    array_copy(&node->nodes, nodes);
    return node;
}

module* create_module(const char* mod_name, FILE* file)
{
    module* mod = (module*)malloc(sizeof(module));
    string_init_chars(&mod->name, mod_name);
    mod->block = (block_node*)malloc(sizeof(block_node));
    array_init(&mod->block->nodes, sizeof(exp_node*));
    mod->tokenizer = create_tokenizer(file);
    return mod;
}
