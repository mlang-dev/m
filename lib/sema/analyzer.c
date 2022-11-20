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
#include "sema/eval.h"
#include "tool/cmodule.h"
#include "error/error.h"
#include "parser/astdump.h"
#include <assert.h>
#include <limits.h>

enum type _get_type_enum(struct sema_context *context, symbol type_name)
{
    return hashtable_get_int(context->symbol_2_int_types, type_name);
}

struct type_expr *_retrieve_type_with_type_name(struct sema_context *context, symbol name, enum Mut mut)
{
    struct type_expr_pair * tep = symboltable_get(&context->typename_2_typexpr_pairs, name);
    if (!tep){
        printf("No type is found for the symbol: %s.\n", string_get(name));
        return 0;
    }
    struct type_expr *type = tep_find_type_expr(tep, mut, false, mut);
    return fresh(type, &context->nongens);
}

struct type_expr *create_type_from_type_node(struct sema_context *context, struct type_node *type_node, enum Mut mut)
{
    struct type_expr *value_type;
    switch(type_node->kind){
    case TypeName:
        return _retrieve_type_with_type_name(context, type_node->type_name, mut); // example like int
    case ArrayType:
    {
        value_type = _retrieve_type_with_type_name(context, type_node->array_type_node->elm_type->type_node->type_name, mut);
        struct array dims;
        array_init(&dims, sizeof(u32));
        for(u32 i=0; i<array_size(&type_node->array_type_node->dims->block->nodes); i++){
            struct ast_node *elm_size_node = *(struct ast_node **)array_get(&type_node->array_type_node->dims->block->nodes, i);
            u32 dim_size = eval(elm_size_node);
            array_push(&dims, &dim_size);
        }
        return create_array_type(value_type, &dims);
    }
    case UnitType:
        return create_unit_type();
    case RefType:
        value_type = create_type_from_type_node(context, type_node->val_node, mut);
        return create_ref_type(value_type, Immutable);
    }
}

struct ast_node *cast_to_node(struct type_expr *to_type, struct ast_node *node)
{
    struct ast_node *type_name = ident_node_new(get_type_symbol(to_type->type), node->loc);
    //we have to make a copy of init_value node, otherwise it's endlessly calling cast node
    //TODO: remove the copy
    struct ast_node *copy_node = node_copy(node);
    copy_node->type = node->type;
    struct ast_node *cast = cast_node_new(type_name, copy_node, node->loc);
    cast->type = to_type;
    return cast;
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
    return get_symbol_type(&context->decl_2_typexprs, &context->nongens, name);
}

struct type_expr *_analyze_ident(struct sema_context *context, struct ast_node *node)
{
    node->ident->var = _get_var_node(context, node);
    if(!node->ident->var){
        report_error(context, EC_IDENT_NOT_DEFINED, node->loc, string_get(node->ident->name));
        return 0;
    }
    return retrieve_type_for_var_name(context, node->ident->name);
}

struct type_expr *_analyze_liter(struct sema_context *context, struct ast_node *node)
{
    symbol type_name = get_type_symbol(node->liter->type);
    return _retrieve_type_with_type_name(context, type_name, Immutable);
}

u32 _get_array_size(struct type_expr *type)
{
    u32 size = 1;
    if(type->type != TYPE_ARRAY)
        return size;
    for(u32 i=0; i<array_size(&type->dims); i++){
        size *= *(u32*)array_get(&type->dims, i);
    }
    return size * _get_array_size(type->val_type);
}

bool _is_array_size_same(struct type_expr *type1, struct type_expr *type2)
{
    return type1->type == type2->type && type2->type == TYPE_ARRAY && _get_array_size(type1) == _get_array_size(type2);
}


struct ast_node *_node_copy_with_type(struct ast_node *node)
{
    struct ast_node *copy = node_copy(node);
    copy->type = node->type;
    return copy;
}

