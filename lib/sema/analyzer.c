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
#include "codegen/codegen.h"
#include "codegen/env.h"
#include "sys.h"
#include "tool/cmodule.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

const char *relational_ops[] = {
    "<",
    ">",
    "==",
    "!=",
    "<=",
    ">="
};

bool _is_predicate_op(const char *op)
{
    for (size_t i = 0; i < ARRAY_SIZE(relational_ops); i++) {
        if (strcmp(relational_ops[i], op) == 0)
            return true;
    }
    return false;
}

void _log_err(struct sema_context *context, struct source_loc loc, const char *msg)
{
    (void)context;
    char full_msg[512];
    sprintf(full_msg, "%s:%d:%d: %s", "", loc.line, loc.col, msg);
    log_info(ERROR, full_msg);
}

struct type_exp *_analyze_unk(struct sema_context *context, struct exp_node *node)
{
    printf("analyzing unk: %s\n", node_type_strings[node->node_type]);
    if (!context || !node)
        return 0;
    return 0;
}

struct type_exp *retrieve_type_with_type_name(struct sema_context *context, symbol name)
{
    return get_symbol_type(&context->tenv, &context->nongens, name);
}

struct type_exp *retrieve_type_for_var_name(struct sema_context *context, symbol name)
{
    return get_symbol_type(&context->venv, &context->nongens, name);
}

struct type_exp *_analyze_ident(struct sema_context *context, struct exp_node *node)
{
    struct ident_node *ident = (struct ident_node *)node;
    struct type_exp *type = 0;
    for (size_t i = 0; i < array_size(&ident->member_accessors); i++) {
        symbol id = *((symbol *)array_get(&ident->member_accessors, i));
        if (i == 0) {
            type = retrieve_type_for_var_name(context, id);
        } else {
            assert(type);
            struct type_oper *oper = (struct type_oper *)type;
            struct type_node *type_node = (struct type_node *)hashtable_get_p(&context->ext_type_ast, oper->base.name);
            int index = find_member_index(type_node, string_get(id));
            if (index < 0) {
                _log_err(context, node->loc, "%s member not matched.");
                return 0;
            }
            type = *(struct type_exp **)array_get(&oper->args, index);
        }
    }
    return type;
}

struct type_exp *_analyze_num(struct sema_context *context, struct exp_node *node)
{
    symbol symbol_type = to_symbol(type_strings[node->annotated_type->type]);
    return retrieve_type_with_type_name(context, symbol_type);
}

struct type_exp *_analyze_var(struct sema_context *context, struct exp_node *node)
{
    struct var_node *var = (struct var_node *)node;
    struct type_exp *type;
    struct env *env = get_env();
    assert(var->base.annotated_type || var->init_value);
    if (var->base.annotated_type && var->base.annotated_type->type == TYPE_EXT) {
        assert(var->base.annotation);
        type = retrieve_type_with_type_name(context, var->base.annotation);
        push_symbol_type(&context->venv, var->var_name, type);
        if (var->init_value)
            analyze(env->sema_context, var->init_value);
        return type;
    } else if (var->base.annotated_type && !var->init_value) {
        type = var->base.annotated_type;
        push_symbol_type(&context->venv, var->var_name, type);
        return type;
    }
    type = analyze(context, var->init_value);
    if (!type)
        return 0;
    if (var->base.annotated_type && var->init_value->annotated_type
        && var->base.annotated_type->type != var->init_value->annotated_type->type) {
        _log_err(context, node->loc, "variable type not matched with literal constant");
        return 0;
    }
    struct type_exp *var_type;
    if (has_symbol_in_scope(&context->venv, var->var_name, context->scope_marker))
        var_type = retrieve_type_for_var_name(context, var->var_name);
    else
        var_type = (struct type_exp *)create_type_var();
    bool unified = unify(var_type, type, &context->nongens);
    if (!unified) {
        _log_err(context, node->loc, "variable type not matched with literal constant");
        return 0;
    }
    push_symbol_type(&context->venv, var->var_name, var_type);
    return var_type;
}

struct type_exp *_analyze_type(struct sema_context *context, struct exp_node *node)
{
    struct type_node *type = (struct type_node *)node;
    struct array args;
    array_init(&args, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&type->body->nodes); i++) {
        //printf("creating type: %zu\n", i);
        struct type_exp *arg = _analyze_var(context, *(struct exp_node **)array_get(&type->body->nodes, i));
        array_push(&args, &arg);
    }
    struct type_exp *result_type = (struct type_exp *)create_type_oper_ext(type->name, &args);
    assert(type->name == result_type->name);
    push_symbol_type(&context->tenv, type->name, result_type);
    hashtable_set_p(&context->ext_type_ast, type->name, node);
    return result_type;
}

