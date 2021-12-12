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
#include "parser/m_grammar.h"
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

struct type_exp *_analyze_unk(struct sema_context *context, struct ast_node *node)
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

struct type_exp *_analyze_ident(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *type = 0;
    for (size_t i = 0; i < array_size(&node->ident->member_accessors); i++) {
        symbol id = *((symbol *)array_get(&node->ident->member_accessors, i));
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

struct type_exp *_analyze_liter(struct sema_context *context, struct ast_node *node)
{
    return retrieve_type_with_type_name(context, node->annotated_type_name);
}

struct type_exp *_analyze_var(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *type;
    assert(node->annotated_type_name || node->var->init_value);
    if (node->annotated_type_name && hashtable_get_int(&context->parser->symbol_2_int_types, node->annotated_type_name) == TYPE_EXT) {
        assert(node->annotated_type_name);
        type = retrieve_type_with_type_name(context, node->annotated_type_name);
        push_symbol_type(&context->decl_2_typexps, node->var->var_name, type);
        push_symbol_type(&context->varname_2_asts, node->var->var_name, node);
        if (node->var->init_value)
            analyze(context, node->var->init_value);
        return type;
    } else if (node->annotated_type_name && !node->var->init_value) {
        type = retrieve_type_with_type_name(context, node->annotated_type_name);
        assert(type);
        push_symbol_type(&context->decl_2_typexps, node->var->var_name, type);
        push_symbol_type(&context->varname_2_asts, node->var->var_name, node);
        return type;
    }
    type = analyze(context, node->var->init_value);
    if (!type)
        return 0;
    if (node->annotated_type_name && node->var->init_value->annotated_type_name
        && node->annotated_type_name != node->var->init_value->annotated_type_name) {
        _log_err(context, node->loc, "variable type not matched with literal constant");
        return 0;
    }
    struct type_exp *var_type;
    if (has_symbol_in_scope(&context->decl_2_typexps, node->var->var_name, context->scope_marker))
        var_type = retrieve_type_for_var_name(context, node->var->var_name);
    else
        var_type = (struct type_exp *)create_type_var();
    bool unified = unify(var_type, type, &context->nongens);
    if (!unified) {
        _log_err(context, node->loc, "variable type not matched with literal constant");
        return 0;
    }
    push_symbol_type(&context->decl_2_typexps, node->var->var_name, var_type);
    push_symbol_type(&context->varname_2_asts, node->var->var_name, node);
    return var_type;
}

struct type_exp *_analyze_type(struct sema_context *context, struct ast_node *node)
{
    struct array args;
    array_init(&args, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&node->type_def->body->block->nodes); i++) {
        //printf("creating type: %zu\n", i);
        struct type_exp *arg = _analyze_var(context, *(struct ast_node **)array_get(&node->type_def->body->block->nodes, i));
        array_push(&args, &arg);
    }
    struct type_oper *result_type = create_type_oper_ext(node->type_def->name, &args);
    assert(node->type_def->name == result_type->base.name);
    push_symbol_type(&context->typename_2_typexps, node->type_def->name, result_type);
    hashtable_set_p(&context->ext_typename_2_asts, node->type_def->name, node);
    return &result_type->base;
}

struct type_exp *_analyze_type_value(struct sema_context *context, struct ast_node *node)
{
    if (node->annotated_type_name)
        node->type = retrieve_type_with_type_name(context, node->annotated_type_name);
    for (size_t i = 0; i < array_size(&node->type_value->body->block->nodes); i++) {
        //printf("creating type: %zu\n", i);
        analyze(context, *(struct ast_node **)array_get(&node->type_value->body->block->nodes, i));
    }
    return node->type;
}

struct type_exp *_analyze_func_type(struct sema_context *context, struct ast_node *node)
{
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&node->ft->fun_params); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&node->ft->fun_params, i);
        assert(param->annotated_type_name);
        assert(param->annotated_type_enum == get_type_enum(param->annotated_type_name));
        struct type_oper* to = create_nullary_type(param->annotated_type_enum, param->annotated_type_name);
        param->type = &to->base;
        array_push(&fun_sig, &param->type);
    }
    assert(node->annotated_type_name);
    struct type_oper *to = create_nullary_type(node->annotated_type_enum, node->annotated_type_name);
    array_push(&fun_sig, &to);
    node->type = (struct type_exp *)create_type_fun(&fun_sig);
    hashtable_set_p(&context->func_types, node->ft->name, node);
    return node->type;
}

