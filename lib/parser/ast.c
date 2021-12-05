/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language ast c code implementation.
 *
 */
#include "parser/ast.h"
#include "clib/array.h"
#include "parser/grammar.h"
#include "parser/m_grammar.h"
#include "clib/string.h"
#include <assert.h>

struct source_location default_loc = {0, 0, 0, 0};

//forward decl
void nodes_free(struct array *nodes);

const char *node_type_strings[] = {
    FOREACH_NODETYPE(GENERATE_ENUM_STRING)
};


struct ast_node *ast_node_new(symbol node_type_name, enum node_type node_type, enum type annotated_type_enum, struct source_location loc, struct ast_node *parent)
{
    struct ast_node *node;
    MALLOC(node, sizeof(*node)); 
    node->node_type_name = node_type_name;
    node->node_type = node_type;
    node->annotated_type_name = annotated_type_enum ? to_symbol(type_strings[annotated_type_enum]) : 0;
    node->annotated_type_enum = annotated_type_enum;
    node->type = 0;
    node->parent = parent;

    node->loc = loc;
    node->is_ret = false;
    node->data = 0;
   
    array_init(&node->children, sizeof(struct ast_node *));
    return node;
}

void ast_node_free(struct ast_node *node)
{
    if(!node) return;
    size_t len = array_size(&node->children);
    for(size_t i = 0; i < len; i++){
        struct ast_node *child = *(struct ast_node **)array_get(&node->children, i);
        ast_node_free(child);
    }
    array_deinit(&node->children);
    if(node->data)
        FREE(node->data);
    FREE(node);
}

bool is_unary_op(struct ast_node *node)
{
    return node->ft->is_operator && array_size(&node->ft->fun_params) == UNARY_PARAM_SIZE;
}

bool is_binary_op(struct ast_node *node)
{
    return node->ft->is_operator && array_size(&node->ft->fun_params) == BINARY_PARAM_SIZE;
}

char get_op_name(struct ast_node *node)
{
    assert(is_unary_op(node) || is_binary_op(node));
    return string_back(node->ft->name);
}

struct type_exp *get_ret_type(struct ast_node *fun_node)
{
    struct type_oper *oper = (struct type_oper *)fun_node->type;
    return *(struct type_exp **)array_back(&oper->args);
}

struct ast_node *_copy_block_node(struct ast_node *orig_node)
{
    struct array nodes;
    array_init(&nodes, sizeof(struct ast_node *));
    for (size_t i = 0; i < array_size(&orig_node->block->nodes); i++) {
        struct ast_node *node = node_copy(*(struct ast_node **)array_get(&orig_node->block->nodes, i));
        // printf("block node copy: %s\n", node_type_strings[node->node_type]);
        array_push(&nodes, &node);
    }
    return block_node_new(orig_node->parent, &nodes);
}

void _free_block_node(struct ast_node *node)
{
    nodes_free(&node->block->nodes);
    ast_node_free(node);
}

struct array to_symbol_array(struct array arr)
{
    struct array symbols;
    array_init(&symbols, sizeof(symbol *));
    for (size_t i = 0; i < array_size(&arr); i++) {
        symbol symbol = to_symbol(string_get((string *)array_get(&arr, i)));
        array_push(&symbols, &symbol);
    }
    return symbols;
}

struct ast_node *ident_node_new(struct ast_node *parent, struct source_location loc, symbol name)
{

    struct ast_node *node = ast_node_new(0, IDENT_NODE, 0, loc, parent);
    MALLOC(node->ident, sizeof(*node->ident));
    node->ident->name = name;
    node->ident->member_accessors = to_symbol_array(string_split(node->ident->name, '.'));
    return node;
}

struct ast_node *_copy_ident_node(struct ast_node *orig_node)
{
    return ident_node_new(orig_node->parent, orig_node->loc,
        orig_node->ident->name);
}

void _free_ident_node(struct ast_node *node)
{
    array_deinit(&node->ident->member_accessors);
    ast_node_free(node);
}

struct ast_node *_create_literal_node(struct ast_node *parent, struct source_location loc, void *val, enum type type)
{
    struct ast_node *node = ast_node_new(0, LITERAL_NODE, type, loc, parent);
    MALLOC(node->liter, sizeof(*node->liter));
    if (type == TYPE_INT)
        node->liter->int_val = *(int *)val;
    else if (type == TYPE_DOUBLE)
        node->liter->double_val = *(double *)val;
    else if (type == TYPE_CHAR)
        node->liter->char_val = *(char *)val;
    else if (type == TYPE_BOOL)
        node->liter->bool_val = *(bool *)val;
    else if (type == TYPE_STRING)
        node->liter->str_val = str_clone((const char *)val);
    return node;
}

