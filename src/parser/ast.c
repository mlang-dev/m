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

void _free_exp_node(struct exp_node* node)
{
    if(node->annotated_type)
        type_exp_free(node->annotated_type);
    if(node->annotation){
        string_free(node->annotation);
    }
    /*TODO: free node->type, need to be considerate for shared types*/
    free(node);
}

struct block_node* _copy_block_node(struct block_node* orig_node)
{
    struct array nodes;
    array_init(&nodes, sizeof(struct exp_node*));
    for(size_t i = 0; i < array_size(&orig_node->nodes); i++){
        struct exp_node* node = node_copy(*(struct exp_node**)array_get(&orig_node->nodes, i));
        //printf("block node copy: %s\n", node_type_strings[node->node_type]);
        array_push(&nodes, &node);
    }
    return block_node_new(orig_node->base.parent, &nodes);
}

void _free_exp_nodes(struct array* nodes)
{
    for(size_t i = 0; i < array_size(nodes); i++){
        struct exp_node* elem = *(struct exp_node**)array_get(nodes, i);
        node_free(elem);
    }
    array_deinit(nodes);
}

void _free_block_node(struct block_node* node)
{
    _free_exp_nodes(&node->nodes);
    _free_exp_node(&node->base);
}

struct ident_node* ident_node_new(struct exp_node* parent, struct source_loc loc, const char* name)
{
    struct ident_node* node = malloc(sizeof(*node));
    node->base.type = 0;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
    node->base.node_type = IDENT_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    node->member_accessors = string_split(&node->name, '.');    
    return node;
}

struct ident_node* _copy_ident_node(struct ident_node* orig_node)
{
    return ident_node_new(orig_node->base.parent, orig_node->base.loc, 
    string_get(&orig_node->name));
}

void _free_ident_node(struct ident_node* node)
{
    string_deinit(&node->name);
    array_deinit(&node->member_accessors);
    _free_exp_node(&node->base);
}

struct literal_node* _create_literal_node(struct exp_node* parent, struct source_loc loc, void* val, enum type type)
{
    struct literal_node* node = malloc(sizeof(*node));
    node->base.node_type = LITERAL_NODE;
    node->base.annotated_type = (struct type_exp*)create_nullary_type(type);
    node->base.annotation = string_new(type_strings[type]);
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

void _free_literal_node(struct literal_node* node)
{
    if(node->base.annotated_type->type == TYPE_STRING)
        free((void*)node->str_val);
    _free_exp_node(&node->base);
}

struct literal_node* double_node_new(struct exp_node* parent, struct source_loc loc, double val)
{
    return _create_literal_node(parent, loc, &val, TYPE_DOUBLE);
}

struct literal_node* int_node_new(struct exp_node* parent, struct source_loc loc, int val)
{
    return _create_literal_node(parent, loc, &val, TYPE_INT);
}

struct literal_node* bool_node_new(struct exp_node* parent, struct source_loc loc, bool val)
{
    return _create_literal_node(parent, loc, &val, TYPE_BOOL);
}

struct literal_node* char_node_new(struct exp_node* parent, struct source_loc loc, char val)
{
    return _create_literal_node(parent, loc, &val, TYPE_CHAR);
}

struct literal_node* unit_node_new(struct exp_node* parent, struct source_loc loc)
{
    return _create_literal_node(parent, loc, 0, TYPE_UNIT);
}

struct literal_node* string_node_new(struct exp_node* parent, struct source_loc loc, const char* val)
{
    return _create_literal_node(parent, loc, (void*)val, TYPE_STRING);
}

struct literal_node* _copy_literal_node(struct literal_node* orig_node)
{
    return _create_literal_node(orig_node->base.parent, orig_node->base.loc, &orig_node->char_val, 
    orig_node->base.annotated_type->type);
}

struct var_node* var_node_new(struct exp_node* parent, struct source_loc loc, const char* var_name, enum type type, string* ext_type,
 struct exp_node* init_value)
{
    (void)ext_type;
    struct var_node* node = (struct var_node*)malloc(sizeof(*node));
    node->base.node_type = VAR_NODE;
    node->base.annotated_type = type? (struct type_exp*)create_nullary_type(type) : 0;
    node->base.annotation = ext_type ? string_new(string_get(ext_type)) : 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->var_name, var_name);
    node->init_value = init_value;
    return node;
}