struct type_exp *_analyze_func(struct sema_context *context, struct ast_node *node)
{
    hashtable_set_p(&context->func_types, node->func->func_type->ft->name, node->func->func_type);
    stack_push(&context->func_stack, &node);
    //# create a new non-generic variable for the binder
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&node->func->func_type->ft->fun_params); i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&node->func->func_type->ft->fun_params, i);
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
    push_symbol_type(&context->decl_2_typexps, node->func->func_type->ft->name, fun_type);
    struct type_exp *ret_type = analyze(context, node->func->body);
    array_push(&fun_sig, &ret_type);
    struct type_exp *result_type = (struct type_exp *)create_type_fun(&fun_sig);
    unify(fun_type, result_type, &context->nongens);
    struct type_exp *result = prune(fun_type);
    node->func->func_type->type = result;
    if (is_generic(result)) {
        hashtable_set(&context->generic_ast, string_get(node->func->func_type->ft->name), node);
    }
    struct ast_node *saved_node = *(struct ast_node **)stack_pop(&context->func_stack);
    assert(node == saved_node);
    return result;
}

struct type_exp *_analyze_call(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *fun_type = retrieve_type_for_var_name(context, node->call->callee);
    if (!fun_type) {
        struct source_location loc = { 1, 1, 0, 0 };
        string error;
        string_copy(&error, node->call->callee);
        string_add_chars(&error, " not defined");
        _log_err(context, loc, string_get(&error));
        string_deinit(&error);
        return 0;
    }
    struct array args;
    array_init(&args, sizeof(struct type_exp *));
    for (size_t i = 0; i < array_size(&node->call->args); i++) {
        struct ast_node *arg = *(struct ast_node **)array_get(&node->call->args, i);
        struct type_exp *type = analyze(context, arg);
        assert(type);
        array_push(&args, &type);
    }

    /* monomorphization of generic */
    if (is_generic(fun_type) && (!is_any_generic(&args) && array_size(&args))) {
        struct ast_node *parent_func = stack_size(&context->func_stack) ? *(struct ast_node**)stack_top(&context->func_stack) : 0;
        if (!parent_func || (parent_func->func->func_type->ft->name != node->call->callee)){
            string sp_callee = monomorphize(string_get(node->call->callee), &args);
            node->call->specialized_callee = to_symbol(string_get(&sp_callee));
            if (has_symbol(&context->decl_2_typexps, node->call->specialized_callee)) {
                fun_type = retrieve_type_for_var_name(context, node->call->specialized_callee);
                struct type_oper *fun_op = (struct type_oper *)fun_type;
                return *(struct type_exp **)array_back(&fun_op->args);
            }
            /* specialized callee */
            struct ast_node *generic_fun = (struct ast_node *)hashtable_get(&context->generic_ast, string_get(node->call->callee));
            struct ast_node *sp_fun = node_copy(generic_fun);
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
    struct type_exp *result_type = (struct type_exp *)create_type_var();
    array_push(&args, &result_type);
    struct type_exp *call_fun = (struct type_exp *)create_type_fun(&args);
    unify(call_fun, fun_type, &context->nongens);
    if (hashtable_in_p(&context->builtin_ast, node->call->callee)) {
        array_push(&context->used_builtin_names, &node->call->callee);
    }
    if (!node->call->specialized_callee) {
        hashtable_set_p(&context->calls, node->call->callee, node);
        node->call->callee_func_type = hashtable_get_p(&context->func_types, node->call->callee);
    }
    return result_type;
}

struct type_exp *_analyze_unary(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *op_type = analyze(context, node->unop->operand);
    if (node->unop->opcode == OP_NOT) {
        struct type_exp *bool_type = (struct type_exp *)create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
        unify(op_type, bool_type, &context->nongens);
        node->unop->operand->type = op_type;
    }
    return op_type;
}

struct type_exp *_analyze_binary(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *lhs_type = analyze(context, node->binop->lhs);
    struct type_exp *rhs_type = analyze(context, node->binop->rhs);
    struct type_exp *result = 0;
    if (unify(lhs_type, rhs_type, &context->nongens)) {
        if (_is_predicate_op(get_opcode(node->binop->opcode)))
            result = (struct type_exp *)create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
        else
            result = lhs_type;
        return result;
    } else {
        string error;
        string_init_chars(&error, "type not same for binary op: ");
        string_add_chars(&error, get_opcode(node->binop->opcode));
        _log_err(context, node->loc, string_get(&error));
    }
    return result;
}

struct type_exp *_analyze_if(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *cond_type = analyze(context, node->cond->if_node);
    struct type_oper *bool_type = create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
    unify(cond_type, (struct type_exp *)bool_type, &context->nongens);
    struct type_exp *then_type = analyze(context, node->cond->then_node);
    struct type_exp *else_type = analyze(context, node->cond->else_node);
    unify(then_type, else_type, &context->nongens);
    return then_type;
}

struct type_exp *_analyze_for(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *int_type = (struct type_exp *)create_nullary_type(TYPE_INT, get_type_symbol(TYPE_INT));
    struct type_exp *bool_type = (struct type_exp *)create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
    push_symbol_type(&context->decl_2_typexps, node->forloop->var_name, int_type);
    struct type_exp *start_type = analyze(context, node->forloop->start);
    struct type_exp *step_type = analyze(context, node->forloop->step);
    struct type_exp *end_type = analyze(context, node->forloop->end);
    struct type_exp *body_type = analyze(context, node->forloop->body);
    if (!unify(start_type, int_type, &context->nongens)) {
        printf("failed to unify start type as int: %s, %s\n", kind_strings[start_type->kind], node_type_strings[node->forloop->start->node_type]);
    }
    unify(step_type, int_type, &context->nongens);
    unify(end_type, bool_type, &context->nongens);
    node->forloop->start->type = start_type;
    node->forloop->step->type = step_type;
    node->forloop->end->type = end_type;
    node->forloop->body->type = body_type;
    return (struct type_exp *)create_nullary_type(TYPE_UNIT, get_type_symbol(TYPE_UNIT));
}

struct type_exp *_analyze_block(struct sema_context *context, struct ast_node *node)
{
    enter_scope(context);
    struct type_exp *type = 0;
    for (size_t i = 0; i < array_size(&node->block->nodes); i++) {
        struct ast_node *n = *(struct ast_node **)array_get(&node->block->nodes, i);
        type = analyze(context, n);
    }
    //tag variable node as returning variable if exists
    struct ast_node *ret_node = *(struct ast_node **)array_back(&node->block->nodes);
    ret_node->is_ret = true;
    if (ret_node->node_type == IDENT_NODE) {
        symbol var_name = ret_node->ident->name;
        struct ast_node *var = (struct ast_node *)symboltable_get(&context->varname_2_asts, var_name);
        //struct member reference id node like xy.x is not a variable
        if (var)
            var->is_ret = true;
    }
    leave_scope(context);
    return type;
}

struct type_exp *analyze(struct sema_context *context, struct ast_node *node)
{
    struct type_exp *type = 0;
    switch(node->node_type){
        case UNK_NODE:
            type = _analyze_unk(context, node);
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
        case TYPE_NODE:
            type = _analyze_type(context, node);
            break;
        case TYPE_VALUE_NODE:
            type = _analyze_type_value(context, node);
            break;
        case UNARY_NODE:
            type = _analyze_unary(context, node);
            break;
        case BINARY_NODE:
            type = _analyze_binary(context, node);
            break;
        case CONDITION_NODE:
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
        case FUNCTION_NODE:
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