void _free_literal_node(struct ast_node *node)
{
    assert(node->node_type == LITERAL_NODE);
    if (node->annotated_type_enum == TYPE_STRING){
        FREE((void*)node->liter->str_val);
    }
    ast_node_free(node);
}

struct ast_node *double_node_new(struct ast_node *parent, struct source_location loc, double val)
{
    return _create_literal_node(parent, loc, &val, TYPE_DOUBLE);
}

struct ast_node *int_node_new(struct ast_node *parent, struct source_location loc, int val)
{
    return _create_literal_node(parent, loc, &val, TYPE_INT);
}

struct ast_node *bool_node_new(struct ast_node *parent, struct source_location loc, bool val)
{
    return _create_literal_node(parent, loc, &val, TYPE_BOOL);
}

struct ast_node *char_node_new(struct ast_node *parent, struct source_location loc, char val)
{
    return _create_literal_node(parent, loc, &val, TYPE_CHAR);
}

struct ast_node *unit_node_new(struct ast_node *parent, struct source_location loc)
{
    return _create_literal_node(parent, loc, 0, TYPE_UNIT);
}

struct ast_node *string_node_new(struct ast_node *parent, struct source_location loc, const char *val)
{
    return _create_literal_node(parent, loc, (void *)val, TYPE_STRING);
}

struct ast_node *_copy_literal_node(struct ast_node *orig_node)
{
    return _create_literal_node(orig_node->parent, orig_node->loc, &orig_node->liter->char_val,
        orig_node->annotated_type_enum);
}

struct ast_node *var_node_new(struct ast_node *parent, struct source_location loc, symbol var_name, enum type annotated_type_enum, symbol ext_type_name,
    struct ast_node *init_value)
{
    struct ast_node *node = ast_node_new(0, VAR_NODE, annotated_type_enum, loc, parent);
    MALLOC(node->var, sizeof(*node->var));
    if (ext_type_name)
        node->annotated_type_name = ext_type_name;
    node->var->var_name = var_name;
    node->var->init_value = init_value;
    return node;
}

struct ast_node *_copy_var_node(struct ast_node *orig_node)
{
    return var_node_new(orig_node->parent, orig_node->loc,
        orig_node->var->var_name, orig_node->type ? orig_node->type->type : TYPE_UNK, 0,
        node_copy(orig_node->var->init_value));
}

void _free_var_node(struct ast_node *node)
{
    if (node->var->init_value)
        node_free(node->var->init_value);
    ast_node_free(node);
}

struct ast_node *type_node_new(struct ast_node *parent, struct source_location loc, symbol name, struct ast_node *body)
{
    struct ast_node *node = ast_node_new(0, TYPE_NODE, 0, loc, parent);
    MALLOC(node->type_def, sizeof(*node->type_def));
    node->type_def->name = name;
    node->type_def->body = body;
    return node;
}

struct ast_node *_copy_type_node(struct ast_node *orig_node)
{
    return type_node_new(orig_node->parent, orig_node->loc,
        orig_node->type_def->name, _copy_block_node(orig_node->type_def->body));
}

void _free_type_node(struct ast_node *node)
{
    _free_block_node(node->type_def->body);
    ast_node_free(node);
}

struct ast_node *type_value_node_new(struct ast_node *parent, struct source_location loc, struct ast_node *body, symbol type_symbol)
{
    struct ast_node *node = ast_node_new(0, TYPE_VALUE_NODE, TYPE_EXT, loc, parent);
    node->annotated_type_name = type_symbol;
    MALLOC(node->type_value, sizeof(*node->type_value));
    node->type_value->body = body;
    return node;
}

struct ast_node *_copy_type_value_node(struct ast_node *orig_node)
{
    return type_value_node_new(orig_node->parent, orig_node->loc,
        _copy_block_node(orig_node->type_value->body), orig_node->annotated_type_name);
}

void _free_type_value_node(struct ast_node *node)
{
    _free_block_node(node->type_value->body);
    ast_node_free(node);
}

