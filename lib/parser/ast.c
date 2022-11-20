/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language ast c code implementation.
 *
 */
#include "parser/ast.h"
#include "parser/node_type.h"
#include "clib/array.h"
#include "parser/grammar.h"
#include "clib/string.h"
#include <assert.h>

struct source_location default_loc = {0, 0, 0, 0};

void ast_init()
{
    node_type_init();
}

void ast_deinit()
{
    node_type_deinit();
}

//forward decl
void nodes_free(struct array *nodes);


struct ast_node *ast_node_new(enum node_type node_type, struct source_location loc)
{
    struct ast_node *node;
    MALLOC(node, sizeof(*node)); 
    node->node_type = node_type;
    node->type = 0;
    node->loc = loc;
    node->is_ret = false;
    node->is_lvalue = false;
    node->data = 0;
    node->is_addressed = false;
    node->is_addressable = false;
    node->transformed = 0;
    return node;
}

void ast_node_free(struct ast_node *node)
{
    if(!node) return;
    if(node->data)
        FREE(node->data);
    if(node->transformed)
        FREE(node->transformed);
    FREE(node);
}

bool is_unary_op(struct ast_node *node)
{
    return node->ft->is_operator && array_size(&node->ft->params->block->nodes) == UNARY_PARAM_SIZE;
}

bool is_binary_op(struct ast_node *node)
{
    return node->ft->is_operator && array_size(&node->ft->params->block->nodes) == BINARY_PARAM_SIZE;
}

char get_op_name(struct ast_node *node)
{
    assert(is_unary_op(node) || is_binary_op(node));
    return string_back(node->ft->name);
}

struct type_expr *get_ret_type(struct ast_node *fun_node)
{
    return *(struct type_expr **)array_back(&fun_node->type->args);
}

struct ast_node *_copy_block_node(struct ast_node *orig_node)
{
    struct array nodes;
    array_init(&nodes, sizeof(struct ast_node *));
    for (size_t i = 0; i < array_size(&orig_node->block->nodes); i++) {
        struct ast_node *node = node_copy(*(struct ast_node **)array_get(&orig_node->block->nodes, i));
        array_push(&nodes, &node);
    }
    return block_node_new(&nodes);
}

void _free_block_node(struct ast_node *node)
{
    nodes_free(&node->block->nodes);
    ast_node_free(node);
}

