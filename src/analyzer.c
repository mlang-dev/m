/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Type Inference Analyzer
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "analyzer.h"
#include "clib/hash.h"
#include "cmodule.h"


const char* relational_ops[] = {
    "<",
    ">",
    "==",
    "!=",
    "<=",
    ">="
};

bool _is_predicate_op(const char* op)
{
    for (size_t i = 0; i < ARRAY_SIZE(relational_ops); i++) {
        if (strcmp(relational_ops[i], op) == 0)
            return true;
    }
    return false;
}

void _log_err(struct type_env* env, struct source_loc loc,  const char *msg)
{
    (void)env;
    char full_msg[512];
    sprintf(full_msg, "%s:%d:%d: %s", "", loc.line, loc.col, msg);
    log_info(ERROR, full_msg);
}


struct type_exp* _analyze_unk(struct type_env* env, struct exp_node* node)
{
    printf("analyzing unk: %s\n", node_type_strings[node->node_type]);
    if (!env || !node)
        return 0;
    return 0;
}

struct type_exp* retrieve(struct type_env* env, const char* name)
{
    struct type_exp* type = retrieve_type(name, &env->nogens, &env->type_env);
    return type? type : retrieve_type(name, &env->nogens, &env->builtin_types);
}

void set(struct type_env* env, const char* name, struct type_exp* type)
{
    set_type(&env->type_env, name, type);
}

void set_builtin(struct type_env* env, const char* name, struct type_exp* type)
{
    set_type(&env->builtin_types, name, type);
}

bool is_builtin(struct type_env* env, const char* name)
{
    return hashtable_in(&env->builtin_types, name);
}

struct type_exp* _analyze_ident(struct type_env* env, struct exp_node* ident)
{
    return retrieve(env, string_get(&((struct ident_node*)ident)->name));
}

struct type_exp* _analyze_num(struct type_env* env, struct exp_node* node)
{
    return retrieve(env, type_strings[node->annotated_type->type]);
}

struct type_exp* _analyze_var(struct type_env* env, struct exp_node* node)
{
    struct var_node* var = (struct var_node*)node;
    struct type_exp* type = analyze(env, var->init_value);
    if (!type)
        return 0;
    struct type_exp* result_type = (struct type_exp*)create_type_var();
    unify(result_type, type, &env->nogens);
    set(env, string_get(&var->var_name), result_type);
    return result_type;
}

struct type_exp* _analyze_call(struct type_env* env, struct exp_node* node)
{
    struct call_node* call = (struct call_node*)node;
    struct type_exp* fun_type = retrieve(env, string_get(&call->callee));
    if (!fun_type){
        struct source_loc loc = {1, 1};
        string error;
        string_copy(&error, &call->callee);
        string_add_chars(&error, " not defined");
        _log_err(env, loc, string_get(&error));
        string_deinit(&error);
        return 0;
    }
    struct array args;
    array_init(&args, sizeof(struct type_exp*));
    for (size_t i = 0; i < array_size(&call->args); i++) {
        struct exp_node* arg = *(struct exp_node**)array_get(&call->args, i);
        struct type_exp* type = analyze(env, arg);
        array_push(&args, &type);
    }
    struct type_exp* result_type = (struct type_exp*)create_type_var();
    array_push(&args, &result_type);
 //   string fun_type_str = to_string(fun_type);
 //   printf("analyzing call: %s, %s\n", string_get(&call->callee), string_get(&fun_type_str));
    struct type_exp* call_fun = (struct type_exp*)create_type_fun(&args);
    unify(call_fun, fun_type, &env->nogens);
    if(is_builtin(env, string_get(&call->callee))){
        array_push(&env->ref_builtin_names, &call->callee);
    }
    return prune(result_type);
}

struct type_exp* _analyze_una(struct type_env* env, struct exp_node* node)
{
    struct unary_node* unary = (struct unary_node*)node;
    struct type_exp* op_type = analyze(env, unary->operand);
    if (string_eq_chars(&unary->op, "!")) {
        struct type_exp* bool_type = (struct type_exp*)create_nullary_type(TYPE_BOOL);
        unify(op_type, bool_type, &env->nogens);
        unary->operand->type = op_type;
    }
    return op_type;
}