struct ast_node *call_node_new(struct ast_node *parent, struct source_location loc, symbol callee,
    struct array *args)
{
    struct ast_node *node = ast_node_new(0, CALL_NODE, 0, loc, parent);
    MALLOC(node->call, sizeof(*node->call));
    node->call->callee = callee;
    array_copy(&node->call->args, args);
    node->call->specialized_callee = 0;
    node->call->callee_func_type = 0;
    return node;
}

struct ast_node *_copy_call_node(struct ast_node *orig_node)
{
    return call_node_new(orig_node->parent, orig_node->loc, orig_node->call->callee,
        &orig_node->call->args);
}

void _free_call_node(struct ast_node *node)
{
    nodes_free(&node->call->args);   
    ast_node_free(node);
}

symbol get_callee(struct ast_node *call)
{
    return call->call->specialized_callee ? call->call->specialized_callee : call->call->callee;
}

struct ast_node *func_type_node_default_new(struct ast_node *parent, struct source_location loc, symbol name, struct array *args,
    struct type_exp *ret_type, bool is_variadic, bool is_external)
{
    return func_type_node_new(parent, loc, name, args, ret_type, false, 0,
        0, is_variadic, is_external);
}

struct ast_node *func_type_node_new(struct ast_node *parent, struct source_location loc, symbol name,
    struct array *params,
    struct type_exp *ret_type,
    bool is_operator, unsigned precedence, symbol op,
    bool is_variadic, bool is_external)
{
    enum type type = ret_type ? ret_type->type : TYPE_UNK;
    struct ast_node *node = ast_node_new(0, FUNC_TYPE_NODE, type, loc, parent);
    MALLOC(node->ft, sizeof(*node->ft));
    node->ft->name = name;
    node->ft->fun_params = *params;
    node->ft->is_operator = is_operator;
    node->ft->precedence = precedence;
    node->ft->is_variadic = is_variadic;
    node->ft->is_extern = is_external;
    node->ft->op = op;
    if (is_variadic) {
        struct ast_node *fun_param = var_node_new(node, loc, to_symbol(type_strings[TYPE_GENERIC]), TYPE_GENERIC, 0, 0);
        fun_param->type = (struct type_exp *)create_nullary_type(TYPE_GENERIC, fun_param->annotated_type_name);
        array_push(&node->ft->fun_params, &fun_param);
    }
    return node;
}

struct ast_node *_copy_func_type_node(struct ast_node *func_type)
{
    struct ast_node *node = ast_node_new(0, func_type->node_type, 
        func_type->annotated_type_enum, func_type->loc, func_type->parent);
    MALLOC(node->ft, sizeof(*node->ft));
    node->ft->name = func_type->ft->name;
    node->ft->fun_params = func_type->ft->fun_params;
    node->ft->is_operator = func_type->ft->is_operator;
    node->ft->precedence = func_type->ft->precedence;
    node->ft->is_variadic = func_type->ft->is_variadic;
    node->ft->is_extern = func_type->ft->is_extern;
    node->ft->op = func_type->ft->op;
    if (func_type->ft->is_variadic) {
        symbol var_name = to_symbol(type_strings[TYPE_GENERIC]);
        struct ast_node *fun_param = var_node_new(node, node->loc, var_name, TYPE_GENERIC, 0, 0);
        array_push(&node->ft->fun_params, &fun_param);
    }
    return node;
}

void _free_func_type_node(struct ast_node *node)
{
    /*TODO: fun_params will be freed in array_deinit, make sure free function is provided*/
    array_deinit(&node->ft->fun_params);
    ast_node_free(node);
}

struct ast_node *function_node_new(struct ast_node *func_type,
    struct ast_node *body)
{
    struct ast_node *node = ast_node_new(0, FUNCTION_NODE, 0, func_type->loc, func_type);
    MALLOC(node->func, sizeof(*node->func));
    node->func->func_type = func_type;
    node->func->body = body;
    array_init(&node->func->sp_funs, sizeof(struct ast_node*));
    return node;
}

struct ast_node *_copy_function_node(struct ast_node *orig_node)
{
    struct ast_node *func_type = _copy_func_type_node(orig_node->func->func_type);
    struct ast_node *block = _copy_block_node(orig_node->func->body);
    return function_node_new(func_type, block);
}

