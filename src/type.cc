/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language type inference algorithms. 
 * references: http://lucacardelli.name/Papers/BasicTypechecking.pdf
 */
#include "type.h"
#include <algorithm>

type_var* create_type_var()
{
    auto var = new type_var();
    var->base.kind = KIND_VAR;
    var->base.name = get_id_name();
    var->instance = nullptr;
    return var;
}

type_oper* create_type_oper(std::string name, std::vector<type_exp*>& args)
{
    auto var = new type_oper();
    var->base.kind = KIND_OPER;
    var->base.name = name;
    var->args = args;
    return var;
}

type_oper* create_type_fun(std::vector<type_exp*>& args, type_exp* ret)
{
    args.push_back(ret);
    return create_type_oper("->", args);
}

void type_exp_free(type_exp* type)
{
    if (type->kind == KIND_VAR) {
        auto var = (type_var*)type;
        delete var;
    }
    auto op = (type_oper*)type;
    delete op;
}

std::string format_type(type_exp* exp)
{
    if (exp->kind == KIND_VAR) {
        auto var = (type_var*)exp;
        return var->instance != nullptr ? format_type(var->instance) : exp->name;
    }
    auto op = (type_oper*)exp;
    std::string str = "";
    for (auto e : op->args) {
        str += " ";
        str += format_type(e);
    }
    return str;
}

bool is_same_type(type_exp* type1, type_exp* type2)
{
    return type1 == type2;
    //return type1->kind == type2->kind && type1->name == type2->name;
}

type_exp* prune(type_exp* type)
{
    if (type->kind == KIND_VAR) {
        auto var = (type_var*)type;
        if (var->instance) {
            var->instance = prune(var->instance);
            return var->instance;
        }
    }
    return type;
}

inline bool _occurs_in_type_list(type_exp* type1, std::vector<type_exp*> list)
{
    return any_of(list.begin(), list.end(), [&](type_exp* exp) { return occurs_in_type(type1, exp); });
}

bool occurs_in_type(type_exp* type1, type_exp* type2)
{
    type2 = prune(type2);
    if (type2->kind == KIND_VAR) {
        return is_same_type(type1, type2);
    }
    auto oper = (type_oper*)type2;
    return _occurs_in_type_list(type1, oper->args);
}

bool unify(type_exp* type1, type_exp* type2, std::vector<type_exp*>& nogens)
{
    type1 = prune(type1);
    type2 = prune(type2);
    //log(DEBUG, "unify type1 : %d: %s", type1->kind, type1->name.c_str());
    //log(DEBUG, "unify type2 : %d: %s", type2->kind, type2->name.c_str());

    //not unify a generic one with no-generic one
    if (type1->kind == KIND_VAR && type2->kind == KIND_VAR) {
        if (_occurs_in_type_list(type1, nogens) && not _occurs_in_type_list(type2, nogens))
            return unify(type2, type1, nogens);
    }

    if (type1->kind == KIND_VAR) {
        type_var* var = (type_var*)type1;
        if (occurs_in_type(type1, type2) && !is_same_type(type1, type2))
            return false;
        else {
            var->instance = type2;
        }
    } else {
        if (type2->kind == KIND_VAR) {
            return unify(type2, type1, nogens);
        } else {
            auto oper1 = (type_oper*)type1;
            auto oper2 = (type_oper*)type2;
            if (type1->name != type2->name || oper1->args.size() != oper2->args.size())
                return false;
            for (int i = 0; i < oper1->args.size(); i++) {
                if (!unify(oper1->args[i], oper2->args[i], nogens))
                    return false;
            }
        }
    }
    return true;
}

struct CopyEnv {
    type_exp* old_type;
    type_exp* new_type;
    CopyEnv* tail;
};

CopyEnv* copy_env(type_exp* old_type, type_exp* new_type, CopyEnv* tail)
{
    auto env = new CopyEnv();
    env->old_type = old_type;
    env->new_type = new_type;
    env->tail = tail;
    return env;
}

type_exp* _fresh_var(type_exp* type1, std::map<type_exp*, type_exp*>& env)
{
    if (env[type1])
        return env[type1];
    auto new_type = (type_exp*)create_type_var();
    env[new_type] = new_type;
    return new_type;
    // if (!scan){
    //   auto new_type = (type_exp*)create_type_var();
    //   env = copy_env(type1, new_type, env);
    //   return new_type;
    // }
    // else if(is_same_type(type1, scan->old_type))
    //   return scan->new_type;
    // else
    //   return _fresh_var(type1, scan->tail, env);
}

type_exp* fresh(type_exp* type1, std::vector<type_exp*>& nogen, std::map<type_exp*, type_exp*>& env)
{
    type1 = prune(type1);
    if (type1->kind == KIND_VAR) {
        if (!_occurs_in_type_list(type1, nogen)) {
            //generic
            return _fresh_var(type1, env);
        } else
            return type1;
    }
    auto op1 = (type_oper*)type1;
    std::vector<type_exp*> refreshed;
    refreshed.resize(op1->args.size());
    std::transform(op1->args.begin(), op1->args.end(), refreshed.begin(),
        [&](type_exp* x) {
            return fresh(x, nogen, env);
        });
    return (type_exp*)create_type_oper(type1->name, refreshed);
}

type_exp* retrieve(std::string name, std::vector<type_exp*>& nogen, std::map<std::string, type_exp*>& env)
{
    if (env[name]) {
        std::map<type_exp*, type_exp*> type_env;
        return fresh(env[name], nogen, type_env);
    }
    return nullptr;
}