struct var_node* _copy_var_node(struct var_node* orig_node)
{
    return var_node_new(orig_node->base.parent, orig_node->base.loc, 
    string_get(&orig_node->var_name), orig_node->base.type ? orig_node->base.type->type : TYPE_UNK, 0,
    node_copy(orig_node->init_value));
}

void _free_var_node(struct var_node* node)
{
    string_deinit(&node->var_name);
    if(node->init_value)
        node_free(node->init_value);
    _free_exp_node(&node->base);
}


struct type_node* type_node_new(struct exp_node* parent, struct source_loc loc, string name, struct block_node* body)
{
    struct type_node* node = malloc(sizeof(*node));
    node->base.node_type = TYPE_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    node->name = name;
    node->body = body;
    return node;
}

struct type_node* _copy_type_node(struct type_node* orig_node)
{
    string name;
    string_copy(&name, &orig_node->name);
    return type_node_new(orig_node->base.parent, orig_node->base.loc, 
        name, _copy_block_node(orig_node->body));
}

void _free_type_node(struct type_node* node)
{
    string_deinit(&node->name);
    _free_block_node(node->body);
    _free_exp_node(&node->base);
}

struct type_value_node* type_value_node_new(struct exp_node* parent, struct source_loc loc, struct block_node* body)
{
    struct type_value_node* node = malloc(sizeof(*node));
    node->base.node_type = TYPE_VALUE_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    node->body = body;
    return node;
}

struct type_value_node* _copy_type_value_node(struct type_value_node* orig_node)
{
    return type_value_node_new(orig_node->base.parent, orig_node->base.loc, 
        _copy_block_node(orig_node->body));
}

void _free_type_value_node(struct type_value_node* node)
{
    _free_block_node(node->body);
    _free_exp_node(&node->base);
}

struct call_node* call_node_new(struct exp_node* parent, struct source_loc loc, const char* callee,
    struct array* args)
{
    struct call_node* node = malloc(sizeof(*node));
    node->base.node_type = CALL_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
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
    return call_node_new(orig_node->base.parent, orig_node->base.loc, string_get(&orig_node->callee), 
    &orig_node->args);
}

void _free_call_node(struct call_node* node)
{
    string_deinit(&node->callee);
    string_deinit(&node->specialized_callee);
    _free_exp_nodes(&node->args);
    _free_exp_node(&node->base);
}

struct prototype_node* prototype_node_default_new(struct exp_node* parent, struct source_loc loc, const char* name, struct array* args, 
struct type_exp* ret_type, bool is_variadic, bool is_external)
{
    return prototype_node_new(parent, loc, name, args, ret_type, false, 0, "", is_variadic, is_external);
}

struct prototype_node* prototype_node_new(struct exp_node* parent, struct source_loc loc, const char* name,
    struct array* args,
    struct type_exp* ret_type,
    bool is_operator, unsigned precedence, const char* op,
    bool is_variadic, bool is_external)
{
    struct prototype_node* node = malloc(sizeof(*node));
    node->base.node_type = PROTOTYPE_NODE;
    node->base.annotated_type = ret_type;
    node->base.annotation = ret_type? string_new(type_strings[ret_type->type]) : 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    node->fun_params = *args;
    node->is_operator = is_operator;
    node->precedence = precedence;
    node->is_variadic = is_variadic;
    node->is_extern = is_external;
    string_init_chars(&node->op, op);
    struct var_node fun_param;
    if(is_variadic){
        string_init_chars(&fun_param.var_name, type_strings[TYPE_GENERIC]);
        fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_GENERIC);
        fun_param.base.annotation = string_new(type_strings[TYPE_GENERIC]);
        fun_param.base.type = fun_param.base.annotated_type;
        array_push(&node->fun_params, &fun_param);
    }
    return node;
}

