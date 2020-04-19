/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Type Inference Analyzer
 */

#include "analyzer.h"

type_exp* retrieve(type_env* env, std::string name)
{
    return retrieve(name, env->nogens, env->type_env);
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
    unify(result_type, type, env->nogens);
    return result_type;
}

type_exp* _analyze_call(type_env* env, exp_node* node)
{
    auto call = (call_node*)node;
    auto fun_type = retrieve(env, std::string(string_get(&call->callee)));
    std::vector<type_exp*> args;
    args.resize(call->args.size());
    transform(call->args.begin(), call->args.end(), args.begin(),
        [&](exp_node* node) {
            return analyze(env, node);
        });
    type_exp* result_type = (type_exp*)create_type_var();
    unify((type_exp*)create_type_fun(args, result_type), fun_type, env->nogens);
    return result_type;
}

type_exp* _analyze_fun(type_env* env, function_node* fun)
{
    //# create a new non-generic variable for the binder
    std::vector<type_exp*> args;
    args.resize(fun->prototype->args.size());
    transform(fun->prototype->args.begin(), fun->prototype->args.end(), args.begin(),
        [](std::string arg) {
            return (type_exp*)create_type_var();
        });
    for (int i = 0; i < args.size(); i++) {
        env->nogens.push_back(args[i]);
        env->type_env[fun->prototype->args[i]] = args[i];
    }
    type_exp* result_type = analyze(env, fun->body->nodes.back()); //TODO: need to recursively analyze
    return (type_exp*)create_type_fun(args, result_type);
}

type_exp* _analyze_bin(type_env* env, exp_node* node)
{
    auto bin = (binary_node*)node;
    type_exp* lhs_type = analyze(env, bin->lhs);
    type_exp* rhs_type = analyze(env, bin->rhs);
    if (unify(lhs_type, rhs_type, env->nogens))
        return lhs_type;
    //log_info(DEBUG, "error binary op with different type");
    return nullptr;
}

type_env* type_env_new()
{
    type_env* env = new type_env();
    std::vector<type_exp*> args;
    for (auto def_type : TypeString)
        env->type_env[def_type] = (type_exp*)create_type_oper(def_type, args);
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
