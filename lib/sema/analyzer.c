/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Type Inference Analyzer
 */
#include "sema/analyzer.h"
#include "clib/hash.h"
#include "clib/hashset.h"
#include "clib/hashtable.h"
#include "clib/symboltable.h"
#include "clib/util.h"
#include "tool/cmodule.h"
#include "error/error.h"
#include <assert.h>
#include <limits.h>

enum type _get_type_enum(struct sema_context *context, symbol type_name)
{
    return hashtable_get_int(context->symbol_2_int_types, type_name);
}

struct type_expr *retrieve_type_with_type_name(struct sema_context *context, symbol name)
{
    return get_symbol_type(&context->typename_2_typexps, &context->nongens, name);
}

struct type_expr *create_type_from_type_node(struct sema_context *context, struct ast_node *type_node)
{
    struct type_expr *value_type;
    if(type_node->node_type == IDENT_NODE){
        return retrieve_type_with_type_name(context, type_node->ident->name); // example like int
    } else if (type_node->node_type == ARRAY_TYPE_NODE){
        value_type = retrieve_type_with_type_name(context, type_node->array_type->elm_type->ident->name);
        struct array dims;
        array_init(&dims, sizeof(u32));
        for(u32 i=0; i<array_size(&type_node->array_type->dims->block->nodes); i++){
            struct ast_node *elm_size_node = *(struct ast_node **)array_get(&type_node->array_type->dims->block->nodes, i);
            u32 dim_size = elm_size_node->liter->int_val;
            array_push(&dims, &dim_size);
        }
        return create_array_type(value_type, &dims);
    } else if (type_node->node_type == UNARY_NODE){
        value_type = create_type_from_type_node(context, type_node->unop->operand);
        return create_ref_type(value_type);
    } else if (type_node->node_type == UNIT_NODE){
        return create_unit_type();
    }
    return 0;
}

struct type_expr *_analyze_unk(struct sema_context *context, struct ast_node *node)
{
    printf("analyzing unk: %s\n", node_type_strings[node->node_type]);
    if (!context || !node)
        return 0;
    return 0;
}

struct ast_node *_get_var_node(struct sema_context *context, struct ast_node *node)
{
    struct ast_node *var = 0;
   if (node->node_type == IDENT_NODE) {
        symbol var_name = node->ident->name;
        var = symboltable_get(&context->varname_2_asts, var_name);
   }
   return var;
}

struct type_expr *retrieve_type_for_var_name(struct sema_context *context, symbol name)
{
    return get_symbol_type(&context->decl_2_typexps, &context->nongens, name);
}

struct type_expr *_analyze_ident(struct sema_context *context, struct ast_node *node)
{
    node->ident->var = _get_var_node(context, node);
    if(!node->ident->var){
        report_error(context, EC_IDENT_NOT_DEFINED, node->loc);
        return 0;
    }
    return retrieve_type_for_var_name(context, node->ident->name);
}

struct type_expr *_analyze_liter(struct sema_context *context, struct ast_node *node)
{
    symbol type_name = get_type_symbol(node->liter->type);
    return retrieve_type_with_type_name(context, type_name);
}

struct type_expr *_analyze_var(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *var_type = 0;
    if(context->scope_level == 1){
        //global variable, test JIT directly evaluates global variable
        node->var->is_global = true;
    }
    assert(node->var->is_of_type || node->var->init_value);
    if (node->var->is_global){
        hashtable_set_p(&context->gvar_name_2_ast, node->var->var_name, node);
    }
    if (node->var->is_of_type){
        var_type = create_type_from_type_node(context, node->var->is_of_type);
        assert(var_type);
        if(hashtable_in_p(&context->struct_typename_2_asts, var_type->name)||
            !node->var->init_value){
            push_symbol_type(&context->decl_2_typexps, node->var->var_name, var_type);
            push_symbol_type(&context->varname_2_asts, node->var->var_name, node);
            if (node->var->init_value)
                analyze(context, node->var->init_value);
            return var_type;
        }
    }
    if (!var_type && has_symbol_in_scope(&context->decl_2_typexps, node->var->var_name, context->scope_marker))
        var_type = retrieve_type_for_var_name(context, node->var->var_name);
    if(!var_type)
        var_type = create_type_var();
    struct type_expr *type = analyze(context, node->var->init_value);
    if (!type)
        return 0;
    bool unified = unify(var_type, type, &context->nongens);
    if (!unified) {
        report_error(context, EC_VAR_TYPE_NO_MATCH_LITERAL, node->loc);
        return 0;
    }
    push_symbol_type(&context->decl_2_typexps, node->var->var_name, var_type);
    push_symbol_type(&context->varname_2_asts, node->var->var_name, node);
    return var_type;
}