struct prototype_node* _copy_prototype_node(struct prototype_node* proto)
{
    struct prototype_node* node = malloc(sizeof(*node));
    node->base.node_type = PROTOTYPE_NODE;
    node->base.annotated_type = proto->base.annotated_type;
    node->base.annotation = proto->base.annotation ? string_new(string_get(proto->base.annotation)) : 0;
    node->base.type = 0;
    node->base.parent = proto->base.parent;
    node->base.loc = proto->base.loc;
    string_init_chars(&node->name, string_get(&proto->name));
    node->fun_params = proto->fun_params;
    node->is_operator = proto->is_operator;
    node->precedence = proto->precedence;
    node->is_variadic = proto->is_variadic;
    node->is_extern = proto->is_extern;
    string_init_chars(&node->op, string_get(&proto->op));
    struct var_node fun_param;
    fun_param.base.type = 0;
    fun_param.base.node_type = VAR_NODE;
    if(proto->is_variadic){
        string_init_chars(&fun_param.var_name, type_strings[TYPE_GENERIC]);
        fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_GENERIC);
        fun_param.base.annotation = string_new(type_strings[TYPE_GENERIC]);
        array_push(&node->fun_params, &fun_param);
    }
    return node;
}

void _free_prototype_node(struct prototype_node* node)
{
    string_deinit(&node->name);
    string_deinit(&node->op);
    /*fun_params will be freed in array_deinit*/
    array_deinit(&node->fun_params);
    _free_exp_node(&node->base);
}

struct function_node* function_node_new(struct prototype_node* prototype,
    struct block_node* body)
{
    struct function_node* node = malloc(sizeof(*node));
    node->base.type = 0;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
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
    return function_node_new(proto, block);
}

void _free_function_node(struct function_node* node)
{
    _free_prototype_node(node->prototype);
    _free_block_node(node->body);
    _free_exp_node(&node->base);
}

struct condition_node* if_node_new(struct exp_node* parent, struct source_loc loc, 
    struct exp_node* condition, struct exp_node* then_node, struct exp_node* else_node)
{
    struct condition_node* node = malloc(sizeof(*node));
    node->base.node_type = CONDITION_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
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
    return if_node_new(orig_node->base.parent, orig_node->base.loc, orig_node->condition_node, 
    orig_node->then_node, orig_node->else_node);
}

void _free_if_node(struct condition_node* node)
{
    if(node->condition_node)
        node_free(node->condition_node);
    if(node->then_node)
        node_free(node->then_node);
    if(node->else_node)
        node_free(node->else_node);
    _free_exp_node(&node->base);
}

struct unary_node* unary_node_new(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* operand)
{
    struct unary_node* node = malloc(sizeof(*node));
    node->base.node_type = UNARY_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->op, op);
    node->operand = operand;
    return node;
}

struct unary_node* _copy_unary_node(struct unary_node* orig_node){
    return unary_node_new(orig_node->base.parent, orig_node->base.loc, string_get(&orig_node->op),
    orig_node->operand);
}

void _free_unary_node(struct unary_node* node){
    string_deinit(&node->op);
    node_free(node->operand);
    _free_exp_node(&node->base);
}

struct binary_node* binary_node_new(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* lhs, struct exp_node* rhs)
{
    struct binary_node* node = malloc(sizeof(*node));
    node->base.node_type = BINARY_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
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
    return binary_node_new(orig_node->base.parent, orig_node->base.loc, string_get(&orig_node->op),
    orig_node->lhs, orig_node->rhs);
}

void _free_binary_node(struct binary_node* node)
{
    string_deinit(&node->op);
    if(node->lhs)
        node_free(node->lhs);
    if(node->rhs)
        node_free(node->rhs);
    _free_exp_node(&node->base);
}

