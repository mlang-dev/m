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
#include "codegen/codegen.h"
#include "codegen/env.h"
#include "tool/cmodule.h"
#include <assert.h>
#include <limits.h>

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

void _log_err(struct sema_context *context, struct source_location loc, const char *msg)
{
    (void)context;
    char full_msg[512];
    sprintf_s(full_msg, sizeof(full_msg), "%s:%d:%d: %s", "", loc.line, loc.col, msg);
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
    return get_symbol_type(&context->typename_2_typexps, &context->nongens, name);
}

struct type_exp *retrieve_type_for_var_name(struct sema_context *context, symbol name)
{
    return get_symbol_type(&context->decl_2_typexps, &context->nongens, name);
}

struct type_exp *_analyze_ident(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *ident = (struct ast_node *)node;
    struct type_exp *type = 0;
    for (size_t i = 0; i < array_size(&ident->ident->member_accessors); i++) {
        symbol id = *((symbol *)array_get(&ident->ident->member_accessors, i));
        if (i == 0) {
            type = retrieve_type_for_var_name(context, id);
        } else {
            assert(type);
            struct type_oper *oper = (struct type_oper *)type;
            struct ast_node *type_node = (struct ast_node *)hashtable_get_p(&context->ext_typename_2_asts, oper->base.name);
            int index = find_member_index(type_node, id);
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
    return retrieve_type_with_type_name(context, node->annotated_type_name);
}

struct type_exp *_analyze_var(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *var = (struct ast_node *)node;
    struct type_exp *type;
    struct env *env = get_env();
    assert(var->annotated_type_name || var->var->init_value);
    if (var->annotated_type_name && hashtable_get_int(&context->parser->symbol_2_int_types, var->annotated_type_name) == TYPE_EXT) {
        assert(var->annotated_type_name);
        type = retrieve_type_with_type_name(context, var->annotated_type_name);
        push_symbol_type(&context->decl_2_typexps, var->var->var_name, type);
        push_symbol_type(&context->varname_2_asts, var->var->var_name, var);
        if (var->var->init_value)
            analyze(env->sema_context, var->var->init_value);
        return type;
    } else if (var->annotated_type_name && !var->var->init_value) {
        type = retrieve_type_with_type_name(context, var->annotated_type_name);
        assert(type);
        push_symbol_type(&context->decl_2_typexps, var->var->var_name, type);
        push_symbol_type(&context->varname_2_asts, var->var->var_name, var);
        return type;
    }
    type = analyze(context, var->var->init_value);
    if (!type)
        return 0;
    if (var->annotated_type_name && var->var->init_value->annotated_type_name
        && var->annotated_type_name != var->var->init_value->annotated_type_name) {
        _log_err(context, node->loc, "variable type not matched with literal constant");
        return 0;
    }
    struct type_exp *var_type;
    if (has_symbol_in_scope(&context->decl_2_typexps, var->var->var_name, context->scope_marker))
        var_type = retrieve_type_for_var_name(context, var->var->var_name);
    else
        var_type = (struct type_exp *)create_type_var();
    bool unified = unify(var_type, type, &context->nongens);
    if (!unified) {
        _log_err(context, node->loc, "variable type not matched with literal constant");
        return 0;
    }
    push_symbol_type(&context->decl_2_typexps, var->var->var_name, var_type);
    push_symbol_type(&context->varname_2_asts, var->var->var_name, var);
    return var_type;
}

struct type_exp *_analyze_type(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *type = (struct ast_node *)node;
    struct array args;
    array_init(&args, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&type->type_def->body->block->nodes); i++) {
        //printf("creating type: %zu\n", i);
        struct type_exp *arg = _analyze_var(context, *(struct exp_node **)array_get(&type->type_def->body->block->nodes, i));
        array_push(&args, &arg);
    }
    struct type_oper *result_type = create_type_oper_ext(type->type_def->name, &args);
    assert(type->type_def->name == result_type->base.name);
    push_symbol_type(&context->typename_2_typexps, type->type_def->name, result_type);
    hashtable_set_p(&context->ext_typename_2_asts, type->type_def->name, node);
    return &result_type->base;
}

struct type_exp *_analyze_type_value(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *type_value = (struct ast_node *)node;
    struct env *env = get_env();
    if (type_value->annotated_type_name)
        type_value->type = retrieve_type_with_type_name(context, type_value->annotated_type_name);
    for (size_t i = 0; i < array_size(&type_value->type_value->body->block->nodes); i++) {
        //printf("creating type: %zu\n", i);
        analyze(env->sema_context, *(struct exp_node **)array_get(&type_value->type_value->body->block->nodes, i));
    }
    return type_value->type;
}

struct type_exp *_analyze_func_type(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *func_type = (struct ast_node *)node;
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&func_type->ft->fun_params); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&func_type->ft->fun_params, i);
        assert(param->annotated_type_name);
        assert(param->annotated_type_enum == get_type_enum(param->annotated_type_name));
        struct type_oper* to = create_nullary_type(param->annotated_type_enum, param->annotated_type_name);
        param->type = &to->base;
        array_push(&fun_sig, &param->type);
    }
    assert(func_type->annotated_type_name);
    struct type_oper *to = create_nullary_type(func_type->annotated_type_enum, func_type->annotated_type_name);
    array_push(&fun_sig, &to);
    func_type->type = (struct type_exp *)create_type_fun(&fun_sig);
    hashtable_set_p(&context->protos, func_type->ft->name, func_type);
    return func_type->type;
}