struct type_exp* _analyze_bin(struct type_env* env, struct exp_node* node)
{
    struct binary_node* bin = (struct binary_node*)node;
    struct type_exp* lhs_type = analyze(env, bin->lhs);
    struct type_exp* rhs_type = analyze(env, bin->rhs);
    struct type_exp* result = 0;
    if (unify(lhs_type, rhs_type, &env->nogens)) {
        if (_is_predicate_op(string_get(&bin->op)))
            result = (struct type_exp*)create_nullary_type(TYPE_BOOL);
        else
            result = lhs_type;
        return result;
    }
    return result;
}

struct type_exp* _analyze_cond(struct type_env* env, struct exp_node* node)
{
    struct condition_node* cond_node = (struct condition_node*)node;
    struct type_exp* cond_type = analyze(env, cond_node->condition_node);
    struct type_oper* bool_type = create_nullary_type(TYPE_BOOL);
    unify(cond_type, (struct type_exp*)bool_type, &env->nogens);
    struct type_exp* then_type = analyze(env, cond_node->then_node);
    struct type_exp* else_type = analyze(env, cond_node->else_node);
    unify(then_type, else_type, &env->nogens);
    return then_type;
}

struct type_exp* _analyze_for(struct type_env* env, struct exp_node* node)
{
    struct for_node* for_node = (struct for_node*)node;
    struct type_exp* int_type = (struct type_exp*)create_nullary_type(TYPE_INT);
    struct type_exp* bool_type = (struct type_exp*)create_nullary_type(TYPE_BOOL);
    set(env, string_get(&for_node->var_name), int_type);
    struct type_exp* start_type = analyze(env, for_node->start);
    struct type_exp* step_type = analyze(env, for_node->step);
    struct type_exp* end_type = analyze(env, for_node->end);
    struct type_exp* body_type = analyze(env, for_node->body);
    if(!unify(start_type, int_type, &env->nogens)){
        printf("failed to unify start type as int: %s, %s\n", kind_strings[start_type->kind], node_type_strings[for_node->start->node_type]);
    }
    unify(step_type, int_type, &env->nogens);
    unify(end_type, bool_type, &env->nogens);
    for_node->start->type = start_type;
    for_node->step->type = step_type;
    for_node->end->type = end_type;
    for_node->body->type = body_type;
    return (struct type_exp*)create_nullary_type(TYPE_UNIT);
}

struct type_env* type_env_new(struct code_generator* cg)
{
    (void)cg;
    struct type_env* env = malloc(sizeof(*env));
    memset((void*)env, 0, sizeof(struct type_env));
    array_init(&env->nogens, sizeof(struct type_exp*));
    array_init_free(&env->ref_builtin_names, sizeof(string), string_free_generic);
    hashtable_init(&env->type_env);
    hashtable_init(&env->builtin_types);
    hashtable_init(&env->builtin_nodes);
    struct array args;
    array_init(&args, sizeof(struct type_exp*));
    /*nullary type: builtin default types*/
    for (size_t i = 0; i < ARRAY_SIZE(type_strings); i++) {
        struct type_exp* exp = (struct type_exp*)create_type_oper(i, &args);
        set_builtin(env, type_strings[i], exp);
    }
    struct array cio_builtins = parse_c_file("stdio.h");
    struct array cmath_builtins = parse_c_file("math.h");
    array_add(&cio_builtins, &cmath_builtins);
    struct array* builtins = &cio_builtins;//&cg->builtins;
    for (size_t i = 0; i < array_size(builtins); i++) {
        struct exp_node* node = *(struct exp_node**)array_get(builtins, i);
        assert(node->node_type == PROTOTYPE_NODE);
        struct prototype_node* proto = (struct prototype_node*)node;
        analyze(env, (struct exp_node*)proto);
        set_builtin(env, string_get(&proto->name), proto->base.type);
        hashtable_set(&env->builtin_nodes, string_get(&proto->name), node);
        //string type = to_string(proto->base.type);
        //printf("parsed builtins: %s, %s\n", string_get(&proto->name), string_get(&type));
    }
    return env;
}