struct for_node* for_node_new(struct exp_node* parent, struct source_loc loc, const char* var_name, struct exp_node* start,
    struct exp_node* end, struct exp_node* step, struct exp_node* body)
{
    struct for_node* node = malloc(sizeof(*node));
    node->base.node_type = FOR_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
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
    return for_node_new(orig_node->base.parent, orig_node->base.loc, 
    string_get(&orig_node->var_name), orig_node->start, orig_node->end, orig_node->step, orig_node->body);
}

void _free_for_node(struct for_node* node)
{
    string_deinit(&node->var_name);
    if(node->start)
        node_free(node->start);
    if(node->end)
        node_free(node->end);
    if(node->step)
        node_free(node->step);
    if(node->body)
        node_free(node->body);
    _free_exp_node(&node->base);
}

struct block_node* block_node_new(struct exp_node* parent, struct array* nodes)
{
    struct block_node* node = malloc(sizeof(*node));
    node->base.node_type = BLOCK_NODE;
    node->base.annotated_type = 0;
    node->base.annotation = 0;
    node->base.type = 0;
    node->base.parent = parent;
    node->base.loc = (*(struct exp_node**)array_front(nodes))->loc;
    node->nodes = *nodes;
    return node;
}

struct exp_node* node_copy(struct exp_node* node)
{
    switch(node->node_type){
        case BLOCK_NODE:
            return (struct exp_node*)_copy_block_node((struct block_node*)node);
        case PROTOTYPE_NODE:
            return (struct exp_node*)_copy_prototype_node((struct prototype_node*)node);
        case FUNCTION_NODE:
            return (struct exp_node*)_copy_function_node((struct function_node*)node);
        case VAR_NODE:
            return (struct exp_node*)_copy_var_node((struct var_node*)node);
        case TYPE_NODE:
            return (struct exp_node*)_copy_type_node((struct type_node*)node);
        case TYPE_VALUE_NODE:
            return (struct exp_node*)_copy_type_value_node((struct type_value_node*)node);
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

void node_free(struct exp_node* node)
{
    switch(node->node_type){
        case UNK_NODE:
            free(node);
            break;
        case BLOCK_NODE:
            _free_block_node((struct block_node*)node);
            break;
        case PROTOTYPE_NODE:
            _free_prototype_node((struct prototype_node*)node);
            break;
        case FUNCTION_NODE:
            _free_function_node((struct function_node*)node);
            break;
        case VAR_NODE:
            _free_var_node((struct var_node*)node);
            break;
        case TYPE_NODE:
            _free_type_node((struct type_node*)node);
            break;
        case TYPE_VALUE_NODE:
            _free_type_value_node((struct type_value_node*)node);
            break;
        case IDENT_NODE:
            _free_ident_node((struct ident_node*)node);
            break;
        case LITERAL_NODE:
            _free_literal_node((struct literal_node*)node);
            break;
        case CALL_NODE:
            _free_call_node((struct call_node*)node);
            break;
        case CONDITION_NODE:
            _free_if_node((struct condition_node*)node);
            break;
        case FOR_NODE:
            _free_for_node((struct for_node*)node);
            break;
        case UNARY_NODE:
            _free_unary_node((struct unary_node*)node);
            break;
        case BINARY_NODE:
            _free_binary_node((struct binary_node*)node);
            break;
        default:
            printf("not supported node type: %d\n", node->node_type);
            assert(false);
    }
}

struct module* module_new(const char* mod_name, FILE* file)
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

int find_member_index(struct type_node* type_node, const char* member)
{
    for(size_t i = 0; i < array_size(&type_node->body->nodes); i++){
        struct var_node* var = *(struct var_node**)array_get(&type_node->body->nodes, i);
        if(string_eq_chars(&var->var_name, member)){
            return i;
        }
    }
    return -1;
}