struct type_expr *_analyze_struct(struct sema_context *context, struct ast_node *node)
{
    struct array args;
    array_init(&args, sizeof(struct type_expr *));
    analyze(context, node->struct_def->body);
    for (size_t i = 0; i < array_size(&node->struct_def->body->block->nodes); i++) {
        //printf("creating type: %zu\n", i);
        struct ast_node *field_node = *(struct ast_node **)array_get(&node->struct_def->body->block->nodes, i);
        array_push(&args, &field_node->type);
    }
    struct type_expr *result_type = create_type_oper_struct(node->struct_def->name, &args);
    assert(node->struct_def->name == result_type->name);
    struct type_expr *ref_type = create_ref_type(result_type);
    push_symbol_type(&context->typename_2_typexps, node->struct_def->name, result_type);
    push_symbol_type(&context->typename_2_typexps, ref_type->name, ref_type);
    hashtable_set_p(&context->struct_typename_2_asts, node->struct_def->name, node);
    return result_type;
}

struct type_expr *_analyze_struct_init(struct sema_context *context, struct ast_node *node)
{
    if (node->struct_init->is_of_type)
        node->type = create_type_from_type_node(context, node->struct_init->is_of_type);
    for (size_t i = 0; i < array_size(&node->struct_init->body->block->nodes); i++) {
        //printf("creating type: %zu\n", i);
        analyze(context, *(struct ast_node **)array_get(&node->struct_init->body->block->nodes, i));
    }
    return node->type;
}

struct type_expr *_analyze_list_comp(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *type = 0;
    struct type_expr *element_type = 0;
    struct array dims;
    array_init(&dims, sizeof(u32));
    if (!node->list_comp){
        element_type = create_unit_type();
        type = create_array_type(element_type, &dims);
    }else if(node->list_comp->node_type == BLOCK_NODE){
        u32 size = array_size(&node->list_comp->block->nodes);
        array_push(&dims, &size);
        if(size){
            for(u32 i = 0; i < array_size(&node->list_comp->block->nodes); i++){
                struct ast_node *element = *(struct ast_node **)array_get(&node->list_comp->block->nodes, i);
                element_type = analyze(context, element);
            }
        }
        type = create_array_type(element_type, &dims);
    }
    //we can't deinit dims, the memory is owned by type->dims
    return type;
}

struct type_expr *_analyze_array_type(struct sema_context *context, struct ast_node *node)
{
    symbol elm_type_name = node->array_type->elm_type->ident->name;
    enum type elm_type_enum = get_type_enum_from_symbol(elm_type_name);
    struct type_expr *elm_type = create_nullary_type(elm_type_enum, elm_type_name);
    struct array dims;
    array_init(&dims, sizeof(u32));
    for(u32 i = 0; i < array_size(&node->list_comp->block->nodes); i++){
        struct ast_node *dim_node = *(struct ast_node **)array_get(&node->list_comp->block->nodes, i);
        u32 dim = dim_node->liter->int_val;
        array_push(&dims, &dim);
    }
    return create_array_type(elm_type, &dims);
}

struct type_expr *_analyze_func_type(struct sema_context *context, struct ast_node *node)
{
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_expr *));
    for (size_t i = 0; i < array_size(&node->ft->params->block->nodes); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&node->ft->params->block->nodes, i);
        assert(param->var->is_of_type);
        struct type_expr* to = create_type_from_type_node(context, param->var->is_of_type);
        param->type = to;
        array_push(&fun_sig, &param->type);
    }
    assert(node->ft->is_of_ret_type_node);
    struct type_expr *to = create_type_from_type_node(context, node->ft->is_of_ret_type_node);
    array_push(&fun_sig, &to);
    node->type = create_type_fun(&fun_sig);
    hashtable_set_p(&context->func_types, node->ft->name, node);
    push_symbol_type(&context->decl_2_typexps, node->ft->name, node->type);
    return node->type;
}

