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

const char* node_type_strings[] = {
    FOREACH_NODETYPE(GENERATE_ENUM_STRING)
};

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

struct type_exp* get_ret_type(struct function_node* fun_node)
{
    struct type_oper* oper = (struct type_oper*)fun_node->base.type;
    return *(struct type_exp**)array_back(&oper->args);
}

struct block_node* _copy_block_node(struct block_node* orig_node)
{
    struct array nodes;
    array_init(&nodes, sizeof(struct exp_node*));
    for(size_t i = 0; i < array_size(&orig_node->nodes); i++){
        struct exp_node* node = copy_node(*(struct exp_node**)array_get(&orig_node->nodes, i));
        //printf("block node copy: %s\n", node_type_strings[node->node_type]);
        array_push(&nodes, &node);
    }
    return create_block_node(orig_node->base.parent, &nodes);
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

struct ident_node* _copy_ident_node(struct ident_node* orig_node)
{
    return create_ident_node(orig_node->base.parent, orig_node->base.loc, 
    string_get(&orig_node->name));
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

struct literal_node* _copy_literal_node(struct literal_node* orig_node)
{
    return _create_literal_node(orig_node->base.parent, orig_node->base.loc, &orig_node->char_val, 
    orig_node->base.annotated_type->type);
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
    string_init(&node->specialized_callee);
    return node;
}

struct call_node* _copy_call_node(struct call_node* orig_node)
{
    return create_call_node(orig_node->base.parent, orig_node->base.loc, string_get(&orig_node->callee), 
    &orig_node->args);
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
    struct var_node fun_param;
    if(is_variadic){
        string_init_chars(&fun_param.var_name, type_strings[TYPE_GENERIC]);
        fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_GENERIC);
        array_push(&node->fun_params, &fun_param);
    }
    return node;
}

struct prototype_node* _copy_prototype_node(struct prototype_node* proto)
{
    struct prototype_node* node = malloc(sizeof(*node));
    node->base.node_type = PROTOTYPE_NODE;
    node->base.annotated_type = proto->base.annotated_type;
    node->base.type = 0;
    node->base.parent = proto->base.parent;
    node->base.loc = proto->base.loc;
    string_init_chars(&node->name, string_get(&proto->name));
    node->fun_params = proto->fun_params;
    node->is_operator = proto->is_operator;
    node->precedence = proto->precedence;
    node->is_variadic = proto->is_variadic;
    string_init_chars(&node->op, string_get(&proto->op));
    struct var_node fun_param;
    if(proto->is_variadic){
        string_init_chars(&fun_param.var_name, type_strings[TYPE_GENERIC]);
        fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_GENERIC);
        array_push(&node->fun_params, &fun_param);
    }
    return node;
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

struct function_node* _copy_function_node(struct function_node* orig_node)
{
    struct prototype_node* proto = _copy_prototype_node(orig_node->prototype);
    struct block_node* block = _copy_block_node(orig_node->body);
    return create_function_node(proto, block);
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

struct condition_node* _copy_if_node(struct condition_node* orig_node)
{
    return create_if_node(orig_node->base.parent, orig_node->base.loc, orig_node->condition_node, 
    orig_node->then_node, orig_node->else_node);
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

struct unary_node* _copy_unary_node(struct unary_node* orig_node){
    return create_unary_node(orig_node->base.parent, orig_node->base.loc, string_get(&orig_node->op),
    orig_node->operand);
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

struct binary_node* _copy_binary_node(struct binary_node* orig_node)
{
    return create_binary_node(orig_node->base.parent, orig_node->base.loc, string_get(&orig_node->op),
    orig_node->lhs, orig_node->rhs);
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

struct for_node* _copy_for_node(struct for_node* orig_node)
{
    return create_for_node(orig_node->base.parent, orig_node->base.loc, 
    string_get(&orig_node->var_name), orig_node->start, orig_node->end, orig_node->step, orig_node->body);
}

struct block_node* create_block_node(struct exp_node* parent, struct array* nodes)
{
    struct block_node* node = malloc(sizeof(*node));
    node->base.node_type = BLOCK_NODE;
    node->base.annotated_type = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = (*(struct exp_node**)array_front(nodes))->loc;
    node->nodes = *nodes;
    return node;
}

struct exp_node* copy_node(struct exp_node* node)
{
    switch(node->node_type){
        case BLOCK_NODE:
            return (struct exp_node*)_copy_block_node((struct block_node*)node);
        case PROTOTYPE_NODE:
            return (struct exp_node*)_copy_prototype_node((struct prototype_node*)node);
        case FUNCTION_NODE:
            return (struct exp_node*)_copy_function_node((struct function_node*)node);
        case IDENT_NODE:
            return (struct exp_node*)_copy_ident_node((struct ident_node*)node);
        case LITERAL_NODE:
            return (struct exp_node*)_copy_literal_node((struct literal_node*)node);
        case CALL_NODE:
            return (struct exp_node*)_copy_call_node((struct call_node*)node);
        case CONDITION_NODE:
            return (struct exp_node*)_copy_if_node((struct condition_node*)node);
        case FOR_NODE:
            return (struct exp_node*)_copy_for_node((struct for_node*)node);
        case UNARY_NODE:
            return (struct exp_node*)_copy_unary_node((struct unary_node*)node);
        case BINARY_NODE:
            return (struct exp_node*)_copy_binary_node((struct binary_node*)node);
        default:
            assert(false);
    }
    return 0;
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

bool is_recursive(struct call_node* call)
{
    struct exp_node* parent = call->base.parent;
    string* fun_name = 0;
    while(parent){
        if(parent->node_type == PROTOTYPE_NODE)
            fun_name = &((struct prototype_node*)parent)->name;
        else if(parent->node_type == FUNCTION_NODE)
            fun_name = &((struct function_node*)parent)->prototype->name;
        if(fun_name && string_eq(fun_name, &call->callee))
            return true;
        parent = parent->parent;
    }
    return false;
}