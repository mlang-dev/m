/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language type inference algorithms. 
 * references: http://lucacardelli.name/Papers/BasicTypechecking.pdf
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "type.h"

type_var* create_type_var()
{
    string name = get_id_name();
    //printf("new id: %s\n", string_get(&name));
    type_var *var = (type_var*)malloc(sizeof(type_var));
    var->base.kind = KIND_VAR;
    var->base.name = name;
    var->instance = NULL;
    string_deinit(&name);
    return var;
}

//array of type_exp*
type_oper* create_type_oper(string *name, array* args)
{
    type_oper* var = (type_oper*)malloc(sizeof(type_oper));
    var->base.kind = KIND_OPER;
    var->base.name = *name;
    if(args)
        var->args = *args;
    return var;
}

type_oper* create_nullary_type(const char * type)
{
    string type_str;
    string_init_chars(&type_str, type);
    array args;
    array_init(&args, sizeof(type_exp*));
    return create_type_oper(&type_str, &args);
}

//args: array of type_exp*
type_oper* create_type_fun(array* args)
{
    string str;
    string_init_chars(&str, "->");
    return create_type_oper(&str, args);
}

void type_exp_free(type_exp* type)
{
    free(type);
    // if (type->kind == KIND_VAR) {
    //     //auto var = (type_var*)type;
    //     free(var);
    // }
    // auto op = (type_oper*)type;
    // delete op;
}

string format_type(type_exp* exp)
{
    if (exp->kind == KIND_VAR) {
        type_var* var = (type_var*)exp;
        return var->instance? format_type(var->instance) : exp->name;
    }
    type_oper* op = (type_oper*)exp;
    string str;
    string_init_chars(&str, "");
    for (unsigned i = 0; i < array_size(&op->args); i++) {
        type_exp *type = *(type_exp**)array_get(&op->args, i);
        string_add_chars(&str, " ");
        string str_type = format_type(type);
        string_add(&str, (string*)&str_type);
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
        type_var* var = (type_var*)type;
        assert(type!=var->instance);
        if (var->instance) {
            var->instance = prune(var->instance);
            return var->instance;
        }
    }
    return type;
}

bool _occurs_in_type_list(type_exp* type1, array* list)
{
    for (unsigned i = 0; i < array_size(list); i++){
        type_exp *type = *(type_exp**)array_get(list, i);
        if (occurs_in_type(type1, type))
            return true;
    }
    return false;
}

bool occurs_in_type(type_exp* type1, type_exp* type2)
{
    type2 = prune(type2);
    if (type2->kind == KIND_VAR) {
        return is_same_type(type1, type2);
    }
    type_oper* oper = (type_oper*)type2;
    return _occurs_in_type_list(type1, &oper->args);
}

bool unify(type_exp* type1, type_exp* type2, array *nogens)
{
    type1 = prune(type1);
    type2 = prune(type2);
    if (type1 == type2)
        return true;
    //log_info(DEBUG, "unify type1 : %d: %s", type1->kind, string_get(&type1->name));
    //log_info(DEBUG, "unify type2 : %d: %s", type2->kind, string_get(&type2->name));

    //not unify a generic one with no-generic one
    if (type1->kind == KIND_VAR && type2->kind == KIND_VAR) {
        if (_occurs_in_type_list(type1, nogens) && !_occurs_in_type_list(type2, nogens))
            return unify(type2, type1, nogens);
    }

    if (type1->kind == KIND_VAR) {
        type_var* var = (type_var*)type1;
        if (occurs_in_type(type1, type2) && !is_same_type(type1, type2))
            return false;
        else{
            // if (type2->kind == KIND_OPER){
            //     string type2_str = to_string(type2);
            //     printf("right side is: %s\n", string_get(&type2_str));
            // }
            var->instance = type2;
        }
    } else {
        if (type2->kind == KIND_VAR) {
            return unify(type2, type1, nogens);
        } else {
            type_oper* oper1 = (type_oper*)type1;
            type_oper* oper2 = (type_oper*)type2;
            if (!string_eq(&type1->name, &type2->name) || array_size(&oper1->args) != array_size(&oper2->args))
                return false;
            for (size_t i = 0; i < array_size(&oper1->args); i++) {
                if (!unify(*(type_exp**)array_get(&oper1->args, i), *(type_exp**)array_get(&oper2->args, i), nogens))
                    return false;
                //string type1_str = to_string(*(type_exp**)array_get(&oper1->args, i));
                //printf("unifyed all args: %zu, %s\n", i, string_get(&type1_str));
            }
        }
    }
    return true;
}