struct type_expr *_analyze_func(struct sema_context *context, struct ast_node *node)
{
    hashtable_set_p(&context->func_types, node->func->func_type->ft->name, node->func->func_type);
    stack_push(&context->func_stack, &node);
    //# create a new non-generic variable for the binder
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_expr *));
    for (size_t i = 0; i < array_size(&node->func->func_type->ft->params->block->nodes); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&node->func->func_type->ft->params->block->nodes, i);
        struct type_expr *exp;
        if (param->var->is_of_type) {
           exp = create_type_from_type_node(context, param->var->is_of_type);
        } else
            exp = create_type_var();
        array_push(&fun_sig, &exp);
        array_push(&context->nongens, &exp);
        push_symbol_type(&context->decl_2_typexps, param->var->var_name, exp);
        push_symbol_type(&context->varname_2_asts, param->var->var_name, param);
    }
    /*analyze function body*/
    struct type_expr *fun_type = create_type_var();
    push_symbol_type(&context->decl_2_typexps, node->func->func_type->ft->name, fun_type);
    struct type_expr *ret_type = analyze(context, node->func->body);
    array_push(&fun_sig, &ret_type);
    struct type_expr *result_type = create_type_fun(&fun_sig);
    unify(fun_type, result_type, &context->nongens);
    struct type_expr *result = prune(fun_type);
    node->func->func_type->type = result;
    if (is_generic(result)) {
        hashtable_set(&context->generic_ast, string_get(node->func->func_type->ft->name), node);
    }
    struct ast_node *saved_node = *(struct ast_node **)stack_pop(&context->func_stack);
    (void)saved_node;
    assert(node == saved_node);
    return result;
}

struct type_expr *_analyze_call(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *fun_type = retrieve_type_for_var_name(context, node->call->callee);
    if (!fun_type) {
        struct source_location loc = { 1, 1, 0, 0 };
        report_error(context, EC_FUNC_NOT_DEFINED, loc);
        return 0;
    }
    struct array args;
    array_init(&args, sizeof(struct type_expr *));
    for (size_t i = 0; i < array_size(&node->call->arg_block->block->nodes); i++) {
        struct ast_node *arg = *(struct ast_node **)array_get(&node->call->arg_block->block->nodes, i);
        struct type_expr *type = analyze(context, arg);
        assert(type);
        array_push(&args, &type);
    }
    struct ast_node *sp_fun = 0;
    /* monomorphization of generic */
    if (is_generic(fun_type) && (!is_any_generic(&args) && array_size(&args))) {
        struct ast_node *parent_func = stack_size(&context->func_stack) ? *(struct ast_node**)stack_top(&context->func_stack) : 0;
        if (!parent_func || (parent_func->func->func_type->ft->name != node->call->callee)){
            string sp_callee = monomorphize(string_get(node->call->callee), &args);
            node->call->specialized_callee = to_symbol(string_get(&sp_callee));
            if (has_symbol(&context->decl_2_typexps, node->call->specialized_callee)) {
                fun_type = retrieve_type_for_var_name(context, node->call->specialized_callee);
                return *(struct type_expr **)array_back(&fun_type->args);
            }
            /* specialized callee */
            struct ast_node *generic_fun = hashtable_get(&context->generic_ast, string_get(node->call->callee));
            sp_fun = node_copy(generic_fun);
            array_push(&generic_fun->func->sp_funs, &sp_fun);

            sp_fun->func->func_type->ft->name = node->call->specialized_callee;
            fun_type = analyze(context, sp_fun);
            hashtable_set(&context->specialized_ast, string_get(sp_fun->func->func_type->ft->name), sp_fun);
            array_push(&context->new_specialized_asts, &sp_fun);     
            push_symbol_type(&context->decl_2_typexps, node->call->specialized_callee, fun_type);
            hashtable_set_p(&context->func_types, node->call->specialized_callee, sp_fun->func->func_type);
            hashtable_set_p(&context->calls, node->call->specialized_callee, node);
            node->call->callee_func_type = sp_fun->func->func_type;
        }
    }
    struct type_expr *result_type = create_type_var();
    array_push(&args, &result_type);
    struct type_expr *call_fun = create_type_fun(&args);
    unify(call_fun, fun_type, &context->nongens);
    if(sp_fun){
        fun_type = prune(fun_type);
        sp_fun->type = fun_type;
        sp_fun->func->func_type->type = fun_type;
    }
    if (hashtable_in_p(&context->builtin_ast, node->call->callee)) {
        array_push(&context->used_builtin_names, &node->call->callee);
    }
    if (!node->call->specialized_callee) {
        hashtable_set_p(&context->calls, node->call->callee, node);
        node->call->callee_func_type = hashtable_get_p(&context->func_types, node->call->callee);
    }
    return result_type;
}