struct type_exp *_analyze_fun(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *fun = (struct ast_node *)node;
    hashtable_set_p(&context->protos, fun->func->func_type->ft->name, fun->func->func_type);
    //# create a new non-generic variable for the binder
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&fun->func->func_type->ft->fun_params); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&fun->func->func_type->ft->fun_params, i);
        struct type_exp *exp;
        if (param->annotated_type_name) {
            if (param->annotated_type_enum == TYPE_EXT)
                exp = retrieve_type_with_type_name(context, param->annotated_type_name);
            else{
                struct type_oper *to = create_nullary_type(param->annotated_type_enum, param->annotated_type_name);
                exp = &to->base;
            }
        } else
            exp = (struct type_exp *)create_type_var();
        array_push(&fun_sig, &exp);
        array_push(&context->nongens, &exp);
        push_symbol_type(&context->decl_2_typexps, param->var->var_name, exp);
        push_symbol_type(&context->varname_2_asts, param->var->var_name, param);
    }
    /*analyze function body*/
    struct type_exp *fun_type = (struct type_exp *)create_type_var();
    push_symbol_type(&context->decl_2_typexps, fun->func->func_type->ft->name, fun_type);
    struct type_exp *ret_type = analyze(context, (struct exp_node *)fun->func->body);
    array_push(&fun_sig, &ret_type);
    struct type_exp *result_type = (struct type_exp *)create_type_fun(&fun_sig);
    unify(fun_type, result_type, &context->nongens);
    struct type_exp *result = prune(fun_type);
    fun->func->func_type->type = result;
    if (is_generic(result)) {
        hashtable_set(&context->generic_ast, string_get(fun->func->func_type->ft->name), node);
    }
    return result;
}

struct type_exp *_analyze_call(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *call = (struct ast_node *)node;
    struct env *env = get_env();
    struct type_exp *fun_type = retrieve_type_for_var_name(context, call->call->callee);
    if (!fun_type) {
        struct source_location loc = { 1, 1 };
        string error;
        string_copy(&error, call->call->callee);
        string_add_chars(&error, " not defined");
        _log_err(context, loc, string_get(&error));
        string_deinit(&error);
        return 0;
    }
    struct array args;
    array_init(&args, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&call->call->args); i++) {
        struct exp_node *arg = *(struct exp_node **)array_get(&call->call->args, i);
        struct type_exp *type = analyze(context, arg);
        assert(type);
        array_push(&args, &type);
    }

    /* monomorphization of generic */
    struct exp_node *specialized_fun = 0;
    if (is_generic(fun_type) && (!is_any_generic(&args) && array_size(&args)) && !is_recursive(call)) {
        string sp_callee = monomorphize(string_get(call->call->callee), &args);
        call->call->specialized_callee = to_symbol(string_get(&sp_callee));
        if (has_symbol(&context->decl_2_typexps, call->call->specialized_callee)) {
            fun_type = retrieve_type_for_var_name(context, call->call->specialized_callee);
            struct type_oper *fun_op = (struct type_oper *)fun_type;
            return *(struct type_exp **)array_back(&fun_op->args);
        }
        /* specialized callee */
        struct exp_node *generic_fun = (struct exp_node *)hashtable_get(&context->generic_ast, string_get(call->call->callee));
        struct ast_node *sp_fun = (struct ast_node *)node_copy(generic_fun);
        sp_fun->func->func_type->ft->name = call->call->specialized_callee;
        fun_type = analyze(env->sema_context, (struct exp_node *)sp_fun);
        hashtable_set(&context->specialized_ast, string_get(sp_fun->func->func_type->ft->name), sp_fun);
        push_symbol_type(&context->decl_2_typexps, call->call->specialized_callee, fun_type);
        specialized_fun = (struct exp_node *)sp_fun;
        hashtable_set_p(&context->protos, call->call->specialized_callee, sp_fun->func->func_type);
        hashtable_set_p(&context->calls, call->call->specialized_callee, node);
        call->call->callee_func_type = sp_fun->func->func_type;
    }
    struct type_exp *result_type = (struct type_exp *)create_type_var();
    array_push(&args, &result_type);
    struct type_exp *call_fun = (struct type_exp *)create_type_fun(&args);
    unify(call_fun, fun_type, &context->nongens);
    if (hashtable_in_p(&context->builtin_ast, call->call->callee)) {
        array_push(&context->used_builtin_names, &call->call->callee);
    }
    if (!call->call->specialized_callee) {
        hashtable_set_p(&context->calls, call->call->callee, node);
        call->call->callee_func_type = hashtable_get_p(&context->protos, call->call->callee);
    }
    // TODO: this should be moved to codegen phase
    if (specialized_fun) {
        emit_ir_code(env->cg, specialized_fun);
    }
    return result_type;
}