void free_block_node(struct ast_node *node, bool deep_free)
{
    if(!node) return;
    if(deep_free){
        _free_block_node(node);
    }else{
        array_deinit(&node->block->nodes);
        ast_node_free(node);
    }
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

struct ast_node *ident_node_new(symbol name, struct source_location loc)
{
    struct ast_node *node = ast_node_new(IDENT_NODE, loc);
    MALLOC(node->ident, sizeof(*node->ident));
    node->ident->name = name;
    node->ident->var = 0;
    node->ident->is_member_index_object = false;
    return node;
}

struct ast_node *_copy_ident_node(struct ast_node *orig_node)
{
    return ident_node_new(
        orig_node->ident->name, orig_node->loc);
}

void _free_ident_node(struct ast_node *node)
{
    ast_node_free(node);
}

struct ast_node *type_node_new_with_type_name(symbol type_name, struct source_location loc)
{
    struct ast_node *node = ast_node_new(TYPE_NODE, loc);
    MALLOC(node->type_node, sizeof(*node->type_node));
    node->type_node->kind = TypeName;
    node->type_node->is_mut = 0;
    node->type_node->type_name = type_name;
    return node;
}

struct ast_node *type_node_new_with_array_type(struct array_type_node *array_type_node, struct source_location loc)
{
    struct ast_node *node = ast_node_new(TYPE_NODE, loc);
    MALLOC(node->type_node, sizeof(*node->type_node));
    node->type_node->kind = ArrayType;
    node->type_node->is_mut = 0;
    node->type_node->array_type_node = array_type_node;
    return node;
}

struct ast_node *type_node_new_with_ref_type(struct type_node *val_node, struct source_location loc)
{
    struct ast_node *node = ast_node_new(TYPE_NODE, loc);
    MALLOC(node->type_node, sizeof(*node->type_node));
    node->type_node->kind = RefType;
    node->type_node->is_mut = 0;
    node->type_node->val_node = val_node;
    return node;
}

struct ast_node *type_node_new_with_unit_type(struct source_location loc)
{
    struct ast_node *node = ast_node_new(TYPE_NODE, loc);
    MALLOC(node->type_node, sizeof(*node->type_node));
    node->type_node->kind = UnitType;
    node->type_node->is_mut = 0;
    node->type_node->array_type_node = 0;
    return node;
}

struct ast_node *_copy_type_node(struct ast_node *orig_node)
{
    switch(orig_node->type_node->kind){
        case ArrayType:
            return type_node_new_with_array_type(orig_node->type_node->array_type_node, orig_node->loc);
        case UnitType:
            return type_node_new_with_unit_type(orig_node->loc);
        case TypeName:
            return type_node_new_with_type_name(orig_node->type_node->type_name, orig_node->loc);
        case RefType:
            return type_node_new_with_ref_type(orig_node->type_node->val_node, orig_node->loc);
    }
}

void _free_real_type_node(struct type_node *type_node)
{
    if(!type_node) return;
    if(type_node->kind == ArrayType){
        ast_node_free(type_node->array_type_node->dims);
        ast_node_free(type_node->array_type_node->elm_type);
    } else if(type_node->kind == RefType){
        _free_real_type_node(type_node->val_node);
    }
}

void _free_type_node(struct ast_node *node)
{
    _free_real_type_node(node->type_node);
    ast_node_free(node);
}

struct ast_node *_create_literal_int_node(int val, enum type type, struct source_location loc)
{
    struct ast_node *node = ast_node_new(LITERAL_NODE, loc);
    MALLOC(node->liter, sizeof(*node->liter));
    node->type = create_nullary_type(type, get_type_symbol(type));
    node->liter->type = type;
    switch (type){ 
        case TYPE_INT:
        case TYPE_CHAR:
        case TYPE_BOOL:
        case TYPE_I8:
        case TYPE_I16:
        case TYPE_I32:
        case TYPE_I64:
            node->liter->int_val = val;
            break;
        default:
            printf("doesn't support the type %s for int literal.\n", string_get(get_type_symbol(type)));
            break;
    }
    return node;
}

struct ast_node *_create_literal_node(void *val, enum type type, struct source_location loc)
{
    struct ast_node *node = ast_node_new(LITERAL_NODE, loc);
    MALLOC(node->liter, sizeof(*node->liter));
    node->type = create_nullary_type(type, get_type_symbol(type));
    node->liter->type = type;
    switch (type){ 
        case TYPE_INT:
        case TYPE_CHAR:
        case TYPE_BOOL:
        case TYPE_I8:
        case TYPE_I16:
        case TYPE_I32:
        case TYPE_I64:
        case TYPE_U8:
        case TYPE_U16:
        case TYPE_U32:
        case TYPE_U64:
            node->liter->int_val = *(int*)val;
            break;
        case TYPE_F32:
        case TYPE_F64:
            node->liter->double_val = *(f64 *)val;
            break;
        case TYPE_STRING:
            node->liter->str_val = str_clone((const char *)val);
            break;
        default:
            printf("doesn't support the type %s for literal.\n", string_get(get_type_symbol(type)));
            break;
    }
    return node;
}

void _free_literal_node(struct ast_node *node)
{
    assert(node->node_type == LITERAL_NODE);
    if (node->liter->type == TYPE_STRING){
        FREE((void*)node->liter->str_val);
    }
    ast_node_free(node);
}

struct ast_node *double_node_new(f64 val, struct source_location loc)
{
    return _create_literal_node(&val, TYPE_F64, loc);
}

struct ast_node *int_node_new(int val, struct source_location loc)
{
    return _create_literal_int_node(val, TYPE_INT, loc);
}

struct ast_node *bool_node_new(bool val, struct source_location loc)
{
    return _create_literal_int_node((int)val, TYPE_BOOL, loc);
}

struct ast_node *const_one_node_new(enum type type, struct source_location loc)
{
    if(type == TYPE_F64){
        return double_node_new(1.0, loc);
    }
    return int_node_new(1, loc);
}

struct ast_node *char_node_new(char val, struct source_location loc)
{
    return _create_literal_int_node((int)val, TYPE_CHAR, loc);
}

struct ast_node *unit_node_new(struct source_location loc)
{
    return _create_literal_node(0, TYPE_UNIT, loc);
}

struct ast_node *string_node_new(const char *val, struct source_location loc)
{
    return _create_literal_node((void *)val, TYPE_STRING, loc);
}

struct ast_node *_copy_literal_node(struct ast_node *orig_node)
{
    return _create_literal_node(&orig_node->liter->int_val,
        orig_node->liter->type, orig_node->loc);
}

struct ast_node *var_node_new(struct ast_node *var, struct ast_node *is_of_type,
    struct ast_node *init_value, bool is_global, bool is_mut, struct source_location loc)
{   
    struct ast_node *node = ast_node_new(VAR_NODE, loc);
    MALLOC(node->var, sizeof(*node->var));
    node->var->var = var;
    node->var->init_value = init_value;
    node->var->is_of_type = is_of_type;
    node->var->is_global = is_global;
    node->var->is_mut = is_mut;
    node->var->is_init_shared = 0;
    node->is_addressable = true;
    return node;
}

struct ast_node *_copy_var_node(struct ast_node *orig_node)
{
    return var_node_new(
        orig_node->var->var, node_copy(orig_node->var->is_of_type),
        node_copy(orig_node->var->init_value), orig_node->var->is_global, orig_node->var->is_mut, 
        orig_node->loc);
}

void _free_var_node(struct ast_node *node)
{
    if(node->var->is_of_type)
        node_free(node->var->is_of_type);
    if (node->var->init_value && !node->var->is_init_shared)
        node_free(node->var->init_value);
    if(node->var->var)
        node_free(node->var->var);
    ast_node_free(node);
}

struct ast_node *struct_node_new(symbol name, struct ast_node *body, struct source_location loc)
{
    struct ast_node *node = ast_node_new(STRUCT_NODE, loc);
    MALLOC(node->struct_def, sizeof(*node->struct_def));
    node->struct_def->name = name;
    node->struct_def->body = body;
    return node;
}

struct ast_node *_copy_struct_node(struct ast_node *orig_node)
{
    return struct_node_new(
        orig_node->struct_def->name, _copy_block_node(orig_node->struct_def->body), orig_node->loc);
}

void _free_struct_node(struct ast_node *node)
{
    _free_block_node(node->struct_def->body);
    ast_node_free(node);
}

struct ast_node *struct_init_node_new(struct ast_node *body, struct ast_node *type_node, struct source_location loc)
{
    struct ast_node *node = ast_node_new(STRUCT_INIT_NODE, loc);
    MALLOC(node->struct_init, sizeof(*node->struct_init));
    node->struct_init->body = body;
    node->struct_init->is_of_type = type_node;
    return node;
}

struct ast_node *_copy_struct_init_node(struct ast_node *orig_node)
{
    return struct_init_node_new(
        _copy_block_node(orig_node->struct_init->body), orig_node->struct_init->is_of_type, orig_node->loc);
}

void _free_struct_init_node(struct ast_node *node)
{
    _free_block_node(node->struct_init->body);
    ast_node_free(node->struct_init->is_of_type);
    ast_node_free(node);
}


struct ast_node *range_node_new(struct ast_node *start, struct ast_node *end, struct ast_node *step, struct source_location loc)
{
    struct ast_node *node = ast_node_new(RANGE_NODE, loc);
    MALLOC(node->range, sizeof(*node->range));
    node->range->start = start;
    node->range->end = end;
    node->range->step = step;
    return node;
}

struct ast_node *_copy_range_node(struct ast_node *orig_node)
{
    return range_node_new(
        node_copy(orig_node->range->start), node_copy(orig_node->range->end), 
        node_copy(orig_node->range->step), orig_node->loc);
}

void _free_range_node(struct ast_node *node)
{
    ast_node_free(node->range->start);
    ast_node_free(node->range->end);
    ast_node_free(node->range->step);
    ast_node_free(node);
}

struct ast_node *array_init_node_new(struct ast_node *comp, struct source_location loc)
{
    struct ast_node *node = ast_node_new(ARRAY_INIT_NODE, loc);
    MALLOC(node->array_init, sizeof(*node->array_init));
    node->array_init = comp;
    return node;
}

struct ast_node *_copy_array_init_node(struct ast_node *orig_node)
{
    return array_init_node_new(
        node_copy(orig_node->array_init), orig_node->loc);
}

void _free_array_init_node(struct ast_node *node)
{
    ast_node_free(node);
}

/********/
struct ast_node *array_type_node_new(struct ast_node *elm_type, struct ast_node *dims, struct source_location loc)
{
    struct ast_node *node = ast_node_new(ARRAY_TYPE_NODE, loc);
    MALLOC(node->array_type, sizeof(*node->array_type));
    node->array_type->elm_type = elm_type;
    node->array_type->dims = dims;
    return node;
}

struct ast_node *_copy_array_type_node(struct ast_node *orig_node)
{
    return array_type_node_new(
        node_copy(orig_node->array_type->elm_type), node_copy(orig_node->array_type->dims), orig_node->loc);
}

void _free_array_type_node(struct ast_node *node)
{
    if(node->array_type){
        ast_node_free(node->array_type->elm_type);
        ast_node_free(node->array_type->dims);
    }
    ast_node_free(node);
}
/*******/

struct ast_node *import_node_new(symbol from_module, struct ast_node *imported, struct source_location loc)
{
    struct ast_node *node = ast_node_new(IMPORT_NODE, loc);
    MALLOC(node->import, sizeof(*node->import));
    node->import->import = imported;
    node->import->from_module = from_module;
    return node;
}

struct ast_node *_copy_import_node(struct ast_node *orig_node)
{
    return import_node_new(orig_node->import->from_module,
        node_copy(orig_node->import->import), orig_node->loc);
}

void _free_import_node(struct ast_node *node)
{
    //ast_node_free(node->import->import);
    node_free(node->import->import);
    ast_node_free(node);
}

struct ast_node *memory_node_new(struct ast_node *initial, struct ast_node *max, struct source_location loc)
{
    struct ast_node *node = ast_node_new(MEMORY_NODE, loc);
    MALLOC(node->memory, sizeof(*node->memory));
    node->memory->initial = initial;
    node->memory->max = max;
    return node;
}

struct ast_node *_copy_memory_node(struct ast_node *orig_node)
{
    return memory_node_new(
        node_copy(orig_node->memory->initial),
        orig_node->memory->max? node_copy(orig_node->memory->max) : 0,
        orig_node->loc);
}

void _free_memory_node(struct ast_node *node)
{
    node_free(node->memory->initial);
    node_free(node->memory->max);

    ast_node_free(node);
}

struct ast_node *call_node_new(symbol callee,
    struct ast_node *arg_block, struct source_location loc)
{
    assert(arg_block);
    struct ast_node *node = ast_node_new(CALL_NODE, loc);
    MALLOC(node->call, sizeof(*node->call));
    node->call->callee = callee;
    node->call->arg_block = arg_block;
    node->call->specialized_callee = 0;
    node->call->callee_func_type = 0;
    return node;
}

struct ast_node *_copy_call_node(struct ast_node *orig_node)
{
    return call_node_new(orig_node->call->callee,
        _copy_block_node(orig_node->call->arg_block), orig_node->loc);
}

void _free_call_node(struct ast_node *node)
{
    ast_node_free(node->call->arg_block);
    ast_node_free(node);
}

symbol get_callee(struct ast_node *call)
{
    return call->call->specialized_callee ? call->call->specialized_callee : call->call->callee;
}

struct ast_node *func_type_node_default_new(symbol name, struct ast_node *arg_block,
    symbol ret_type, struct ast_node *ret_type_node, bool is_variadic, bool is_external, struct source_location loc)
{
    return func_type_node_new(name, arg_block, ret_type, ret_type_node, is_variadic, is_external, loc);
}

struct ast_node *func_type_node_new(symbol name,
    struct ast_node *params, /*params block ast node*/
    symbol ret_type,
    struct ast_node *ret_type_node, 
    bool is_variadic, bool is_external, struct source_location loc)
{
    struct ast_node *node = ast_node_new(FUNC_TYPE_NODE, loc);
    MALLOC(node->ft, sizeof(*node->ft));
    node->ft->name = name;
    node->ft->params = params;
    node->ft->ret_type_node = ret_type_node;
    node->ft->is_operator = false;
    node->ft->precedence = 0;
    node->ft->is_variadic = is_variadic;
    node->ft->is_extern = is_external;
    node->ft->op = 0;
    if (is_variadic) {
        symbol symbol_name = get_type_symbol(TYPE_GENERIC);
        struct ast_node *is_of_type = type_node_new_with_type_name(symbol_name, loc);
        struct ast_node *fun_param = var_node_new(ident_node_new(symbol_name, loc), is_of_type, 0, false, true, loc);
        fun_param->type = create_nullary_type(TYPE_GENERIC, symbol_name);
        array_push(&node->ft->params->block->nodes, &fun_param);
    }
    return node;
}

struct ast_node *_copy_func_type_node(struct ast_node *func_type)
{
    struct ast_node *node = ast_node_new(func_type->node_type, func_type->loc);
    MALLOC(node->ft, sizeof(*node->ft));
    node->ft->name = func_type->ft->name;
    node->ft->params = _copy_block_node(func_type->ft->params);
    node->ft->is_operator = func_type->ft->is_operator;
    node->ft->precedence = func_type->ft->precedence;
    node->ft->is_variadic = func_type->ft->is_variadic;
    node->ft->is_extern = func_type->ft->is_extern;
    node->ft->op = func_type->ft->op;
    if (func_type->ft->is_variadic) {
        symbol var_name = get_type_symbol(TYPE_GENERIC);
        struct ast_node *is_of_type = type_node_new_with_type_name(var_name, node->loc);
        struct ast_node *fun_param = var_node_new(ident_node_new(var_name, node->loc), is_of_type, 0, false, true, node->loc);
        array_push(&node->ft->params->block->nodes, &fun_param);
    }
    return node;
}

void _free_func_type_node(struct ast_node *node)
{
    node_free(node->ft->params);
    node_free(node->ft->ret_type_node);
    ast_node_free(node);
}

void _free_sp(void *elm)
{
    //?? node_free(elm);
}

struct ast_node *function_node_new(struct ast_node *func_type,
    struct ast_node *body, struct source_location loc)
{
    struct ast_node *node = ast_node_new(FUNC_NODE, loc);
    MALLOC(node->func, sizeof(*node->func));
    node->func->func_type = func_type;
    node->func->body = body;
    array_init_free(&node->func->sp_funs, sizeof(struct ast_node*), _free_sp);
    return node;
}

struct ast_node *_copy_function_node(struct ast_node *orig_node)
{
    struct ast_node *func_type = _copy_func_type_node(orig_node->func->func_type);
    struct ast_node *block = _copy_block_node(orig_node->func->body);
    return function_node_new(func_type, block, orig_node->loc);
}

struct ast_node *find_sp_fun(struct ast_node *generic_fun, symbol sp_fun_name)
{
    assert(generic_fun->node_type == FUNC_NODE);
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

struct ast_node *if_node_new(
    struct ast_node *if_node, struct ast_node *then_node, struct ast_node *else_node, struct source_location loc)
{
    struct ast_node *node = ast_node_new(IF_NODE, loc);
    MALLOC(node->cond, sizeof(*node->cond));
    node->cond->if_node = if_node;
    node->cond->then_node = then_node;
    node->cond->else_node = else_node;
    return node;
}

struct ast_node *_copy_if_node(struct ast_node *orig_node)
{
    return if_node_new(orig_node->cond->if_node,
        orig_node->cond->then_node, orig_node->cond->else_node, orig_node->loc);
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

struct ast_node *unary_node_new(enum op_code opcode, struct ast_node *operand, bool is_postfix, struct source_location loc)
{
    struct ast_node *node = ast_node_new(UNARY_NODE, loc);
    MALLOC(node->unop, sizeof(*node->unop));
    node->unop->opcode = opcode;
    node->unop->operand = operand;
    node->unop->is_postfix = is_postfix;
    if(opcode == OP_STAR){//dereference
        node->is_addressable = true;
    }
    return node;
}

struct ast_node *_copy_unary_node(struct ast_node *orig_node)
{
    return unary_node_new(orig_node->unop->opcode,
        orig_node->unop->operand, orig_node->unop->is_postfix, orig_node->loc);
}

void _free_unary_node(struct ast_node *node)
{
    node_free(node->unop->operand);
    ast_node_free(node);
}

struct ast_node *binary_node_new(enum op_code opcode, struct ast_node *lhs, struct ast_node *rhs, struct source_location loc)
{
    struct ast_node *node = ast_node_new(BINARY_NODE, loc);
    MALLOC(node->binop, sizeof(*node->binop));
    node->binop->opcode = opcode;
    node->binop->lhs = lhs;
    node->binop->rhs = rhs;
    return node;
}

struct ast_node *_copy_binary_node(struct ast_node *orig_node)
{
    return binary_node_new(orig_node->binop->opcode,
        orig_node->binop->lhs, orig_node->binop->rhs, orig_node->loc);
}

void _free_binary_node(struct ast_node *node)
{
    if (node->binop->lhs)
        node_free(node->binop->lhs);
    if (node->binop->rhs)
        node_free(node->binop->rhs);
    ast_node_free(node);
}

struct ast_node *assign_node_new(enum op_code opcode, struct ast_node *lhs, struct ast_node *rhs, struct source_location loc)
{
    struct ast_node *node = ast_node_new(ASSIGN_NODE, loc);
    MALLOC(node->binop, sizeof(*node->binop));
    node->binop->opcode = opcode;
    node->binop->lhs = lhs;
    node->binop->rhs = rhs;
    return node;
}

struct ast_node *_copy_assign_node(struct ast_node *orig_node)
{
    return assign_node_new(orig_node->binop->opcode,
        orig_node->binop->lhs, orig_node->binop->rhs, orig_node->loc);
}

void _free_assign_node(struct ast_node *node)
{
    if (node->binop->lhs)
        node_free(node->binop->lhs);
    if (node->binop->rhs)
        node_free(node->binop->rhs);
    ast_node_free(node);
}

struct ast_node *cast_node_new(struct ast_node *to_type_node, struct ast_node *expr, struct source_location loc)
{
    struct ast_node *node = ast_node_new(CAST_NODE, loc);
    MALLOC(node->cast, sizeof(*node->cast));
    node->cast->to_type_node = to_type_node;
    node->cast->expr = expr;
    return node;
}

struct ast_node *_copy_cast_node(struct ast_node *orig_node)
{
    return cast_node_new(orig_node->cast->to_type_node,
        orig_node->cast->expr, orig_node->loc);
}

void _free_cast_node(struct ast_node *node)
{
    if (node->cast->to_type_node)
        node_free(node->cast->to_type_node);
    if (node->cast->expr)
        node_free(node->cast->expr);
    ast_node_free(node);
}

struct ast_node *member_index_node_new(enum aggregate_type aggregate_type, struct ast_node *object, struct ast_node *index, struct source_location loc)
{
    struct ast_node *node = ast_node_new(MEMBER_INDEX_NODE, loc);
    MALLOC(node->index, sizeof(*node->index));
    if(object->node_type == IDENT_NODE){
        object->ident->is_member_index_object = true;
    }
    node->index->object = object;
    node->index->index = index;
    node->index->aggregate_type = aggregate_type;
    node->is_addressable = true;
    return node;
}

struct ast_node *_copy_member_index_node(struct ast_node *orig_node)
{
    return member_index_node_new(orig_node->index->aggregate_type,
        orig_node->index->object, orig_node->index->index, orig_node->loc);
}

void _free_member_index_node(struct ast_node *node)
{
    if (node->index->object)
        node_free(node->index->object);
    if (node->index->index)
        node_free(node->index->index);
    ast_node_free(node);
}

struct ast_node *for_node_new(struct ast_node *var, struct ast_node *range,
    struct ast_node *body, struct source_location loc)
{
    struct ast_node *node = ast_node_new(FOR_NODE, loc);
    MALLOC(node->forloop, sizeof(*node->forloop));
    node->forloop->var = var;
    node->forloop->range = range;
    node->forloop->body = body;
    return node;
}

struct ast_node *_copy_for_node(struct ast_node *orig_node)
{
    return for_node_new(
        orig_node->forloop->var, orig_node->forloop->range, 
        orig_node->forloop->body, orig_node->loc);
}

void _free_for_node(struct ast_node *node)
{
    if (node->forloop->var)
        node_free(node->forloop->var);
    if (node->forloop->range)
        node_free(node->forloop->range);
    if (node->forloop->body)
        node_free(node->forloop->body);
    ast_node_free(node);
}

struct ast_node *while_node_new(struct ast_node *expr, struct ast_node *body, struct source_location loc)
{
    struct ast_node *node = ast_node_new(WHILE_NODE, loc);
    MALLOC(node->whileloop, sizeof(*node->whileloop));
    node->whileloop->expr = expr;
    node->whileloop->body = body;
    return node;
}

struct ast_node *_copy_while_node(struct ast_node *orig_node)
{
    return while_node_new(
        node_copy(orig_node->whileloop->expr), node_copy(orig_node->whileloop->body), 
        orig_node->loc);
}

void _free_while_node(struct ast_node *node)
{
    node_free(node->whileloop->expr);
    node_free(node->whileloop->body);
    ast_node_free(node);
}

struct ast_node *jump_node_new(enum token_type token_type, struct ast_node *expr, struct source_location loc)
{
    struct ast_node *node = ast_node_new(JUMP_NODE, loc);
    MALLOC(node->jump, sizeof(*node->jump));
    node->jump->token_type = token_type;
    node->jump->expr = expr;
    node->jump->nested_block_levels = 0;
    return node;
}

struct ast_node *_copy_jump_node(struct ast_node *orig_node)
{
    return jump_node_new(
        orig_node->jump->token_type, node_copy(orig_node->jump->expr), 
        orig_node->loc);
}

void _free_jump_node(struct ast_node *node)
{
    node_free(node->jump->expr);
    ast_node_free(node);
}

struct ast_node *block_node_new(struct array *nodes)
{
    struct source_location loc = (nodes && array_size(nodes) > 0) ? (*(struct ast_node **)array_front(nodes))->loc : default_loc;
    struct ast_node *node = ast_node_new(BLOCK_NODE, loc);
    MALLOC(node->block, sizeof(*node->block));
    if(nodes)
        node->block->nodes = *nodes;
    return node;
}

struct ast_node *block_node_add(struct ast_node *block, struct ast_node *node)
{
    assert(block && block->node_type == BLOCK_NODE);
    if(node->node_type == BLOCK_NODE)
        return block_node_add_block(block, node);
    array_push(&block->block->nodes, &node);
    return block;
}

struct ast_node *block_node_add_block(struct ast_node *block, struct ast_node *node)
{
    for(u32 i = 0; i < array_size(&node->block->nodes); i++){
        struct ast_node *elem = *(struct ast_node **)array_get(&node->block->nodes, i);
        array_push(&block->block->nodes, &elem);
    }
    return block;
}

struct ast_node *node_copy(struct ast_node *node)
{
    if (!node) return node;
    switch (node->node_type) {
    case BLOCK_NODE:
        return _copy_block_node(node);
    case IMPORT_NODE:
        return _copy_import_node(node);
    case FUNC_TYPE_NODE:
        return _copy_func_type_node(node);
    case FUNC_NODE:
        return _copy_function_node(node);
    case VAR_NODE:
        return _copy_var_node(node);
    case STRUCT_NODE:
        return _copy_struct_node(node);
    case STRUCT_INIT_NODE:
        return _copy_struct_init_node(node);
    case IDENT_NODE:
        return _copy_ident_node(node);
    case LITERAL_NODE:
        return _copy_literal_node(node);
    case CALL_NODE:
        return _copy_call_node(node);
    case IF_NODE:
        return _copy_if_node(node);
    case FOR_NODE:
        return _copy_for_node(node);
    case UNARY_NODE:
        return _copy_unary_node(node);
    case BINARY_NODE:
        return _copy_binary_node(node);
    case ASSIGN_NODE:
        return _copy_assign_node(node);
    case RANGE_NODE:
        return _copy_range_node(node);
    case ARRAY_INIT_NODE:
        return _copy_array_init_node(node);
    case ARRAY_TYPE_NODE:
        return _copy_array_type_node(node);
    case TYPE_NODE:
        return _copy_type_node(node);
    case WHILE_NODE:
        return _copy_while_node(node);
    case JUMP_NODE:
        return _copy_jump_node(node);
    case CAST_NODE:
        return _copy_cast_node(node);
    case NULL_NODE:
    case UNIT_NODE:
    case MEMORY_NODE:
    case ENUM_NODE:
    case UNION_NODE:
    case MEMBER_INDEX_NODE:
    case TOTAL_NODE:
        break;
    }
    return 0;
}

void node_free(struct ast_node *node)
{
    if (!node) return;
    if(node->node_type > TOTAL_NODE){
        //hacked token node
        ast_node_free(node);
        return;
    }
    switch (node->node_type) {
    case NULL_NODE:
        FREE(node);
        break;
    case IMPORT_NODE:
        _free_import_node(node);
        break;
    case BLOCK_NODE:
        _free_block_node(node);
        break;
    case FUNC_TYPE_NODE:
        _free_func_type_node(node);
        break;
    case FUNC_NODE:
        _free_function_node(node);
        break;
    case VAR_NODE:
        _free_var_node(node);
        break;
    case STRUCT_NODE:
        _free_struct_node(node);
        break;
    case STRUCT_INIT_NODE:
        _free_struct_init_node(node);
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
    case IF_NODE:
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
    case ASSIGN_NODE:
        _free_assign_node(node);
        break;
    case RANGE_NODE:
        _free_range_node(node);
        break;
    case ARRAY_INIT_NODE:
        _free_array_init_node(node);
        break;
    case ARRAY_TYPE_NODE:
        _free_array_type_node(node);
        break;
    case TYPE_NODE:
        _free_type_node(node);
        break;
    case WHILE_NODE:
        _free_while_node(node);
        break;
    case JUMP_NODE:
        _free_jump_node(node);
        break;
    case CAST_NODE:
        _free_cast_node(node);
        break;
    case MEMORY_NODE:
        _free_memory_node(node);
        break;
    case MEMBER_INDEX_NODE:
        _free_member_index_node(node);
        break;
    case UNIT_NODE:
    case ENUM_NODE:
    case UNION_NODE:
    case TOTAL_NODE:
        ast_node_free(node);
        break;
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
    struct ast_node *node = block_node_new(0);
    array_init(&node->block->nodes, sizeof(struct ast_node *));
    mod->block = node;
    return mod;
}

int find_member_index(struct ast_node *type_node, symbol member)
{
    for (int i = 0; i < (int)array_size(&type_node->struct_def->body->block->nodes); i++) {
        struct ast_node *var = *(struct ast_node **)array_get(&type_node->struct_def->body->block->nodes, i);
        if (var->var->var->ident->name == member) {
            return i;
        }
    }
    return -1;
}

struct ast_node *block_node_new_empty()
{
    struct array nodes;
    array_init(&nodes, sizeof(struct ast_node *));
    return block_node_new(&nodes);
}

struct ast_node *wrap_expr_as_function(struct hashtable *symbol_2_int_types, struct ast_node *exp, symbol fn)
{
    struct array nodes;
    array_init(&nodes, sizeof(struct ast_node *));
    array_push(&nodes, &exp);
    struct ast_node *body_block = block_node_new(&nodes);
    return wrap_nodes_as_function(symbol_2_int_types, fn, body_block);
}

struct ast_node *wrap_nodes_as_function(struct hashtable *symbol_2_int_types, symbol func_name, struct ast_node *block)
{
    ARRAY_FUN_PARAM(fun_params);
    struct ast_node *params = block_node_new(&fun_params);
    struct ast_node *func_type = func_type_node_default_new(func_name, params, 0, 0, false, false, block->loc);
    hashtable_set_int(symbol_2_int_types, func_type->ft->name, TYPE_FUNCTION);
    return function_node_new(func_type, block, block->loc);
}

struct ast_node *get_root_object(struct ast_node *node)
{
    //for member index node, getting the root of index node
    while(node->node_type == MEMBER_INDEX_NODE)
        node = node->index->object;
    return node;
}

bool is_refered_later(struct ast_node *node)
{
    return node->node_type == IDENT_NODE;
}

void set_lvalue(struct ast_node *node)
{
    node->is_lvalue = true;
    while(node->node_type == MEMBER_INDEX_NODE){
        node = node->index->object;
        node->is_lvalue = true;
    }
}