struct type_expr *_analyze_var(struct sema_context *context, struct ast_node *node)
{
    symbol var_name = node->var->var->ident->name;
    if(has_symbol(&context->decl_2_typexprs, var_name)){
        //this is assignment, not var declaration
        //check if it's mutable
        struct ast_node *orig_var = symboltable_get(&context->varname_2_asts, var_name);
        if(orig_var->var->mut == Immutable){
            report_error(context, EC_IMMUTABLE_ASSIGNMENT, node->loc, string_get(var_name));
            return 0;
        }
        //
        node->transformed = assign_node_new(OP_ASSIGN, _node_copy_with_type(node->var->var), _node_copy_with_type(node->var->init_value), node->loc);
        //moved 
        node->transformed->type = analyze(context, node->transformed);
        return node->transformed->type;
    }
    struct type_expr *var_type = 0;
    if(context->scope_level == 1){
        //global variable, test JIT directly evaluates global variable
        node->var->is_global = true;
    }
    assert(node->var->is_of_type || node->var->init_value);
    if (node->var->is_global){
        hashtable_set_p(&context->gvar_name_2_ast, var_name, node);
    }
    if (node->var->is_of_type){
        enum Mut mut = Immutable;
        if(node->var->mut == Mutable || node->var->is_of_type->type_node->mut == Mutable){
            mut = Mutable;
        }
        var_type = create_type_from_type_node(context, node->var->is_of_type->type_node, mut);
        assert(var_type);
        if(hashtable_in_p(&context->struct_typename_2_asts, var_type->name)||
            !node->var->init_value){
            push_symbol_type(&context->decl_2_typexprs, var_name, var_type);
            push_symbol_type(&context->varname_2_asts, var_name, node);
            if (node->var->init_value)
                analyze(context, node->var->init_value);
            return var_type;
        }
    }
    if (!var_type && has_symbol_in_scope(&context->decl_2_typexprs, var_name, context->scope_marker))
        var_type = retrieve_type_for_var_name(context, var_name);
    struct type_expr *type = analyze(context, node->var->init_value);
    if (type && var_type && _is_array_size_same(var_type, type)){
        type = var_type;
        node->var->init_value->type = type;
    }
    if (!type)
        return 0;
    if(!var_type)
        var_type = create_type_var(node->var->mut);
    struct type_expr *result_type = unify(var_type, type, &context->nongens);
    if (!result_type) {
        report_error(context, EC_VAR_TYPE_NO_MATCH_LITERAL, node->loc);
        return 0;
    }
    if(node->var->is_of_type && result_type->type != var_type->type && node->var->init_value){
        node->var->init_value->transformed = cast_to_node(var_type, node->var->init_value);
    }
    
    push_symbol_type(&context->decl_2_typexprs, var_name, var_type);
    push_symbol_type(&context->varname_2_asts, var_name, node);
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
    symbol struct_name = node->struct_def->name;
    struct type_expr *result_type = create_type_oper_struct(struct_name, Immutable, &args);
    assert(node->struct_def->name == result_type->name);
    struct type_expr_pair * tep = get_type_expr_pair(struct_name);
    push_symbol_type(&context->typename_2_typexpr_pairs, struct_name, tep);
    hashtable_set_p(&context->struct_typename_2_asts, node->struct_def->name, node);
    return result_type;
}

struct type_expr *_analyze_struct_init(struct sema_context *context, struct ast_node *node)
{
    if (node->struct_init->is_of_type){
        node->type = create_type_from_type_node(context, node->struct_init->is_of_type->type_node, Immutable);
    }
    for (size_t i = 0; i < array_size(&node->struct_init->body->block->nodes); i++) {
        //printf("creating type: %zu\n", i);
        analyze(context, *(struct ast_node **)array_get(&node->struct_init->body->block->nodes, i));
    }
    return node->type;
}

