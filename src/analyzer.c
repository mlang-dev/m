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

type_exp* _analyze_unk(type_env* env, exp_node* node)
{
    printf("analyzing unk: %s\n", NodeTypeString[node->node_type]);
    if (!env || !node)
        return 0;
    return 0;
}

type_exp* retrieve(type_env* env, string *name)
{
    return retrieve_type(name, &env->nogens, &env->type_env);
}

type_exp* _analyze_ident(type_env* env, exp_node* ident)
{
    return retrieve(env, &((ident_node*)ident)->name);
}

type_exp* _analyze_num(type_env* env, exp_node* num)
{
    return retrieve(env, &((num_node*)num)->base.type->name);
}

type_exp* _analyze_var(type_env* env, exp_node* node)
{
    var_node* var = (var_node*)node;
    type_exp* type = analyze(env, var->init_value);
    if (!type)
        return type;
    type_exp* result_type = (type_exp*)create_type_var();
    unify(result_type, type, &env->nogens);
    node->type = result_type;
    return result_type;
}

type_exp* _analyze_call(type_env* env, exp_node* node)
{
    call_node* call = (call_node*)node;
    type_exp* fun_type = retrieve(env, &call->callee);
    assert(fun_type);
    array args;
    array_init(&args, sizeof(type_exp*));
    for(size_t i = 0; i < array_size(&call->args); i++){
        exp_node* arg = *(exp_node**)array_get(&call->args, i);
        type_exp *type = analyze(env, arg);
        array_push(&args, &type);
    }
    type_exp* result_type = (type_exp*)create_type_var();
    unify((type_exp*)create_type_fun(&args, result_type), fun_type, &env->nogens);
    array_deinit(&args);
    node->type = result_type;
    return result_type;
}


type_exp* _analyze_una(type_env* env, exp_node* node)
{
    unary_node* unary = (unary_node*)node;
    type_exp* op_type = analyze(env, unary->operand);
    if(string_eq_chars(&unary->op, "!")){
        type_exp* bool_type = (type_exp*)create_nullary_type("bool");
        unify(op_type, bool_type, &env->nogens);
        unary->operand->type = op_type;
        node->type = op_type;
    }
    return op_type;
}

type_exp* _analyze_bin(type_env* env, exp_node* node)
{
    binary_node* bin = (binary_node*)node;
    type_exp* lhs_type = analyze(env, bin->lhs);
    type_exp* rhs_type = analyze(env, bin->rhs);
    if (unify(lhs_type, rhs_type, &env->nogens)){
        node->type = lhs_type;
        return lhs_type;
    }
    //log_info(DEBUG, "error binary op with different type");
    return NULL;
}

type_exp* _analyze_cond(type_env* env, exp_node* node)
{
    condition_node *cond_node = (condition_node*)node;
    type_exp *cond_type = analyze(env, cond_node->condition_node);
    type_oper *bool_type = create_nullary_type("bool");
    unify(cond_type, (type_exp*)bool_type, &env->nogens);
    type_exp *then_type = analyze(env, cond_node->then_node);
    type_exp *else_type = analyze(env, cond_node->else_node);
    unify(then_type, else_type, &env->nogens);
    return then_type;
}


type_exp* _analyze_for(type_env* env, exp_node* node)
{
    for_node *f_node = (for_node*)node;
    type_exp *int_type = (type_exp*)create_nullary_type("int");
    type_exp* start_type = analyze(env, f_node->start);
    type_exp* step_type = analyze(env, f_node->step);
    binary_node* bin = (binary_node*)f_node->end;
    type_exp* end_type = analyze(env, bin->rhs);
    unify(start_type, int_type, &env->nogens);
    unify(step_type, int_type, &env->nogens);
    unify(end_type, int_type, &env->nogens);
    return (type_exp*)create_nullary_type("()");
}

type_env* type_env_new()
{
    type_env* env = (type_env*)malloc(sizeof(type_env));
    memset((void*)env, 0, sizeof(type_env));
    array_init(&env->nogens, sizeof(type_exp*));
    hashtable_init(&env->type_env);
    array args;
    array_init(&args, sizeof(type_exp*));
    int types = sizeof(TypeString)/sizeof(TypeString[0]);
    for (int i=0; i<types; i++){
        string type_str;
        string_init_chars(&type_str, TypeString[i]);
        type_exp* exp = (type_exp*)create_type_oper(&type_str, &args);
        hashtable_set(&env->type_env, TypeString[i], exp);
    }
    return env;
}

void type_env_free(type_env* env)
{
    // for (int i = 0; i < env->type_env.buckets.cap; i++) {
    //     hashbox* box = (hashbox*)array_get(&env->type_env.buckets, i);
    //     if (box->status)
    //         type_exp_free(*(type_exp**)hashbox_get_value(box));
    // }
    hashtable_deinit(&env->type_env);
    free(env);
}

type_exp* _analyze_block(type_env* env, exp_node* node)
{
    block_node* block = (block_node*)node;
    //std::vector<type_exp*> exps;
    type_exp* exp = NULL;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        exp_node *node = *(exp_node**)array_get(&block->nodes, i);
        exp = analyze(env, node);//exps.push_back(analyze(env, node));
        node->type = exp;
    }
    return exp;
}

type_exp* _analyze_proto(type_env* env, exp_node* node)
{
    printf("analyzing proto: %s\n", NodeTypeString[node->node_type]);
    if (!env || !node)
        return 0;
    return 0;
}

type_exp* _analyze_fun(type_env* env, exp_node* node)
{
    function_node* fun = (function_node*)node;
    //# create a new non-generic variable for the binder
    array args;//<type_exp*> args;
    array_init(&args, sizeof(type_exp*));
    for(size_t i = 0; i < array_size(&fun->prototype->args); i++){
        type_exp *exp = (type_exp*)create_type_var();
        array_push(&args, &exp);
        array_push(&env->nogens, &exp);
        char *arg_str = string_get((string*)array_get(&fun->prototype->args, i));
        hashtable_set(&env->type_env, arg_str, exp);
    }
    type_exp *fun_type = (type_exp*)create_type_var();
    hashtable_set(&env->type_env, string_get(&fun->prototype->name), fun_type);
    type_exp* ret_type = analyze(env, (exp_node*)fun->body); 
    type_exp* result_type = (type_exp*)create_type_fun(&args, ret_type);
    unify(fun_type, result_type, &env->nogens);
    node->type = prune(fun_type);
    //printf("analyzing fun: %p, %p\n", (void*)fun_type, (void*)result_type);
    array_deinit(&args);
    return node->type;
}

type_exp* (*analyze_fp[])(type_env*, exp_node*) = {
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

type_exp* analyze(type_env* env, exp_node* node)
{
    return analyze_fp[node->node_type](env, node);
}
