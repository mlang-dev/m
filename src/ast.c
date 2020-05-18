/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language ast c code implementation. 
 * 
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

bool is_unary_op(struct prototype_node* node)
{
    return node->is_operator && array_size(&node->fun_params) == UNARY_PARAM_SIZE;
}

bool is_binary_op(struct prototype_node* node)
{
    return node->is_operator && array_size(&node->fun_params) == BINARY_PARAM_SIZE;
}

char get_op_name(struct prototype_node* node)
{
    assert(is_unary_op(node) || is_binary_op(node));
    return string_back(&node->name);
}

struct function_node* create_function_node(struct prototype_node* prototype,
    struct block_node* body)
{
    struct function_node* node = malloc(sizeof(*node));
    node->base.type = 0;
    node->base.annotated_type = 0;
    node->base.node_type = FUNCTION_NODE;
    node->base.parent = (struct exp_node*)prototype;
    node->base.loc = prototype->base.loc;
    node->prototype = prototype;
    node->body = body;
    return node;
}

struct type_exp* get_ret_type(struct function_node* fun_node)
{
    struct type_oper* oper = (struct type_oper*)fun_node->base.type;
    return *(struct type_exp**)array_back(&oper->args);
}

struct ident_node* create_ident_node(struct exp_node* parent, struct source_loc loc, const char* name)
{
    struct ident_node* node = malloc(sizeof(*node));
    node->base.type = 0;
    node->base.annotated_type = 0;
    node->base.node_type = IDENT_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    return node;
}


struct literal_node* _create_literal_node(struct exp_node* parent, struct source_loc loc, void* val, enum type type)
{
    struct literal_node* node = malloc(sizeof(*node));
    node->base.node_type = LITERAL_NODE;
    node->base.annotated_type = (struct type_exp*)create_nullary_type(type);
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    if (type == TYPE_INT)
        node->int_val = *(int*)val;
    else if(type == TYPE_DOUBLE)
        node->double_val = *(double*)val;
    else if(type == TYPE_CHAR)
        node->char_val = *(char*)val;
    else if(type == TYPE_BOOL)
        node->bool_val = *(bool*)val;
    else if(type == TYPE_STRING)
        node->str_val = strdup((const char*)val);
    return node;
}

struct literal_node* create_double_node(struct exp_node* parent, struct source_loc loc, double val)
{
    return _create_literal_node(parent, loc, &val, TYPE_DOUBLE);
}

struct literal_node* create_int_node(struct exp_node* parent, struct source_loc loc, int val)
{
    return _create_literal_node(parent, loc, &val, TYPE_INT);
}

struct literal_node* create_bool_node(struct exp_node* parent, struct source_loc loc, bool val)
{
    return _create_literal_node(parent, loc, &val, TYPE_BOOL);
}

struct literal_node* create_char_node(struct exp_node* parent, struct source_loc loc, char val)
{
    return _create_literal_node(parent, loc, &val, TYPE_CHAR);
}

struct literal_node* create_string_node(struct exp_node* parent, struct source_loc loc, const char* val)
{
    return _create_literal_node(parent, loc, (void*)val, TYPE_STRING);
}

struct var_node* create_var_node(struct exp_node* parent, struct source_loc loc, const char* var_name, struct exp_node* init_value)
{
    struct var_node* node = (struct var_node*)malloc(sizeof(*node));
    node->base.node_type = VAR_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->var_name, var_name);
    node->init_value = init_value;
    return node;
}

struct call_node* create_call_node(struct exp_node* parent, struct source_loc loc, const char* callee,
    struct array* args)
{
    struct call_node* node = malloc(sizeof(*node));
    node->base.node_type = CALL_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->callee, callee);
    array_copy(&node->args, args);
    return node;
}

struct prototype_node* create_prototype_node_default(struct exp_node* parent, struct source_loc loc, const char* name, struct array* args, struct type_exp* ret_type, bool is_variadic)
{
    return create_prototype_node(parent, loc, name, args, ret_type, false, 0, "", is_variadic);
}

struct prototype_node* create_prototype_node(struct exp_node* parent, struct source_loc loc, const char* name,
    struct array* args,
    struct type_exp* ret_type,
    bool is_operator, unsigned precedence, const char* op,
    bool is_variadic)
{
    struct prototype_node* node = malloc(sizeof(*node));
    node->base.node_type = PROTOTYPE_NODE;
    node->base.annotated_type = ret_type;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    node->fun_params = *args;
    node->is_operator = is_operator;
    node->precedence = precedence;
    node->is_variadic = is_variadic;
    string_init_chars(&node->op, op);
    return node;
}

struct condition_node* create_if_node(struct exp_node* parent, struct source_loc loc, struct exp_node* condition, struct exp_node* then_node,
    struct exp_node* else_node)
{
    struct condition_node* node = malloc(sizeof(*node));
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

struct unary_node* create_unary_node(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* operand)
{
    struct unary_node* node = malloc(sizeof(*node));
    node->base.node_type = UNARY_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->op, op);
    node->operand = operand;
    return node;
}

struct binary_node* create_binary_node(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* lhs, struct exp_node* rhs)
{
    struct binary_node* node = malloc(sizeof(*node));
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

struct for_node* create_for_node(struct exp_node* parent, struct source_loc loc, const char* var_name, struct exp_node* start,
    struct exp_node* end, struct exp_node* step, struct exp_node* body)
{
    struct for_node* node = malloc(sizeof(*node));
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

struct block_node* create_block_node(struct exp_node* parent, struct array* nodes)
{
    struct block_node* node = malloc(sizeof(*node));
    node->base.node_type = BLOCK_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = (*(struct exp_node**)array_front(nodes))->loc;
    array_copy(&node->nodes, nodes);
    return node;
}

struct module* create_module(const char* mod_name, FILE* file)
{
    struct module* mod = malloc(sizeof(*mod));
    string_init_chars(&mod->name, mod_name);
    struct block_node* node = malloc(sizeof(*node));
    mod->block = node;
    array_init(&mod->block->nodes, sizeof(struct exp_node*));
    mod->tokenizer = create_tokenizer(file, mod_name);
    return mod;
}