struct type_expr *_analyze_unary(struct sema_context *context, struct ast_node *node)
{
    struct ast_node *var = 0;
    struct type_expr *op_type = analyze(context, node->unop->operand);
    if(!op_type) return 0;
    if (node->unop->opcode == OP_NOT) {
        struct type_expr *bool_type = create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
        unify(op_type, bool_type, &context->nongens);
        node->unop->operand->type = op_type;
    }
    else if(node->unop->opcode == OP_BAND){ //'&'
        if(op_type->type == TYPE_REF){
            report_error(context, EC_NOT_VALUE_TYPE, node->loc);
            return 0;
        }
        //reference-of or address-of operator
        if(node->unop->operand->node_type == IDENT_NODE){
            var = _get_var_node(context, node->unop->operand);
            if(var) var->is_addressed = true;
        }
        node->unop->operand->is_addressed = true;
        op_type = create_ref_type(op_type);
    }
    else if(node->unop->opcode == OP_STAR){
        //dereference-of
        if(op_type->type != TYPE_REF){
            report_error(context, EC_NOT_REFERENCE_TYPE, node->loc);
            return 0;
        }
        op_type = node->unop->operand->type->val_type;
    }
    return op_type;
}

struct type_expr *_analyze_struct_field_accessor(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *type = analyze(context, node->index->object);
    if(type->type != TYPE_STRUCT && !(type->type == TYPE_REF && type->val_type->type == TYPE_STRUCT)){
        report_error(context, EC_EXPECT_STRUCT_TYPE, node->loc);
        return 0;
    }
    struct type_expr *struct_type = type->val_type ? type->val_type : type;
    struct ast_node *type_node = hashtable_get_p(&context->struct_typename_2_asts, struct_type->name);
    int index = find_member_index(type_node, node->index->index->ident->name);
    if (index < 0) {
        report_error(context, EC_FIELD_NOT_EXISTS, node->loc);
        return 0;
    }
    struct type_expr *member_type = *(struct type_expr **)array_get(&struct_type->args, index);
    node->index->index->type = member_type;
    return member_type;
}

struct type_expr *_analyze_array_member_accessor(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *type = analyze(context, node->index->object);
    analyze(context, node->index->index);
    if(type->type != TYPE_ARRAY && !(type->type == TYPE_REF && type->val_type->type == TYPE_ARRAY)){
        report_error(context, EC_EXPECT_ARRAY_TYPE, node->loc);
        return 0;
    }
    return type->val_type;
}

struct type_expr *_analyze_binary(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *lhs_type = analyze(context, node->binop->lhs);
    struct type_expr *rhs_type = analyze(context, node->binop->rhs);
    struct type_expr *result = 0;
    if (unify(lhs_type, rhs_type, &context->nongens)) {
        if (is_relational_op(node->binop->opcode))
            result = create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
        else
            result = lhs_type;
        return result;
    } else {
        report_error(context, EC_TYPES_DO_NOT_MATCH, node->loc);
    }
    return result;
}

struct type_expr *_analyze_assign(struct sema_context *context, struct ast_node *node)
{
    if(!node->binop->lhs->is_addressable){
        report_error(context, EC_NOT_ASSIGNABLE, node->loc);
        return 0;
    }
    set_lvalue(node->binop->lhs);
    struct type_expr *lhs_type = analyze(context, node->binop->lhs);
    struct type_expr *rhs_type = analyze(context, node->binop->rhs);
    struct type_expr *result = 0;
    if (unify(lhs_type, rhs_type, &context->nongens)) {
        return create_unit_type();
    } else {
        report_error(context, EC_TYPES_DO_NOT_MATCH, node->loc);
    }
    return result;
}

struct type_expr *_analyze_if(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *cond_type = analyze(context, node->cond->if_node);
    struct type_expr *bool_type = create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
    unify(cond_type, bool_type, &context->nongens);
    struct type_expr *then_type = analyze(context, node->cond->then_node);
    struct type_expr *else_type = analyze(context, node->cond->else_node);
    unify(then_type, else_type, &context->nongens);
    return then_type;
}