struct type_exp *_analyze_type_value(struct sema_context *context, struct exp_node *node)
{
    struct type_value_node *type_value = (struct type_value_node *)node;
    struct env *env = get_env();
    for (size_t i = 0; i < array_size(&type_value->body->nodes); i++) {
        //printf("creating type: %zu\n", i);
        analyze(env->sema_context, *(struct exp_node **)array_get(&type_value->body->nodes, i));
    }
    return 0;
}

struct type_exp *_analyze_proto(struct sema_context *context, struct exp_node *node)
{
    struct prototype_node *proto = (struct prototype_node *)node;
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&proto->fun_params); i++) {
        struct var_node *param = (struct var_node *)array_get(&proto->fun_params, i);
        assert(param->base.annotated_type);
        param->base.type = param->base.annotated_type;
        array_push(&fun_sig, &param->base.annotated_type);
    }
    assert(proto->base.annotated_type);
    array_push(&fun_sig, &proto->base.annotated_type);
    //printf("ret type analyzing proto: %p, %p\n", (void*)proto->base.annotated_type, *(void**)array_back(&fun_sig));
    proto->base.type = (struct type_exp *)create_type_fun(&fun_sig);
    return proto->base.type;
}

struct type_exp *_analyze_fun(struct sema_context *context, struct exp_node *node)
{
    struct function_node *fun = (struct function_node *)node;
    //# create a new non-generic variable for the binder
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&fun->prototype->fun_params); i++) {
        struct var_node *param = (struct var_node *)array_get(&fun->prototype->fun_params, i);
        struct type_exp *exp;
        if (param->base.annotated_type)
            exp = param->base.annotated_type;
        else
            exp = (struct type_exp *)create_type_var();
        array_push(&fun_sig, &exp);
        array_push(&context->nongens, &exp);
        push_symbol_type(&context->venv, param->var_name, exp);
    }
    /*analyze function body*/
    struct type_exp *fun_type = (struct type_exp *)create_type_var();
    push_symbol_type(&context->venv, fun->prototype->name, fun_type);
    struct type_exp *ret_type = analyze(context, (struct exp_node *)fun->body);
    array_push(&fun_sig, &ret_type);
    struct type_exp *result_type = (struct type_exp *)create_type_fun(&fun_sig);
    unify(fun_type, result_type, &context->nongens);
    struct type_exp *result = prune(fun_type);
    fun->prototype->base.type = result;
    if (is_generic(result)) {
        hashtable_set(&context->generic_ast, string_get(fun->prototype->name), node);
    }
    return result;
}

struct type_exp *_analyze_call(struct sema_context *context, struct exp_node *node)
{
    struct call_node *call = (struct call_node *)node;
    struct env *env = get_env();
    struct type_exp *fun_type = retrieve_type_for_var_name(context, call->callee);
    if (!fun_type) {
        struct source_loc loc = { 1, 1 };
        string error;
        string_copy(&error, call->callee);
        string_add_chars(&error, " not defined");
        _log_err(context, loc, string_get(&error));
        string_deinit(&error);
        return 0;
    }
    struct array args;
    array_init(&args, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&call->args); i++) {
        struct exp_node *arg = *(struct exp_node **)array_get(&call->args, i);
        struct type_exp *type = analyze(context, arg);
        array_push(&args, &type);
    }

    /* monomorphization of generic */
    struct exp_node *specialized_node = 0;
    if (is_generic(fun_type) && (!is_any_generic(&args) && array_size(&args)) && !is_recursive(call)) {
        string sp_callee = monomorphize(string_get(call->callee), &args);
        call->specialized_callee = to_symbol(string_get(&sp_callee));
        if (has_symbol(&context->venv, call->specialized_callee)) {
            fun_type = retrieve_type_for_var_name(context, call->specialized_callee);
            struct type_oper *fun_op = (struct type_oper *)fun_type;
            return *(struct type_exp **)array_back(&fun_op->args);
        }
        /* specialized callee */
        struct exp_node *generic_fun = (struct exp_node *)hashtable_get(&context->generic_ast, string_get(call->callee));
        struct function_node *sp_fun = (struct function_node *)node_copy(generic_fun);
        sp_fun->prototype->name = call->specialized_callee;
        fun_type = analyze(env->sema_context, (struct exp_node *)sp_fun);
        hashtable_set(&context->specialized_ast, string_get(sp_fun->prototype->name), sp_fun);
        push_symbol_type(&context->venv, call->specialized_callee, fun_type);
        specialized_node = (struct exp_node *)sp_fun;
    }
    struct type_exp *result_type = (struct type_exp *)create_type_var();
    array_push(&args, &result_type);
    struct type_exp *call_fun = (struct type_exp *)create_type_fun(&args);
    unify(call_fun, fun_type, &context->nongens);
    if (hashtable_in_p(&context->builtin_ast, call->callee)) {
        array_push(&context->used_builtin_names, &call->callee);
    }
    // TODO: this should be moved to codegen phase
    if (specialized_node) {
        emit_ir_code(env->cg, specialized_node);
    }
    return prune(result_type);
}