void type_env_free(struct type_env* env)
{
    // for (int i = 0; i < env->type_env.buckets.cap; i++) {
    //     hashbox* box = (hashbox*)array_get(&env->type_env.buckets, i);
    //     if (box->status)
    //         type_exp_free(*(struct type_exp**)hashbox_get_value(box));
    // }
    hashtable_deinit(&env->type_env);
    hashtable_deinit(&env->builtin_types);
    hashtable_deinit(&env->builtin_nodes);
    array_deinit(&env->ref_builtin_names);
    array_deinit(&env->nogens);
    free(env);
}

struct type_exp* _analyze_block(struct type_env* env, struct exp_node* node)
{
    struct block_node* block = (struct block_node*)node;
    struct type_exp* exp = 0;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node* node = *(struct exp_node**)array_get(&block->nodes, i);
        exp = analyze(env, node); 
        string exp_type = to_string(exp);
        if (node->node_type == FUNCTION_NODE)
            printf("to exp: %zu, %s, %s, %s\n", i, node_type_strings[node->node_type], string_get(&((struct function_node*)node)->prototype->name), string_get(&exp_type));
    }
    return exp;
}

struct type_exp* _analyze_proto(struct type_env* env, struct exp_node* node)
{
    assert(env);
    struct prototype_node* proto = (struct prototype_node*)node;
    struct array fun_sig; 
    array_init(&fun_sig, sizeof(struct type_exp*));
    for (size_t i = 0; i < array_size(&proto->fun_params); i++) {
        struct var_node* param = (struct var_node*)array_get(&proto->fun_params, i);
        assert(param->base.annotated_type);
        param->base.type = param->base.annotated_type;
        if(string_eq_chars(&proto->name, "var_func"))
            printf("proto: %d\n", param->base.type->type);
        array_push(&fun_sig, &param->base.annotated_type);
    }   
    assert(proto->base.annotated_type);
    array_push(&fun_sig, &proto->base.annotated_type);
    //printf("ret type analyzing proto: %p, %p\n", (void*)proto->base.annotated_type, *(void**)array_back(&fun_sig));
    proto->base.type = (struct type_exp*)create_type_fun(&fun_sig);
    return proto->base.type;
}

struct type_exp* _analyze_fun(struct type_env* env, struct exp_node* node)
{
    struct function_node* fun = (struct function_node*)node;
    //# create a new non-generic variable for the binder
    struct array fun_sig; 
    array_init(&fun_sig, sizeof(struct type_exp*));
    for (size_t i = 0; i < array_size(&fun->prototype->fun_params); i++) {
        struct var_node* param = (struct var_node*)array_get(&fun->prototype->fun_params, i);
        struct type_exp* exp;
        if (param->base.annotated_type)
            exp = param->base.annotated_type;
        else
            exp = (struct type_exp*)create_type_var();
        array_push(&fun_sig, &exp);
        array_push(&env->nogens, &exp);
        set(env, string_get(&param->var_name), exp);
        //printf("setting fun arg: %s\n", arg_str);
    }
    struct type_exp* fun_type = (struct type_exp*)create_type_var();
    set(env, string_get(&fun->prototype->name), fun_type);
    struct type_exp* ret_type = analyze(env, (struct exp_node*)fun->body);
    string ret_type_str = to_string(ret_type);
    printf("analyzing for body ret: %s\n", string_get(&ret_type_str));
    array_push(&fun_sig, &ret_type);
    struct type_exp* result_type = (struct type_exp*)create_type_fun(&fun_sig);
    unify(fun_type, result_type, &env->nogens);
    struct type_exp* result = prune(fun_type);
    fun->prototype->base.type = result;
    return result;
}

struct type_exp* (*analyze_fp[])(struct type_env*, struct exp_node*) = {
    _analyze_unk,
    _analyze_num,
    _analyze_ident,
    _analyze_var,
    _analyze_una,
    _analyze_bin,
    _analyze_cond,
    _analyze_for,
    _analyze_call,
    _analyze_proto,
    _analyze_fun,
    _analyze_block,
};

struct type_exp* analyze(struct type_env* env, struct exp_node* node)
{
    struct type_exp* type = analyze_fp[node->node_type](env, node);
    node->type = type;
    return type;
}