// struct CopyEnv {
//     type_exp* old_type;
//     type_exp* new_type;
//     CopyEnv* tail;
// };

// CopyEnv* copy_env(type_exp* old_type, type_exp* new_type, CopyEnv* tail)
// {
//     auto env = new CopyEnv();
//     env->old_type = old_type;
//     env->new_type = new_type;
//     env->tail = tail;
//     return env;
// }

type_exp* _fresh_var(type_exp* type1)//, std::map<type_exp*, type_exp*>& env)
{
    if (!type1)
        return NULL;
    //if (env[type1]) //std::map<type_exp*, type_exp*>& env
    //    return env[type1];
    type_exp* new_type = (type_exp*)create_type_var();
    //env[new_type] = new_type;
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

type_exp* fresh(type_exp* type, array* nogen)//, std::map<type_exp*, type_exp*>& env)
{
    type = prune(type);
    if (type->kind == KIND_VAR) {
        if (!_occurs_in_type_list(type, nogen)) {
            //generic
            //printf("generic type: %s\n", string_get(&type->name));
            return _fresh_var(type);//, env);
        } else
            return type;
    }
    type_oper* op = (type_oper*)type;
    array refreshed; //array of type_exp*
    array_init(&refreshed, sizeof(type_exp*));
    for(size_t i = 0; i<array_size(&op->args); i++){
        type_exp *arg_type = *(type_exp**)array_get(&op->args, i);
        //printf("fresh type: %p, %zu\n", (void*)arg_type, array_size(&op->args));
        type_exp *new_arg_type = fresh(arg_type, nogen);//, env);
        array_push(&refreshed, &new_arg_type);
    }
    return (type_exp*)create_type_oper(&type->name, &refreshed);
}

type_exp* retrieve_type(string *name, array *nogen, struct hashtable *env)
{
    type_exp* exp = (type_exp*)hashtable_get(env, string_get(name));
    if (exp) {
        return fresh(exp, nogen);
    }
    return 0;
}

string to_string(type_exp* type)
{
    type = prune(type);
    if (type->kind == KIND_VAR){
        type_var* var = (type_var*)type;
        if(var->instance){
            return to_string(var->instance);
        }else
            return var->base.name;
    }else if(type->kind == KIND_OPER){
        type_oper *oper = (type_oper*)type;
        if(array_size(&oper->args)==0) /* nullary operator, e.g. builtin types: int, double*/
            return oper->base.name;
        else{
            array array_type_strs;
            array_string_init(&array_type_strs);
            for(size_t i = 0; i < array_size(&oper->args); i++){
                string type_str = to_string(*(type_exp**)array_get(&oper->args, i));
                array_push(&array_type_strs, &type_str);
            }
            array subarray;
            array_copy_size(&subarray, &array_type_strs, array_size(&array_type_strs) - 1);
            string typestr = string_join(&subarray, " * ");
            if(string_eq_chars(&oper->base.name, "->")){
                string_add_chars(&typestr, " -> ");
            }else{
                string_add_chars(&typestr, " * ");
            }
            string_add(&typestr, (string*)array_back(&array_type_strs));
            //printf("hello: %s\n", string_get(&typestr));
            return typestr;
        }
    }else{
        assert(false);
        string empty;
        string_init(&empty);
        return empty;
    }
}