struct ast_node *find_sp_fun(struct ast_node *generic_fun, symbol sp_fun_name)
{
    assert(generic_fun->node_type == FUNCTION_NODE);
    struct ast_node *sp_fun = 0;
    for(size_t i = 0; i < array_size(&generic_fun->func->sp_funs); i++){
        struct ast_node *fun = *(struct ast_node **)array_get(&generic_fun->func->sp_funs, i);
        if(fun->func->func_type->ft->name == sp_fun_name){
            sp_fun = fun;
            break;
        } 
    }
    return sp_fun;
}

void _free_function_node(struct ast_node *node)
{
    _free_func_type_node(node->func->func_type);
    _free_block_node(node->func->body);
    /*TODO: sp_funs make sure free function is provided*/
    array_deinit(&node->func->sp_funs);
    ast_node_free(node);
}

struct ast_node *if_node_new(struct ast_node *parent, struct source_location loc,
    struct ast_node *if_node, struct ast_node *then_node, struct ast_node *else_node)
{
    struct ast_node *node = ast_node_new(0, CONDITION_NODE, 0, loc, parent);
    MALLOC(node->cond, sizeof(*node->cond));
    node->cond->if_node = if_node;
    node->cond->then_node = then_node;
    node->cond->else_node = else_node;
    return node;
}

struct ast_node *_copy_if_node(struct ast_node *orig_node)
{
    return if_node_new(orig_node->parent, orig_node->loc, orig_node->cond->if_node,
        orig_node->cond->then_node, orig_node->cond->else_node);
}

void _free_if_node(struct ast_node *node)
{
    if (node->cond->if_node)
        node_free(node->cond->if_node);
    if (node->cond->then_node)
        node_free(node->cond->then_node);
    if (node->cond->else_node)
        node_free(node->cond->else_node);
    ast_node_free(node);
}

struct ast_node *unary_node_new(struct ast_node *parent, struct source_location loc, symbol op, struct ast_node *operand)
{
    struct ast_node *node = ast_node_new(0, UNARY_NODE, 0, loc, parent);
    MALLOC(node->unop, sizeof(*node->unop));
    node->unop->op = op;
    node->unop->operand = operand;
    return node;
}

struct ast_node *_copy_unary_node(struct ast_node *orig_node)
{
    return unary_node_new(orig_node->parent, orig_node->loc, orig_node->unop->op,
        orig_node->unop->operand);
}

void _free_unary_node(struct ast_node *node)
{
    node_free(node->unop->operand);
    ast_node_free(node);
}

struct ast_node *binary_node_new(struct ast_node *parent, struct source_location loc, symbol op, struct ast_node *lhs, struct ast_node *rhs)
{
    struct ast_node *node = ast_node_new(0, BINARY_NODE, 0, loc, parent);
    MALLOC(node->binop, sizeof(*node->binop));
    node->binop->op = op;
    node->binop->lhs = lhs;
    node->binop->rhs = rhs;
    return node;
}

struct ast_node *_copy_binary_node(struct ast_node *orig_node)
{
    return binary_node_new(orig_node->parent, orig_node->loc, orig_node->binop->op,
        orig_node->binop->lhs, orig_node->binop->rhs);
}

void _free_binary_node(struct ast_node *node)
{
    if (node->binop->lhs)
        node_free(node->binop->lhs);
    if (node->binop->rhs)
        node_free(node->binop->rhs);
    ast_node_free(node);
}

struct ast_node *for_node_new(struct ast_node *parent, struct source_location loc, symbol var_name, struct ast_node *start,
    struct ast_node *end, struct ast_node *step, struct ast_node *body)
{
    struct ast_node *node = ast_node_new(0, FOR_NODE, 0, loc, parent);
    MALLOC(node->forloop, sizeof(*node->forloop));
    node->forloop->var_name = var_name;
    node->forloop->start = start;
    node->forloop->end = end;
    node->forloop->step = step;
    node->forloop->body = body;
    return node;
}

struct ast_node *_copy_for_node(struct ast_node *orig_node)
{
    return for_node_new(orig_node->parent, orig_node->loc,
        orig_node->forloop->var_name, orig_node->forloop->start, orig_node->forloop->end, orig_node->forloop->step, orig_node->forloop->body);
}

void _free_for_node(struct ast_node *node)
{
    if (node->forloop->start)
        node_free(node->forloop->start);
    if (node->forloop->end)
        node_free(node->forloop->end);
    if (node->forloop->step)
        node_free(node->forloop->step);
    if (node->forloop->body)
        node_free(node->forloop->body);
    ast_node_free(node);
}