struct type_exp *_analyze_unary(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *unary = (struct ast_node *)node;
    struct type_exp *op_type = analyze(context, unary->unop->operand);
    if (string_eq_chars(unary->unop->op, "!")) {
        struct type_exp *bool_type = (struct type_exp *)create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
        unify(op_type, bool_type, &context->nongens);
        unary->unop->operand->type = op_type;
    }
    return op_type;
}

struct type_exp *_analyze_binary(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *bin = (struct ast_node *)node;
    struct type_exp *lhs_type = analyze(context, bin->binop->lhs);
    struct type_exp *rhs_type = analyze(context, bin->binop->rhs);
    struct type_exp *result = 0;
    if (unify(lhs_type, rhs_type, &context->nongens)) {
        if (_is_predicate_op(string_get(bin->binop->op)))
            result = (struct type_exp *)create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
        else
            result = lhs_type;
        return result;
    } else {
        string error;
        string_init_chars(&error, "type not same for binary op: ");
        string_add(&error, bin->binop->op);
        _log_err(context, bin->loc, string_get(&error));
    }
    return result;
}

struct type_exp *_analyze_if(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *cond_node = (struct ast_node *)node;
    struct type_exp *cond_type = analyze(context, cond_node->cond->if_node);
    struct type_oper *bool_type = create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
    unify(cond_type, (struct type_exp *)bool_type, &context->nongens);
    struct type_exp *then_type = analyze(context, cond_node->cond->then_node);
    struct type_exp *else_type = analyze(context, cond_node->cond->else_node);
    unify(then_type, else_type, &context->nongens);
    return then_type;
}

struct type_exp *_analyze_for(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *for_loop = (struct ast_node *)node;
    struct type_exp *int_type = (struct type_exp *)create_nullary_type(TYPE_INT, get_type_symbol(TYPE_INT));
    struct type_exp *bool_type = (struct type_exp *)create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
    push_symbol_type(&context->decl_2_typexps, for_loop->forloop->var_name, int_type);
    struct type_exp *start_type = analyze(context, for_loop->forloop->start);
    struct type_exp *step_type = analyze(context, for_loop->forloop->step);
    struct type_exp *end_type = analyze(context, for_loop->forloop->end);
    struct type_exp *body_type = analyze(context, for_loop->forloop->body);
    if (!unify(start_type, int_type, &context->nongens)) {
        printf("failed to unify start type as int: %s, %s\n", kind_strings[start_type->kind], node_type_strings[for_loop->forloop->start->node_type]);
    }
    unify(step_type, int_type, &context->nongens);
    unify(end_type, bool_type, &context->nongens);
    for_loop->forloop->start->type = start_type;
    for_loop->forloop->step->type = step_type;
    for_loop->forloop->end->type = end_type;
    for_loop->forloop->body->type = body_type;
    return (struct type_exp *)create_nullary_type(TYPE_UNIT, get_type_symbol(TYPE_UNIT));
}

struct type_exp *_analyze_block(struct sema_context *context, struct exp_node *node)
{
    struct ast_node *block = (struct ast_node *)node;
    enter_scope(context);
    struct type_exp *type = 0;
    for (size_t i = 0; i < array_size(&block->block->nodes); i++) {
        struct exp_node *n = *(struct exp_node **)array_get(&block->block->nodes, i);
        type = analyze(context, n);
    }
    //tag variable node as returning variable if exists
    struct exp_node *ret_node = *(struct exp_node **)array_back(&block->block->nodes);
    ret_node->is_ret = true;
    if (ret_node->node_type == IDENT_NODE) {
        symbol var_name = ((struct ast_node *)ret_node)->ident->name;
        struct ast_node *var = (struct ast_node *)symboltable_get(&context->varname_2_asts, var_name);
        //struct member reference id node like xy.x is not a variable
        if (var)
            var->is_ret = true;
    }
    leave_scope(context);
    return type;
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
    _analyze_func_type,
    _analyze_fun,
    _analyze_block,
};

struct type_exp *analyze(struct sema_context *context, struct exp_node *node)
{
    struct type_exp *type = analyze_fp[node->node_type](context, node);
    type = prune(type);
    node->type = type;
    return type;
}
