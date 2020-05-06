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
#include "clib/hashtable.h"

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

bool _occurs_in_type_list(type_var* var, array* list)
{
    for (unsigned i = 0; i < array_size(list); i++){
        type_exp *type = *(type_exp**)array_get(list, i);
        if (occurs_in_type(var, type))
            return true;
    }
    return false;
}

bool occurs_in_type(type_var* var, type_exp* type2)
{
    type2 = prune(type2);
    if (type2->kind == KIND_VAR) {
        return (type_exp*)var == type2;
    }
    type_oper* oper = (type_oper*)type2;
    return _occurs_in_type_list(var, &oper->args);
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
        if (_occurs_in_type_list((type_var*)type1, nogens) && !_occurs_in_type_list((type_var*)type2, nogens))
            return unify(type2, type1, nogens);
    }

    if (type1->kind == KIND_VAR) {
        type_var* var = (type_var*)type1;
        if (occurs_in_type(var, type2) && type1 != type2)
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

bool _is_generic(type_var* var, array* nogen)
{
    return !_occurs_in_type_list(var, nogen);
}

type_exp* _freshrec(type_exp* type, array* nogen, struct hashtable *type_vars)
{
    type = prune(type);
    if (type->kind == KIND_VAR) {
        type_var *var = (type_var*)type;
        if (_is_generic(var, nogen)) {
            //printf("generic type: %s\n", string_get(&type->name));
            type_var *temp = hashtable_get_p(type_vars, var);
            if (!temp){
                temp = create_type_var();
                hashtable_set_p(type_vars, var, temp);
            }
            return (type_exp*)temp;
        } else
            return type;
    }
    type_oper* op = (type_oper*)type;
    array refreshed; //array of type_exp*
    array_init(&refreshed, sizeof(type_exp*));
    for(size_t i = 0; i<array_size(&op->args); i++){
        type_exp *arg_type = *(type_exp**)array_get(&op->args, i);
        //printf("fresh type: %p, %zu\n", (void*)arg_type, array_size(&op->args));
        type_exp *new_arg_type = _freshrec(arg_type, nogen, type_vars);//, env);
        array_push(&refreshed, &new_arg_type);
    }
    return (type_exp*)create_type_oper(&type->name, &refreshed);
}

type_exp* fresh(type_exp* type, array* nogen)
{
    struct hashtable type_vars;
    hashtable_init(&type_vars);
    type_exp* result = _freshrec(type, nogen, &type_vars);
    hashtable_deinit(&type_vars);
    return result;
}

type_exp* retrieve_type(string *name, array *nogen, struct hashtable *env)
{
    type_exp* exp = (type_exp*)hashtable_get(env, string_get(name));
    if (exp) {
        return fresh(exp, nogen);
    }
    return 0;
}

void set_type(struct hashtable *env, const char *name, type_exp* type)
{
    hashtable_set(env, name, type);
}

string to_string(type_exp* type)
{
    if(!type){
        string error;
        string_init_chars(&error, "type error");
        return error;
    }
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
        printf("type kind: %d\n", type->kind);
        assert(false);
    }
}
