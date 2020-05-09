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
#include "builtins.h"

const char * pred_ops[] = {
    "<",
    ">",
    "==",
    "!=",
    "<=",
    ">="
};

bool _is_pred_op(const char* op)
{
    for(size_t i=0; i<sizeof(pred_ops)/sizeof(pred_ops[0]);i++){
        if (strcmp(pred_ops[i], op) == 0)
            return true;
    }
    return false;
}

type_exp* _analyze_unk(type_env* env, struct exp_node* node)
{
    printf("analyzing unk: %s\n", NodeTypeString[node->node_type]);
    if (!env || !node)
        return 0;
    return 0;
}

type_exp* retrieve(type_env* env, const char *name)
{
    return retrieve_type(name, &env->nogens, &env->type_env);
}

void set(type_env* env, const char *name, type_exp* type)
{
     set_type(&env->type_env, name, type);
}

type_exp* _analyze_ident(type_env* env, struct exp_node* ident)
{
    return retrieve(env, string_get(&((struct ident_node*)ident)->name));
}

type_exp* _analyze_num(type_env* env, struct exp_node* node)
{
    return retrieve(env, TypeString[node->annotated_type]);
}

type_exp* _analyze_var(type_env* env, struct exp_node* node)
{
    struct var_node* var = (struct var_node*)node;
    type_exp* type = analyze(env, var->init_value);
    if(!type)
        return 0;
    type_exp* result_type = (type_exp*)create_type_var();
    unify(result_type, type, &env->nogens);
    //printf("analyzing var done: %s\n", string_get(&var->var_name));
    set(env, string_get(&var->var_name), result_type);
    return result_type;
}

type_exp* _analyze_call(type_env* env, struct exp_node* node)
{
    struct call_node* call = (struct call_node*)node;
    type_exp* fun_type = retrieve(env, string_get(&call->callee));
    assert(fun_type);
    array args;
    array_init(&args, sizeof(type_exp*));
    for(size_t i = 0; i < array_size(&call->args); i++){
        struct exp_node* arg = *(struct exp_node**)array_get(&call->args, i);
        type_exp *type = analyze(env, arg);
        array_push(&args, &type);
    }
    type_exp* result_type = (type_exp*)create_type_var();
    array_push(&args, &result_type);
    //string fun_type_str = to_string(fun_type);
    //printf("analyzing call: %s, %s\n", string_get(&call->callee), string_get(&fun_type_str));
    unify((type_exp*)create_type_fun(&args), fun_type, &env->nogens);
    // string result = to_string(result_type);
    // printf("returning called type: %s\n", string_get(&result));
    return result_type;
}


type_exp* _analyze_una(type_env* env, struct exp_node* node)
{
    struct unary_node* unary = (struct unary_node*)node;
    type_exp* op_type = analyze(env, unary->operand);
    if(string_eq_chars(&unary->op, "!")){
        type_exp* bool_type = (type_exp*)create_nullary_type(TYPE_BOOL);
        unify(op_type, bool_type, &env->nogens);
        unary->operand->type = op_type;
    }
    return op_type;
}

type_exp* _analyze_bin(type_env* env, struct exp_node* node)
{
    struct binary_node* bin = (struct binary_node*)node;
    type_exp* lhs_type = analyze(env, bin->lhs);
    type_exp* rhs_type = analyze(env, bin->rhs);
    type_exp* result = 0;
    if(unify(lhs_type, rhs_type, &env->nogens)){
        if (_is_pred_op(string_get(&bin->op)))
            result = (type_exp*)create_nullary_type(TYPE_BOOL);
        else
            result = lhs_type;
        return result;
    }
    return result;
}

type_exp* _analyze_cond(type_env* env, struct exp_node* node)
{
    struct condition_node *cond_node = (struct condition_node*)node;
    type_exp *cond_type = analyze(env, cond_node->condition_node);
    type_oper *bool_type = create_nullary_type(TYPE_BOOL);
    unify(cond_type, (type_exp*)bool_type, &env->nogens);
    type_exp *then_type = analyze(env, cond_node->then_node);
    type_exp *else_type = analyze(env, cond_node->else_node);
    unify(then_type, else_type, &env->nogens);
    return then_type;
}