struct type_exp *_analyze_unary(struct sema_context *context, struct exp_node *node)
{
    struct unary_node *unary = (struct unary_node *)node;
    struct type_exp *op_type = analyze(context, unary->operand);
    if (string_eq_chars(unary->op, "!")) {
        struct type_exp *bool_type = (struct type_exp *)create_nullary_type(TYPE_BOOL);
        unify(op_type, bool_type, &context->nongens);
        unary->operand->type = op_type;
    }
    return op_type;
}

struct type_exp *_analyze_binary(struct sema_context *context, struct exp_node *node)
{
    struct binary_node *bin = (struct binary_node *)node;
    struct type_exp *lhs_type = analyze(context, bin->lhs);
    struct type_exp *rhs_type = analyze(context, bin->rhs);
    struct type_exp *result = 0;
    if (unify(lhs_type, rhs_type, &context->nongens)) {
        if (_is_predicate_op(string_get(bin->op)))
            result = (struct type_exp *)create_nullary_type(TYPE_BOOL);
        else
            result = lhs_type;
        return result;
    } else {
        string error;
        string_init_chars(&error, "type not same for binary op: ");
        string_add(&error, bin->op);
        _log_err(context, bin->base.loc, string_get(&error));
    }
    return result;
}

struct type_exp *_analyze_if(struct sema_context *context, struct exp_node *node)
{
    struct condition_node *cond_node = (struct condition_node *)node;
    struct type_exp *cond_type = analyze(context, cond_node->condition_node);
    struct type_oper *bool_type = create_nullary_type(TYPE_BOOL);
    unify(cond_type, (struct type_exp *)bool_type, &context->nongens);
    struct type_exp *then_type = analyze(context, cond_node->then_node);
    struct type_exp *else_type = analyze(context, cond_node->else_node);
    unify(then_type, else_type, &context->nongens);
    return then_type;
}

struct type_exp *_analyze_for(struct sema_context *context, struct exp_node *node)
{
    struct for_node *for_node = (struct for_node *)node;
    struct type_exp *int_type = (struct type_exp *)create_nullary_type(TYPE_INT);
    struct type_exp *bool_type = (struct type_exp *)create_nullary_type(TYPE_BOOL);
    push_symbol_type(&context->venv, for_node->var_name, int_type);
    struct type_exp *start_type = analyze(context, for_node->start);
    struct type_exp *step_type = analyze(context, for_node->step);
    struct type_exp *end_type = analyze(context, for_node->end);
    struct type_exp *body_type = analyze(context, for_node->body);
    if (!unify(start_type, int_type, &context->nongens)) {
        printf("failed to unify start type as int: %s, %s\n", kind_strings[start_type->kind], node_type_strings[for_node->start->node_type]);
    }
    unify(step_type, int_type, &context->nongens);
    unify(end_type, bool_type, &context->nongens);
    for_node->start->type = start_type;
    for_node->step->type = step_type;
    for_node->end->type = end_type;
    for_node->body->type = body_type;
    return (struct type_exp *)create_nullary_type(TYPE_UNIT);
}

struct type_exp *_analyze_block(struct sema_context *context, struct exp_node *node)
{
    struct block_node *block = (struct block_node *)node;
    enter_scope(context);
    struct type_exp *exp = 0;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node *node = *(struct exp_node **)array_get(&block->nodes, i);
        exp = analyze(context, node);
    }
    leave_scope(context);
    return exp;
}

struct type_exp *(*analyze_fp[])(struct sema_context *, struct exp_node *) = {
    _analyze_unk,
    _analyze_num,
    _analyze_ident,
    _analyze_var,
    _analyze_type,
    _analyze_type_value,
    _analyze_unary,
    _analyze_binary,
    _analyze_if,
    _analyze_for,
    _analyze_call,
    _analyze_proto,
    _analyze_fun,
    _analyze_block,
};

struct type_exp *analyze(struct sema_context *context, struct exp_node *node)
{
    struct type_exp *type = analyze_fp[node->node_type](context, node);
    node->type = type;
    return type;
}