struct type_expr *_analyze_for(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *bool_type = create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
    node->forloop->var->var->init_value = node->forloop->range->range->start;
    struct type_expr *var_type = analyze(context, node->forloop->var);
    struct type_expr *start_type = analyze(context, node->forloop->range->range->start);
    struct type_expr *step_type = 0;
    if(node->forloop->range->range->step)
        step_type = analyze(context, node->forloop->range->range->step);
    struct type_expr *end_type = analyze(context, node->forloop->range->range->end);
    struct type_expr *body_type = analyze(context, node->forloop->body);
    if (!unify(start_type, var_type, &context->nongens)) {
        printf("failed to unify start type as int: %s, %s\n", kind_strings[start_type->kind], node_type_strings[node->forloop->range->range->start->node_type]);
    }
    if(step_type){
        unify(step_type, var_type, &context->nongens);
    }
    unify(end_type, bool_type, &context->nongens);
    if(!node->forloop->range->range->step){
        node->forloop->range->range->step = const_one_node_new(var_type->type, node->loc);
        step_type = analyze(context, node->forloop->range->range->step);
    }
    node->forloop->range->range->start->type = start_type;
    node->forloop->range->range->step->type = step_type;
    node->forloop->range->range->end->type = end_type;
    node->forloop->body->type = body_type;
    return create_nullary_type(TYPE_UNIT, get_type_symbol(TYPE_UNIT));
}

struct type_expr *_analyze_block(struct sema_context *context, struct ast_node *node)
{
    enter_scope(context);
    struct type_expr *type = 0;
    for (size_t i = 0; i < array_size(&node->block->nodes); i++) {
        struct ast_node *n = *(struct ast_node **)array_get(&node->block->nodes, i);
        type = analyze(context, n);
    }
    //tag variable node as returning variable if exists
    struct ast_node *ret_node = *(struct ast_node **)array_back(&node->block->nodes);
    ret_node->is_ret = true;
    struct ast_node *var = _get_var_node(context, ret_node);
    if(var)
        var->is_ret = true;
    leave_scope(context);
    return type;
}

struct type_expr *analyze(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *type = 0;
    if (node->type && node->type->kind == KIND_OPER)
        return node->type;
    switch(node->node_type){
        case ENUM_NODE:
        case UNION_NODE:
        case TOTAL_NODE:
        case NULL_NODE:
            //type = _analyze_unk(context, node);
            assert(false);
            break;
        case RANGE_NODE:
            break;
        case LIST_COMP_NODE:
            type = _analyze_list_comp(context, node);
            break;
        case ARRAY_TYPE_NODE:
            type = _analyze_array_type(context, node);
            break;
        case UNIT_NODE:
            type = create_unit_type();
            break;
        case IMPORT_NODE:
            type = analyze(context, node->import->import);
            //type = &UNIT_TYPE->base;
            break;
        case MEMORY_NODE:
            type = analyze(context, node->memory->initial);
            if (node->memory->max)
                type = analyze(context, node->memory->max);
            break;
        case LITERAL_NODE:
            type = _analyze_liter(context, node);
            break;
        case IDENT_NODE:
            type = _analyze_ident(context, node);
            break;
        case VAR_NODE:
            type = _analyze_var(context, node);
            break;
        case STRUCT_NODE:
            type = _analyze_struct(context, node);
            break;
        case STRUCT_INIT_NODE:
            type = _analyze_struct_init(context, node);
            break;
        case UNARY_NODE:
            type = _analyze_unary(context, node);
            break;
        case MEMBER_INDEX_NODE:
            if(node->index->aggregate_type == AGGREGATE_TYPE_ARRAY)
                type = _analyze_array_member_accessor(context, node);
            else if(node->index->aggregate_type == AGGREGATE_TYPE_STRUCT)
                type = _analyze_struct_field_accessor(context, node);
            break;
        case BINARY_NODE:
            if(is_assign(node->binop->opcode)){
                type = _analyze_assign(context, node);
            }else{
                type = _analyze_binary(context, node);
            }
            break;
        case IF_NODE:
            type = _analyze_if(context, node);
            break;
        case FOR_NODE:
            type = _analyze_for(context, node);
            break;
        case CALL_NODE:
            type = _analyze_call(context, node);
            break;
        case FUNC_TYPE_NODE:
            type = _analyze_func_type(context, node);
            break;
        case FUNC_NODE:
            type = _analyze_func(context, node);
            break;
        case BLOCK_NODE:
            type = _analyze_block(context, node);
            break;
    }
    type = prune(type);
    node->type = type;
    return type;
}