type_exp* _analyze_for(type_env* env, struct exp_node* node)
{
    struct for_node *f_node = (struct for_node*)node;
    type_exp *int_type = (type_exp*)create_nullary_type(TYPE_INT);
    type_exp* start_type = analyze(env, f_node->start);
    type_exp* step_type = analyze(env, f_node->step);
    struct binary_node* bin = (struct binary_node*)f_node->end;
    type_exp* end_type = analyze(env, bin->rhs);
    unify(start_type, int_type, &env->nogens);
    unify(step_type, int_type, &env->nogens);
    unify(end_type, int_type, &env->nogens);
    return (type_exp*)create_nullary_type(TYPE_UNIT);
}

type_env* type_env_new(void* context)
{
    type_env* env = malloc(sizeof(*env));
    memset((void*)env, 0, sizeof(type_env));
    array_init(&env->nogens, sizeof(type_exp*));
    hashtable_init(&env->type_env);
    array args;
    array_init(&args, sizeof(type_exp*));
    size_t types = sizeof(TypeString)/sizeof(TypeString[0]);
    for (size_t i=0; i<types; i++){
        type_exp* exp = (type_exp*)create_type_oper(i, &args);
        set(env, TypeString[i], exp);
    }
    type_exp* double_type = (type_exp*)create_nullary_type(TYPE_DOUBLE);
    array double_args;
    array_init(&double_args, sizeof(type_exp*));
    array_push(&double_args, &double_type);
    array_push(&double_args, &double_type);
    array builtins = get_builtins(context);
    for(size_t i = 0; i < array_size(&builtins); i++){
        struct prototype_node* proto = *(struct prototype_node**)array_get(&builtins, i);
        type_exp* exp = (type_exp*)create_type_fun(&double_args);
        set(env, string_get(&proto->name), exp);
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

type_exp* _analyze_block(type_env* env, struct exp_node* node)
{
    struct block_node* block = (struct block_node*)node;
    //std::vector<type_exp*> exps;
    type_exp* exp = 0;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node *node = *(struct exp_node**)array_get(&block->nodes, i);
        exp = analyze(env, node);//exps.push_back(analyze(env, node));
    }
    return exp;
}

type_exp* _analyze_proto(type_env* env, struct exp_node* node)
{
    printf("analyzing proto: %s\n", NodeTypeString[node->node_type]);
    if (!env || !node)
        return 0;
    return 0;
}

type_exp* _analyze_fun(type_env* env, struct exp_node* node)
{
    struct function_node* fun = (struct function_node*)node;
    //# create a new non-generic variable for the binder
    array args;//<type_exp*> args;
    array_init(&args, sizeof(type_exp*));
    for(size_t i = 0; i < array_size(&fun->prototype->args); i++){
        type_exp *exp = (type_exp*)create_type_var();
        array_push(&args, &exp);
        array_push(&env->nogens, &exp);
        const char *arg_str = string_get((string*)array_get(&fun->prototype->args, i));
        set(env, arg_str, exp);
        //printf("setting fun arg: %s\n", arg_str);
    }
    type_exp *fun_type = (type_exp*)create_type_var();
    set(env, string_get(&fun->prototype->name), fun_type);
    type_exp* ret_type = analyze(env, (struct exp_node*)fun->body); 
    array_push(&args, &ret_type);
    type_exp* result_type = (type_exp*)create_type_fun(&args);
    unify(fun_type, result_type, &env->nogens);
    type_exp* result = prune(fun_type);
    fun->prototype->base.type = result;
    return result;
}

type_exp* (*analyze_fp[])(type_env*, struct exp_node*) = {
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

type_exp* analyze(type_env* env, struct exp_node* node)
{
    type_exp *type = analyze_fp[node->node_type](env, node);
    node->type = type;
    return type;
}