struct ast_node *block_node_new(struct ast_node *parent, struct array *nodes)
{
    struct source_location loc = nodes ? (*(struct ast_node **)array_front(nodes))->loc : default_loc;
    struct ast_node *node = ast_node_new(0, BLOCK_NODE, 0, loc, parent);
    MALLOC(node->block, sizeof(*node->block));
    if(nodes)
        node->block->nodes = *nodes;
    return node;
}

struct ast_node *node_copy(struct ast_node *node)
{
    switch (node->node_type) {
    case BLOCK_NODE:
        return _copy_block_node(node);
    case FUNC_TYPE_NODE:
        return _copy_func_type_node(node);
    case FUNCTION_NODE:
        return _copy_function_node(node);
    case VAR_NODE:
        return _copy_var_node(node);
    case TYPE_NODE:
        return _copy_type_node(node);
    case TYPE_VALUE_NODE:
        return _copy_type_value_node(node);
    case IDENT_NODE:
        return _copy_ident_node(node);
    case LITERAL_NODE:
        return _copy_literal_node(node);
    case CALL_NODE:
        return _copy_call_node(node);
    case CONDITION_NODE:
        return _copy_if_node(node);
    case FOR_NODE:
        return _copy_for_node(node);
    case UNARY_NODE:
        return _copy_unary_node(node);
    case BINARY_NODE:
        return _copy_binary_node(node);
    default:
        assert(false);
    }
    return 0;
}

void node_free(struct ast_node *node)
{
    switch (node->node_type) {
    case UNK_NODE:
        FREE(node);
        break;
    case BLOCK_NODE:
        _free_block_node(node);
        break;
    case FUNC_TYPE_NODE:
        _free_func_type_node(node);
        break;
    case FUNCTION_NODE:
        _free_function_node(node);
        break;
    case VAR_NODE:
        _free_var_node(node);
        break;
    case TYPE_NODE:
        _free_type_node(node);
        break;
    case TYPE_VALUE_NODE:
        _free_type_value_node(node);
        break;
    case IDENT_NODE:
        _free_ident_node(node);
        break;
    case LITERAL_NODE:
        _free_literal_node(node);
        break;
    case CALL_NODE:
        _free_call_node(node);
        break;
    case CONDITION_NODE:
        _free_if_node(node);
        break;
    case FOR_NODE:
        _free_for_node(node);
        break;
    case UNARY_NODE:
        _free_unary_node(node);
        break;
    case BINARY_NODE:
        _free_binary_node(node);
        break;
    default:
        printf("not supported node type: %d\n", node->node_type);
        assert(false);
    }
}

void nodes_free(struct array *nodes)
{
    for (size_t i = 0; i < array_size(nodes); i++) {
        struct ast_node *elem = *(struct ast_node **)array_get(nodes, i);
        node_free(elem);
    }
    array_deinit(nodes);
}

struct module *module_new(const char *mod_name, FILE *file)
{
    struct module *mod;
    MALLOC(mod, sizeof(*mod));
    // printf("new module: %s\n", mod_name);
    mod->name = to_symbol(mod_name);
    // printf("got new module: %s\n", mod_name);
    struct ast_node *node = block_node_new(0, 0);
    array_init(&node->block->nodes, sizeof(struct ast_node *));
    mod->block = node;
    mod->tokenizer = create_tokenizer(file, mod_name, keyword_symbols, keyword_count);
    return mod;
}

bool is_recursive(struct ast_node *call)
{
    struct ast_node *parent = call->parent;
    while (parent) {
        if (parent->node_type == FUNC_TYPE_NODE){
            if (parent->ft->name == call->call->callee)
                return true;
        }
        parent = parent->parent;
    }
    return false;
}

int find_member_index(struct ast_node *type_node, symbol member)
{
    for (int i = 0; i < (int)array_size(&type_node->type_def->body->block->nodes); i++) {
        struct ast_node *var = *(struct ast_node **)array_get(&type_node->type_def->body->block->nodes, i);
        if (var->var->var_name == member) {
            return i;
        }
    }
    return -1;
}

struct ast_node *find_parent_proto(struct ast_node *node)
{
    struct ast_node *func_type = 0;
    while (node->parent) {
        if (node->parent->node_type == FUNC_TYPE_NODE) {
            func_type = node->parent;
            break;
        }
        node = node->parent;
    }
    return func_type;
}
