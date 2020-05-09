/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language type inference algorithms. 
 * references: http://lucacardelli.name/Papers/BasicTypechecking.pdf
 */
#include "type.h"
#include "clib/hashtable.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct type_var* create_type_var()
{
    string name = get_id_name();
    struct type_var* var = malloc(sizeof(*var));
    var->base.kind = KIND_VAR;
    var->base.type = 0;
    var->name = name;
    var->instance = 0;
    return var;
}

struct type_oper* create_type_oper(enum type type, struct array* args)
{
    struct type_oper* oper = malloc(sizeof(*oper));
    oper->base.kind = KIND_OPER;
    oper->base.type = type;
    oper->args = *args;
    return oper;
}

struct type_oper* create_nullary_type(enum type type)
{
    struct array args;
    array_init(&args, sizeof(struct type_exp*));
    return create_type_oper(type, &args);
}

struct type_oper* create_type_fun(struct array* args)
{
    return create_type_oper(TYPE_FUNCTION, args);
}

void type_exp_free(struct type_exp* type)
{
    free(type);
}

struct type_exp* prune(struct type_exp* type)
{
    if (type->kind == KIND_VAR) {
        struct type_var* var = (struct type_var*)type;
        assert(type != var->instance);
        if (var->instance) {
            var->instance = prune(var->instance);
            return var->instance;
        }
    }
    return type;
}

bool _occurs_in_type_list(struct type_var* var, struct array* list)
{
    for (unsigned i = 0; i < array_size(list); i++) {
        struct type_exp* type = *(struct type_exp**)array_get(list, i);
        if (occurs_in_type(var, type))
            return true;
    }
    return false;
}

bool occurs_in_type(struct type_var* var, struct type_exp* type2)
{
    type2 = prune(type2);
    if (type2->kind == KIND_VAR) {
        return (struct type_exp*)var == type2;
    }
    struct type_oper* oper = (struct type_oper*)type2;
    return _occurs_in_type_list(var, &oper->args);
}

bool unify(struct type_exp* type1, struct type_exp* type2, struct array* nogens)
{
    type1 = prune(type1);
    type2 = prune(type2);
    if (type1 == type2)
        return true;
    if (type1->kind == KIND_VAR && type2->kind == KIND_VAR) {
        if (_occurs_in_type_list((struct type_var*)type1, nogens) && !_occurs_in_type_list((struct type_var*)type2, nogens))
            return unify(type2, type1, nogens);
    }

    if (type1->kind == KIND_VAR) {
        struct type_var* var = (struct type_var*)type1;
        if (occurs_in_type(var, type2) && type1 != type2)
            return false;
        else {
            var->instance = type2;
        }
    } else {
        if (type2->kind == KIND_VAR) {
            return unify(type2, type1, nogens);
        } else {
            struct type_oper* oper1 = (struct type_oper*)type1;
            struct type_oper* oper2 = (struct type_oper*)type2;
            if (type1->type != type2->type || array_size(&oper1->args) != array_size(&oper2->args))
                return false;
            for (size_t i = 0; i < array_size(&oper1->args); i++) {
                if (!unify(*(struct type_exp**)array_get(&oper1->args, i), *(struct type_exp**)array_get(&oper2->args, i), nogens))
                    return false;
            }
        }
    }
    return true;
}

bool _is_generic(struct type_var* var, struct array* nogen)
{
    return !_occurs_in_type_list(var, nogen);
}

struct type_exp* _freshrec(struct type_exp* type, struct array* nogen, struct hashtable* type_vars)
{
    type = prune(type);
    if (type->kind == KIND_VAR) {
        struct type_var* var = (struct type_var*)type;
        if (_is_generic(var, nogen)) {
            struct type_var* temp = hashtable_get_p(type_vars, var);
            if (!temp) {
                temp = create_type_var();
                hashtable_set_p(type_vars, var, temp);
            }
            return (struct type_exp*)temp;
        } else
            return type;
    }
    struct type_oper* op = (struct type_oper*)type;
    if (array_size(&op->args) == 0)
        return type;
    struct array refreshed;
    array_init(&refreshed, sizeof(struct type_exp*));
    for (size_t i = 0; i < array_size(&op->args); i++) {
        struct type_exp* arg_type = *(struct type_exp**)array_get(&op->args, i);
        struct type_exp* new_arg_type = _freshrec(arg_type, nogen, type_vars);
        array_push(&refreshed, &new_arg_type);
    }
    return (struct type_exp*)create_type_oper(type->type, &refreshed);
}

struct type_exp* fresh(struct type_exp* type, struct array* nogen)
{
    struct hashtable type_vars;
    hashtable_init(&type_vars);
    struct type_exp* result = _freshrec(type, nogen, &type_vars);
    hashtable_deinit(&type_vars);
    return result;
}

struct type_exp* retrieve_type(const char* name, struct array* nogen, struct hashtable* env)
{
    struct type_exp* exp = (struct type_exp*)hashtable_get(env, name);
    if (exp) {
        return fresh(exp, nogen);
    }
    return 0;
}

void set_type(struct hashtable* env, const char* name, struct type_exp* type)
{
    hashtable_set(env, name, type);
}

string to_string(struct type_exp* type)
{
    if (!type) {
        string error;
        string_init_chars(&error, "type mismatch");
        return error;
    }
    type = prune(type);
    if (type->kind == KIND_VAR) {
        struct type_var* var = (struct type_var*)type;
        if (var->instance) {
            return to_string(var->instance);
        } else
            return var->name;
    } else if (type->kind == KIND_OPER) {
        struct type_oper* oper = (struct type_oper*)type;
        if (array_size(&oper->args) == 0) { /* nullary operator, e.g. builtin types: int, double*/
            string builtin;
            string_init_chars(&builtin, type_strings[oper->base.type]);
            return builtin;
        } else {
            ARRAY_STRING(array_type_strs);
            for (size_t i = 0; i < array_size(&oper->args); i++) {
                string type_str = to_string(*(struct type_exp**)array_get(&oper->args, i));
                array_push(&array_type_strs, &type_str);
            }
            struct array subarray;
            array_copy_size(&subarray, &array_type_strs, array_size(&array_type_strs) - 1);
            string typestr = string_join(&subarray, " * ");
            if (oper->base.type == TYPE_FUNCTION) {
                string_add_chars(&typestr, " -> ");
            } else {
                string_add_chars(&typestr, " * ");
            }
            string_add(&typestr, (string*)array_back(&array_type_strs));
            return typestr;
        }
    } else {
        assert(false);
    }
}
