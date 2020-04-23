/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Type Inference Analyzer
 */

#include "analyzer.h"

type_exp* retrieve(type_env* env, std::string name)
{
    return env->type_env[name];
    //return retrieve(name, env->nogens, env->type_env);
}

type_exp* _analyze_ident(type_env* env, exp_node* ident)
{
    std::string idname(string_get(&((ident_node*)ident)->name));
    return retrieve(env, idname);
}

type_exp* _analyze_num(type_env* env, exp_node* num)
{
    return retrieve(env, ((num_node*)num)->base.type.name);
}

type_exp* _analyze_var(type_env* env, exp_node* node)
{
    auto var = (var_node*)node;
    auto type = analyze(env, var->init_value);
    if (!type)
        return type;
    type_exp* result_type = (type_exp*)create_type_var();
    unify(result_type, type, &env->nogens);
    return result_type;
}

type_exp* _analyze_call(type_env* env, exp_node* node)
{
    auto call = (call_node*)node;
    auto fun_type = retrieve(env, std::string(string_get(&call->callee)));
    array args;
    array_init(&args, sizeof(object));
    for(auto arg: call->args){
        type_exp *type = analyze(env, arg);
        object o = make_ref(type);
        array_push(&args, &o);
    }
    type_exp* result_type = (type_exp*)create_type_var();
    unify((type_exp*)create_type_fun(&args, result_type), fun_type, &env->nogens);
    array_deinit(&args);
    return result_type;
}

type_exp* _analyze_fun(type_env* env, function_node* fun)
{
    //# create a new non-generic variable for the binder
    array args;//<type_exp*> args;
    array_init(&args, sizeof(object));
    //args.resize(fun->prototype->args.size());
    for(unsigned i = 0; i < array_size(&fun->prototype->args); i++){
        type_exp *exp = (type_exp*)create_type_var();
        object o = make_ref(exp);
        array_push(&args, &o);
        array_push(&env->nogens, &o);
        env->type_env[std::string(string_get((string*)array_get(&fun->prototype->args, i)))] = exp;
    }
    type_exp* result_type = analyze(env, fun->body->nodes.back()); //TODO: need to recursively analyze
    type_exp* ret = (type_exp*)create_type_fun(&args, result_type);
    array_deinit(&args);
    return ret;
}

type_exp* _analyze_bin(type_env* env, exp_node* node)
{
    auto bin = (binary_node*)node;
    type_exp* lhs_type = analyze(env, bin->lhs);
    type_exp* rhs_type = analyze(env, bin->rhs);
    if (unify(lhs_type, rhs_type, &env->nogens))
        return lhs_type;
    //log_info(DEBUG, "error binary op with different type");
    return nullptr;
}

type_env* type_env_new()
{
    type_env* env = new type_env();
    array_init(&env->nogens, sizeof(object));
    array args;
    array_init(&args, sizeof(object));
    for (auto def_type : TypeString)
        env->type_env[def_type] = (type_exp*)create_type_oper(def_type, &args);
    return env;
}

void type_env_free(type_env* env)
{
    for (auto type : env->type_env) {
        type_exp_free(type.second);
    }
    delete env;
}

type_exp* _analyze_block(type_env* env, exp_node* node)
{
    auto block = (block_node*)node;
    std::vector<type_exp*> exps;
    for (auto node : block->nodes) {
        exps.push_back(analyze(env, node));
    }
    return exps.back();
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
    return 0;
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
