/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Type Inference Analyzer
 */
#include <stdlib.h>
#include <string.h>

#include "analyzer.h"
#include "clib/hash.h"

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
    return retrieve(env, &((num_node*)num)->base.type.name);
}

type_exp* _analyze_var(type_env* env, exp_node* node)
{
    var_node* var = (var_node*)node;
    type_exp* type = analyze(env, var->init_value);
    if (!type)
        return type;
    type_exp* result_type = (type_exp*)create_type_var();
    unify(result_type, type, &env->nogens);
    return result_type;
}

type_exp* _analyze_call(type_env* env, exp_node* node)
{
    call_node* call = (call_node*)node;
    type_exp* fun_type = retrieve(env, &call->callee);
    array args;
    array_init(&args, sizeof(type_exp*));
    for(int i = 0; i < array_size(&call->args); i++){
        exp_node* arg = *(exp_node**)array_get(&call->args, i);
        type_exp *type = analyze(env, arg);
        array_push(&args, &type);
    }
    type_exp* result_type = (type_exp*)create_type_var();
    unify((type_exp*)create_type_fun(&args, result_type), fun_type, &env->nogens);
    array_deinit(&args);
    return result_type;
}


type_exp* _analyze_bin(type_env* env, exp_node* node)
{
    binary_node* bin = (binary_node*)node;
    type_exp* lhs_type = analyze(env, bin->lhs);
    type_exp* rhs_type = analyze(env, bin->rhs);
    if (unify(lhs_type, rhs_type, &env->nogens))
        return lhs_type;
    //log_info(DEBUG, "error binary op with different type");
    return NULL;
}

type_env* type_env_new()
{
    type_env* env = (type_env*)malloc(sizeof(type_env));
    memset((void*)env, 0, sizeof(type_env));
    array_init(&env->nogens, sizeof(type_exp*));
    hashtable_init_ref(&env->type_env);
    array args;
    array_init(&args, sizeof(type_exp*));
    int types = sizeof(TypeString)/sizeof(TypeString[0]);
    for (int i=0; i<types; i++){
        string type_str;
        string_init_chars(&type_str, TypeString[i]);
        type_exp* exp = (type_exp*)create_type_oper(&type_str, &args);
        value_ref key = {(void*)TypeString[i], strlen(TypeString[i])+1};
        value_ref value = {(void*)&exp, sizeof(type_exp*)};
        hashtable_add_ref(&env->type_env, key, value);
    }
    return env;
}

void type_env_free(type_env* env)
{
    for (int i = 0; i < env->type_env.buckets.cap; i++) {
        hashbox* box = (hashbox*)array_get(&env->type_env.buckets, i);
        if (box->status)
            type_exp_free(*(type_exp**)hashbox_get_value(box));
    }
    hashtable_deinit(&env->type_env);
    free(env);
}

type_exp* _analyze_block(type_env* env, exp_node* node)
{
    block_node* block = (block_node*)node;
    //std::vector<type_exp*> exps;
    type_exp* exp = NULL;
    for (int i = 0; i < array_size(&block->nodes); i++) {
        exp_node *node = *(exp_node**)array_get(&block->nodes, i);
        exp = analyze(env, node);//exps.push_back(analyze(env, node));
    }
    return exp;
}

type_exp* _analyze_una(type_env* env, exp_node* una)
{
    //bin->base.value_type = var->init_value->value_type;
    return 0;
}

type_exp* _analyze_unk(type_env* env, exp_node* node)
{
    return 0;
}

type_exp* _analyze_cond(type_env* env, exp_node* node)
{
    return 0;
}

type_exp* _analyze_for(type_env* env, exp_node* node)
{
    return 0;
}

type_exp* _analyze_proto(type_env* env, exp_node* node)
{
    return 0;
}

type_exp* _analyze_fun(type_env* env, exp_node* node)
{
    function_node* fun = (function_node*)node;
    //# create a new non-generic variable for the binder
    array args;//<type_exp*> args;
    array_init(&args, sizeof(type_exp*));
    for(unsigned i = 0; i < array_size(&fun->prototype->args); i++){
        type_exp *exp = (type_exp*)create_type_var();
        array_push(&args, &exp);
        array_push(&env->nogens, &exp);
        char *arg_str = string_get((string*)array_get(&fun->prototype->args, i));
        value_ref key = {(void*)arg_str, strlen(arg_str)+1};
        value_ref value = {(void*)&exp, sizeof(type_exp*)};
        hashtable_add_ref(&env->type_env, key, value);
    }
    type_exp* result_type = analyze(env, *(exp_node**)array_back(&fun->body->nodes)); //TODO: need to recursively analyze
    type_exp* ret = (type_exp*)create_type_fun(&args, result_type);
    array_deinit(&args);
    return ret;
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