struct type_expr *_analyze_array_init(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *type = 0;
    struct type_expr *element_type = 0;
    struct array dims;
    array_init(&dims, sizeof(u32));
    if (!node->array_init){
        element_type = create_unit_type();
        type = create_array_type(element_type, &dims);
    }else if(node->array_init->node_type == BLOCK_NODE){
        u32 size = array_size(&node->array_init->block->nodes);
        array_push(&dims, &size);
        if(size){
            for(u32 i = 0; i < array_size(&node->array_init->block->nodes); i++){
                struct ast_node *element = *(struct ast_node **)array_get(&node->array_init->block->nodes, i);
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
    struct type_expr *elm_type = create_nullary_type(elm_type_enum);
    struct array dims;
    array_init(&dims, sizeof(u32));
    for(u32 i = 0; i < array_size(&node->array_init->block->nodes); i++){
        struct ast_node *dim_node = *(struct ast_node **)array_get(&node->array_init->block->nodes, i);
        u32 dim = eval(dim_node);
        array_push(&dims, &dim);
    }
    return create_array_type(elm_type, &dims);
}

struct type_expr *_analyze_type_node(struct sema_context *context, struct ast_node *node)
{
    return 0;
}

struct type_expr *_analyze_func_type(struct sema_context *context, struct ast_node *node)
{
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_expr *));
    for (size_t i = 0; i < array_size(&node->ft->params->block->nodes); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&node->ft->params->block->nodes, i);
        assert(param->var->is_of_type);
        struct type_expr* to = create_type_from_type_node(context, param->var->is_of_type->type_node, Immutable);
        param->type = to;
        array_push(&fun_sig, &param->type);
    }
    assert(node->ft->ret_type_node);
    struct type_expr *to = create_type_from_type_node(context, node->ft->ret_type_node->type_node, Immutable);
    array_push(&fun_sig, &to);
    node->type = create_type_fun(&fun_sig);
    hashtable_set_p(&context->func_types, node->ft->name, node);
    push_symbol_type(&context->decl_2_typexprs, node->ft->name, node->type);
    return node->type;
}

struct type_expr *_analyze_func(struct sema_context *context, struct ast_node *node)
{
    enter_function(context);
    hashtable_set_p(&context->func_types, node->func->func_type->ft->name, node->func->func_type);
    stack_push(&context->func_stack, &node);
    //# create a new non-generic variable for the binder
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_expr *));
    for (size_t i = 0; i < array_size(&node->func->func_type->ft->params->block->nodes); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&node->func->func_type->ft->params->block->nodes, i);
        struct type_expr *exp;
        if (param->var->is_of_type) {
           exp = create_type_from_type_node(context, param->var->is_of_type->type_node, Immutable);
        } else{
            exp = create_type_var(param->var->mut);
        }
        array_push(&fun_sig, &exp);
        array_push(&context->nongens, &exp);
        push_symbol_type(&context->decl_2_typexprs, param->var->var->ident->name, exp);
        push_symbol_type(&context->varname_2_asts, param->var->var->ident->name, param);
    }
    /*analyze function body*/
    struct type_expr *fun_type_var = create_type_var(Immutable); //?
    push_symbol_type(&context->decl_2_typexprs, node->func->func_type->ft->name, fun_type_var);
    struct type_expr *ret_type = analyze(context, node->func->body);
    array_push(&fun_sig, &ret_type);
    struct type_expr *result_type = create_type_fun(&fun_sig);
    unify(fun_type_var, result_type, &context->nongens);
    struct type_expr *result = prune(fun_type_var);
    node->func->func_type->type = result;
    if (is_generic(result)) {
        hashtable_set(&context->generic_ast, string_get(node->func->func_type->ft->name), node);
    }
    struct ast_node *saved_node = *(struct ast_node **)stack_pop(&context->func_stack);
    (void)saved_node;
    assert(node == saved_node);
    leave_function(context);
    //free type variable
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
            if (has_symbol(&context->decl_2_typexprs, node->call->specialized_callee)) {
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
            push_symbol_type(&context->decl_2_typexprs, node->call->specialized_callee, fun_type);
            hashtable_set_p(&context->func_types, node->call->specialized_callee, sp_fun->func->func_type);
            hashtable_set_p(&context->calls, node->call->specialized_callee, node);
            node->call->callee_func_type = sp_fun->func->func_type;
        }
    }
    struct type_expr *result_type = create_type_var(Immutable); //?? immutable result assumed
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
    struct type_expr *op_type = analyze(context, node->unop->operand);
    if(!op_type) return 0;
    if (node->unop->opcode == OP_NOT) {
        struct type_expr *bool_type = create_nullary_type(TYPE_BOOL);
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
            node->unop->operand->ident->var->is_addressed = true;
        }
        node->unop->operand->is_addressed = true;
        op_type = create_ref_type(op_type, Immutable);
    }
    else if(node->unop->opcode == OP_STAR){
        //dereference-of
        if(op_type->type != TYPE_REF){
            report_error(context, EC_NOT_REFERENCE_TYPE, node->loc);
            return 0;
        }
        op_type = node->unop->operand->type->val_type;
    }
    else if(node->unop->opcode == OP_INC || node->unop->opcode == OP_DEC){
        struct ast_node *new_node = binary_node_new(node->unop->opcode == OP_INC ? OP_PLUS : OP_MINUS, _node_copy_with_type(node->unop->operand), int_node_new(1, node->loc), node->loc);
        node->transformed = assign_node_new(OP_ASSIGN, _node_copy_with_type(node->unop->operand), new_node, node->loc);
        node->transformed->type = analyze(context, node->transformed);
    }
    return op_type;
}

struct type_expr *_analyze_cast(struct sema_context *context, struct ast_node *node)
{
    analyze(context, node->cast->expr);
    return create_type_from_type_node(context, node->cast->to_type_node->type_node, Immutable);
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
    node->index->index->ident->var = *(struct ast_node **)array_get(&type_node->struct_def->body->block->nodes, index);    
    return member_type;
}

struct type_expr *_analyze_array_member_accessor(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *type = analyze(context, node->index->object);
    if(!type) return 0;
    analyze(context, node->index->index);
    if(type->type != TYPE_ARRAY && !(type->type == TYPE_REF && type->val_type->type == TYPE_ARRAY)){
        report_error(context, EC_EXPECT_ARRAY_TYPE, node->loc);
        return 0;
    }
    if(array_size(&type->dims) == 1){
        return find_type_expr(type->val_type, type->mut);
    }else{
        //remove one dimension
        struct array dims;
        array_init(&dims, sizeof(u32));
        for(u32 i = 1; i < array_size(&type->dims); i++){
            u32 size = *(u32*)array_get(&type->dims, i);
            array_push(&dims, &size);
        }
        return create_array_type(type->val_type, &dims);
    }
}

struct type_expr *_analyze_binary(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *lhs_type = analyze(context, node->binop->lhs);
    struct type_expr *rhs_type = analyze(context, node->binop->rhs);
    if(!lhs_type || !rhs_type) return 0;
    struct type_expr *result_type = unify(lhs_type, rhs_type, &context->nongens);
    if (result_type) {
        if (is_relational_op(node->binop->opcode))
            result_type = create_nullary_type(TYPE_BOOL);
        else{    
            lhs_type = prune(lhs_type);
            rhs_type = prune(rhs_type);
            result_type = prune(result_type);
            if(lhs_type != result_type){
                node->binop->lhs->transformed = cast_to_node(result_type, node->binop->lhs);
            }
            if(rhs_type != result_type){
                node->binop->rhs->transformed =  cast_to_node(result_type, node->binop->rhs);
            }
        }
        return result_type;
    } else {
        report_error(context, EC_TYPES_DO_NOT_MATCH, node->loc);
    }
    return result_type;
}

struct type_expr *_analyze_assign(struct sema_context *context, struct ast_node *node)
{
    set_lvalue(node->binop->lhs);
    if(node->binop->lhs->node_type == IDENT_NODE){
        symbol var_name = node->binop->lhs->ident->name;
        if(!has_symbol(&context->decl_2_typexprs, var_name)){
            //first time assignment, immutable binding
            //this is only possible when grammar is changed to "id = expr" not 
            //as var decl. 
            assert(false);
            node->transformed = var_node_new(_node_copy_with_type(node->binop->lhs), 0, _node_copy_with_type(node->binop->rhs), false, false, node->loc);
            node->transformed->type = analyze(context, node->transformed);
            return node->transformed->type;
        }else{
            struct ast_node *orig_var = symboltable_get(&context->varname_2_asts, var_name);
            if(orig_var->var->mut == Immutable){
                report_error(context, EC_IMMUTABLE_ASSIGNMENT, node->loc, string_get(var_name));
                return 0;
                
            }
        }
    }
    struct type_expr *lhs_type = analyze(context, node->binop->lhs);
    if(lhs_type && !lhs_type->mut){
        string lhs_str = dump(context, node->binop->lhs);
        report_error(context, EC_IMMUTABLE_ASSIGNMENT, node->loc, string_get(&lhs_str));
        return 0;
    }  
    struct type_expr *rhs_type = analyze(context, node->binop->rhs);
    struct type_expr *result = 0;
    struct type_expr *unified = unify(lhs_type, rhs_type, &context->nongens);
    if (unified == lhs_type) {
        result = create_unit_type();
    } else {
        if(is_int_type(lhs_type->type) && is_int_type(rhs_type->type)){
            node->binop->rhs->type = lhs_type;
        }else{
            report_error(context, EC_TYPES_DO_NOT_MATCH, node->loc);
        }
    }
    if(is_assign_op_sugar(node->binop->opcode)){
        enum op_code binop = get_op_code_from_assign_op(node->binop->opcode);
        struct ast_node *new_node = binary_node_new(binop, _node_copy_with_type(node->binop->lhs), _node_copy_with_type(node->binop->rhs), node->loc);
        node->transformed = assign_node_new(OP_ASSIGN, _node_copy_with_type(node->binop->lhs), new_node, node->loc);
        node->transformed->type = result;
    }
    return result;
}

struct type_expr *_analyze_if(struct sema_context *context, struct ast_node *node)
{
    struct loop_nested_level *lnl = get_current_block_level(context);
    if(lnl) lnl->block_levels++;
    struct type_expr *cond_type = analyze(context, node->cond->if_node);
    struct type_expr *bool_type = create_nullary_type(TYPE_BOOL);
    unify(cond_type, bool_type, &context->nongens);
    struct type_expr *then_type = analyze(context, node->cond->then_node);
    if(node->cond->else_node){
        struct type_expr *else_type = analyze(context, node->cond->else_node);
        unify(then_type, else_type, &context->nongens);
    }
    if(lnl) lnl->block_levels--;
    return then_type;
}

struct type_expr *_analyze_for(struct sema_context *context, struct ast_node *node)
{
    struct loop_nested_level *bnl = enter_loop(context);
    bnl->block_levels = 1; //body is itself a block
    struct type_expr *bool_type = create_nullary_type(TYPE_BOOL);
    //now we need to know who owns the start node memory
    node->forloop->var->var->init_value = node->forloop->range->range->start;
    node->forloop->var->var->is_init_shared = true;
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
    leave_loop(context);
    return create_unit_type();
}


struct type_expr *_analyze_while(struct sema_context *context, struct ast_node *node)
{
    struct loop_nested_level *bnl = enter_loop(context);
    bnl->block_levels = 1; 
    struct type_expr *bool_type = create_nullary_type(TYPE_BOOL);
    struct type_expr *expr_type = analyze(context, node->whileloop->expr);
    struct type_expr *body_type = analyze(context, node->whileloop->body);
    leave_loop(context);
    if (!unify(bool_type, expr_type, &context->nongens)) {
        printf("failed to unify expr type as bool.\n");
        return 0;
    }
    node->whileloop->body->type = body_type;
    return create_nullary_type(TYPE_UNIT);
}

struct type_expr *_analyze_jump(struct sema_context *context, struct ast_node *node)
{
    struct type_expr *type = 0;
    if (node->jump->expr){
        type = analyze(context, node->jump->expr);
    }else{
        struct loop_nested_level *bnl = get_current_block_level(context);
        assert(bnl);
        node->jump->nested_block_levels = bnl->block_levels;
        type = create_unit_type();
    }
    return type;
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
        case ARRAY_INIT_NODE:
            type = _analyze_array_init(context, node);
            break;
        case ARRAY_TYPE_NODE:
            type = _analyze_array_type(context, node);
            break;
        case TYPE_NODE:
            type = _analyze_type_node(context, node);
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
        case CAST_NODE:
            type = _analyze_cast(context, node);
            break;
        case MEMBER_INDEX_NODE:
            if(node->index->aggregate_type == AGGREGATE_TYPE_ARRAY)
                type = _analyze_array_member_accessor(context, node);
            else if(node->index->aggregate_type == AGGREGATE_TYPE_STRUCT)
                type = _analyze_struct_field_accessor(context, node);
            break;
        case BINARY_NODE:
            type = _analyze_binary(context, node);
            break;
        case ASSIGN_NODE:
            type = _analyze_assign(context, node);
            break;
        case IF_NODE:
            type = _analyze_if(context, node);
            break;
        case FOR_NODE:
            type = _analyze_for(context, node);
            break;
        case WHILE_NODE:
            type = _analyze_while(context, node);
            break;
        case JUMP_NODE:
            type = _analyze_jump(context, node);
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
