/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language type inference algorithms. 
 * references: http://lucacardelli.name/Papers/BasicTypechecking.pdf
 */
#include "sema/type.h"
#include "clib/hashtable.h"
#include "clib/symboltable.h"
#include <assert.h>
#include <stdio.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

const char *kind_strings[] = {
    FOREACH_KIND(GENERATE_ENUM_STRING)
};

const char *const _type_strings[] = {
    "",
    "...",
    "()",
    "bool",
    "char",
    "int",
    "float",
    "double",
    "string",
    "->",
    "struct",
    "union",
    "complex",
    "enum",
    "ref"
};

struct symbol_ref_pair{
    symbol type_symbol; 
    symbol ref_type_symbol;
};

struct symbol_ref_pair type_symbols[TYPE_TYPES] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},

    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},

    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
};

/*symbol 2 type exprs*/
struct hashtable g_symbol_2_type_exprs; 
struct hashtable g_ref_symbol_2_type_exprs; 

void _free_type_expr(void *type)
{
    type_exp_free(type);
}

void types_init()
{
    for(int i = 0; i < TYPE_TYPES; i++){
        type_symbols[i].type_symbol = to_symbol(_type_strings[i]);
        string str;
        string_init_chars(&str, "&");
        string_add_chars(&str, _type_strings[i]);
        type_symbols[i].ref_type_symbol = to_symbol(string_get(&str));
        string_deinit(&str);
    }
    hashtable_init_with_value_size(&g_symbol_2_type_exprs, 0, _free_type_expr);
    hashtable_init_with_value_size(&g_ref_symbol_2_type_exprs, 0, _free_type_expr);
}

void types_deinit()
{
    hashtable_deinit(&g_symbol_2_type_exprs);
    hashtable_deinit(&g_ref_symbol_2_type_exprs);
}

struct type_var *_create_type_var(symbol name)
{
    struct type_var *var;
    MALLOC(var, sizeof(*var));
    var->base.kind = KIND_VAR;
    var->base.type = 0;
    var->base.name = name;
    var->base.ref_type = 0;
    var->instance = 0;
    return var;
}

struct type_var *create_type_var()
{
    string name = get_id_name();
    symbol type_name = to_symbol(string_get(&name));
    return _create_type_var(type_name);
}

struct type_var *copy_type_var(struct type_var *var)
{
    struct type_var *copy_var = _create_type_var(var->base.name);
    copy_var->instance = var->instance;
    copy_var->base.type = var->base.type;
    return copy_var;
}

struct type_oper *create_type_oper(symbol type_name, enum type type, struct type_expr *ref_type, struct array *args)
{
    struct type_oper *oper;
    MALLOC(oper, sizeof(*oper));
    oper->base.kind = KIND_OPER;
    oper->base.type = type;
    oper->base.name = type_name;
    oper->base.ref_type = ref_type;
    if(args){
        oper->args = *args;
    } else {
        array_init(&oper->args, sizeof(struct type_expr *));
    }
    return oper;
}

/*ref_type: referenced type: e.g. it's int for &int type */
struct type_expr *create_ref_type(struct type_expr *ref_type)
{
    symbol ref_type_name = get_ref_type_symbol(ref_type->type);
    return (struct type_expr *)create_type_oper(ref_type_name, TYPE_REF, ref_type, 0);
}

struct type_oper *create_unit_type()
{
    symbol type_name = get_type_symbol(TYPE_UNIT);
    struct type_oper *oper = hashtable_get_p(&g_symbol_2_type_exprs, type_name);
    if(oper)
        return oper;
    MALLOC(oper, sizeof(*oper));
    oper->base.kind = KIND_OPER;
    oper->base.type = TYPE_UNIT;
    oper->base.name = type_name;
    oper->base.ref_type = 0;
    array_init(&oper->args, sizeof(struct type_expr *));
    hashtable_set_p(&g_symbol_2_type_exprs, type_name, oper);
    return oper;
}

struct type_oper *create_type_oper_struct(symbol type_name, struct array *args)
{
    return create_type_oper(type_name, TYPE_STRUCT, 0, args);
}

struct type_oper *create_nullary_type(enum type type, symbol type_symbol)
{
    assert(type_symbol);
    struct type_oper *oper;
    oper = hashtable_get_p(&g_symbol_2_type_exprs, type_symbol);
    if(oper)
        return oper;
    struct array args;
    array_init(&args, sizeof(struct type_expr *));
    oper = create_type_oper(type_symbol, type, 0, &args);
    hashtable_set_p(&g_symbol_2_type_exprs, type_symbol, oper);
    return oper;
}

struct type_oper *create_type_fun(struct array *args)
{
    symbol type_name = get_type_symbol(TYPE_FUNCTION);
    return create_type_oper(type_name, TYPE_FUNCTION, 0, args);
}

struct type_oper *wrap_as_fun_type(struct type_oper *oper)
{
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_expr *));
    array_push(&fun_sig, &oper);
    return create_type_fun(&fun_sig);
}

void type_exp_free(struct type_expr *type)
{
    if(type->kind == KIND_OPER){
        struct type_oper *to = (struct type_oper *)type;
        array_deinit(&to->args);
    }
    FREE(type);
}

struct type_expr *prune(struct type_expr *type)
{
    if (!type)
        return type;
    if (type->kind == KIND_VAR) {
        struct type_var *var = (struct type_var *)type;
        assert(type != var->instance);
        if (var->instance) {
            var->instance = prune(var->instance);
            return var->instance;
        }
    } else {
        assert(type->kind == KIND_OPER);
        struct type_oper *op_type = (struct type_oper *)type;
        struct type_expr *argt;
        for (unsigned i = 0; i < array_size(&op_type->args); ++i) {
            argt = *(struct type_expr **)array_get(&op_type->args, i);
            struct type_expr *element_type = prune(argt);
            array_set(&op_type->args, i, &element_type);
        }
    }
    return type;
}

bool _occurs_in_type_list(struct type_var *var, struct array *list)
{
    for (unsigned i = 0; i < array_size(list); i++) {
        struct type_expr *type = *(struct type_expr **)array_get(list, i);
        if (occurs_in_type(var, type))
            return true;
    }
    return false;
}

/* whether the type variable is in any of the type2, true - found*/
bool occurs_in_type(struct type_var *var, struct type_expr *type2)
{
    type2 = prune(type2);
    if (type2->kind == KIND_VAR) {
        return (struct type_expr *)var == type2;
    }
    struct type_oper *oper = (struct type_oper *)type2;
    return _occurs_in_type_list(var, &oper->args);
}

bool _is_variadic(struct array *args)
{
    size_t size = array_size(args);
    if (size > 1) {
        struct type_expr *exp = *(struct type_expr **)array_get(args, size - 2);
        return get_type(exp) == TYPE_GENERIC;
    }
    return false;
}

bool _is_valid_args_size(struct array *args1, struct array *args2)
{
    if (_is_variadic(args1)) {
        return array_size(args2) >= array_size(args1) - 1;
    } else if (_is_variadic(args2)) {
        return array_size(args1) >= array_size(args2) - 1;
    }
    return array_size(args1) == array_size(args2);
}

bool unify(struct type_expr *type1, struct type_expr *type2, struct array *nongens)
{
    type1 = prune(type1);
    type2 = prune(type2);
    assert(type1 && type2);
    /*type1 and type2 are the same one*/
    if (type1 == type2)
        return true;
    if (type2->kind == KIND_VAR){
        if (type1->kind != KIND_VAR){
            return unify(type2, type1, nongens);
        } else {
            /*type1 is non-generic, type2 is generic, swap the arguments, and the first is generic, the second one is not*/
            if (_occurs_in_type_list((struct type_var *)type1, nongens) && !_occurs_in_type_list((struct type_var *)type2, nongens))
                return unify(type2, type1, nongens);
            else {
                struct type_var *var = (struct type_var *)type1;
                var->instance = type2;//assign the type2 to type1 variable
            }
        }
    }
    if (type1->kind == KIND_VAR) {
        struct type_var *var = (struct type_var *)type1;
        assert(!occurs_in_type(var, type2) || type1 == type2);
        var->instance = type2;
    } else {
        /*type1 is known type: type_oper*/
        struct type_oper *oper1 = (struct type_oper *)type1;
        struct type_oper *oper2 = (struct type_oper *)type2;
        if (type1->ref_type != type2->ref_type || type1->type != type2->type || !_is_valid_args_size(&oper1->args, &oper2->args))
            return false;
        size_t arg_size1 = array_size(&oper1->args);
        size_t arg_size2 = array_size(&oper2->args);
        size_t arg_size = MIN(arg_size1, arg_size2);
        for (size_t i = 0; i < arg_size; i++) {
            unify(*(struct type_expr **)array_get(&oper1->args, i == arg_size - 1 ? arg_size1 - 1 : i),
                *(struct type_expr **)array_get(&oper2->args, i == arg_size - 1 ? arg_size2 - 1 : i), nongens);
        }
    }
    return true;
}

bool _is_generic(struct type_var *var, struct array *nongens)
{
    return !_occurs_in_type_list(var, nongens);
}

bool _all_is_oper(struct array *arr)
{
    for (size_t i = 0; i < array_size(arr); i++) {
        struct type_expr *type = (struct type_expr *)array_get(arr, i);
        if (type->kind != KIND_OPER)
            return false;
    }
    return true;
}

/*for any generic type in the type, create a a type variable thunk for it*/
struct type_expr *_freshrec(struct type_expr *type, struct array *nongens, struct hashtable *type_vars)
{
    type = prune(type);
    if (type->kind == KIND_VAR) {
        struct type_var *var = (struct type_var *)type;
        if (_is_generic(var, nongens)) {
            struct type_var *temp = hashtable_get_p(type_vars, var);
            if (!temp) {
                temp = create_type_var();
                hashtable_set_p(type_vars, var, temp);
            }
            return (struct type_expr *)temp;
        } else
            return type;
    }
    struct type_oper *op = (struct type_oper *)type;
    if (array_size(&op->args) == 0 || _all_is_oper(&op->args))
        return type;
    struct array refreshed;
    array_init(&refreshed, sizeof(struct type_expr *));
    for (size_t i = 0; i < array_size(&op->args); i++) {
        struct type_expr *arg_type = *(struct type_expr **)array_get(&op->args, i);
        struct type_expr *new_arg_type = _freshrec(arg_type, nongens, type_vars);
        array_push(&refreshed, &new_arg_type);
    }
    if (type->type == TYPE_STRUCT) {
        return (struct type_expr *)create_type_oper_struct(type->name, &refreshed);
    }
    return (struct type_expr *)create_type_oper(type->name, type->type, 0, &refreshed);
}

struct type_expr *fresh(struct type_expr *type, struct array *nongens)
{
    struct hashtable type_vars;
    hashtable_init(&type_vars);
    struct type_expr *result = _freshrec(type, nongens, &type_vars);
    hashtable_deinit(&type_vars);
    return result;
}

struct type_expr *get_symbol_type(symboltable *st, struct array *nongens, symbol name)
{
    struct type_expr *exp = (struct type_expr *)symboltable_get(st, name);
    if (!exp){
        printf("No type is found for the symbol: %s.\n", string_get(name));
        return 0;
    }
    return fresh(exp, nongens);
    
    
}

void push_symbol_type(symboltable *st, symbol name, void *type)
{
    symboltable_push(st, name, type);
}

enum type get_type(struct type_expr *type)
{
    type = prune(type);
    assert(type && type->type >= 0 && type->type < TYPE_TYPES);
    return type->type;
}

string to_string(struct type_expr *type)
{
    string typestr;
    string_init_chars(&typestr, "");
    if (!type) {
        string_add_chars(&typestr, "type mismatch");
        return typestr;
    }
    type = prune(type);
    if (type->kind == KIND_VAR) {
        struct type_var *var = (struct type_var *)type;
        if (var->instance) {
            return to_string(var->instance);
        } else {
            string_copy(&typestr, var->base.name);
            return typestr;
        }
    } else if (type->kind == KIND_OPER) {
        struct type_oper *oper = (struct type_oper *)type;
        if (array_size(&oper->args) == 0) { /* nullary operator, e.g. builtin types: int, double*/
            string_add(&typestr, get_type_symbol(oper->base.type));
            return typestr;
        } else if (oper->base.type == TYPE_STRUCT) {
            string_copy(&typestr, oper->base.name);
            return typestr;
        } else {
            ARRAY_STRING(array_type_strs);
            for (size_t i = 0; i < array_size(&oper->args); i++) {
                string type_str = to_string(*(struct type_expr **)array_get(&oper->args, i));
                array_push(&array_type_strs, &type_str);
            }
            struct array subarray;
            array_copy_size(&subarray, &array_type_strs, array_size(&array_type_strs) - 1);
            typestr = string_join(&subarray, " * ");
            if (oper->base.type == TYPE_FUNCTION) {
                if (array_size(&subarray) == 0)
                    string_add_chars(&typestr, "()");
                string_add_chars(&typestr, " -> ");
            } else {
                string_add_chars(&typestr, " * ");
            }
            string_add(&typestr, (string *)array_back(&array_type_strs));
            return typestr;
        }
    } else {
        print_backtrace();
        assert(false);
    }
    return typestr;
}

bool is_generic(struct type_expr *type)
{
    type = prune(type);
    if (type->kind == KIND_VAR)
        return true;
    struct type_oper *op = (struct type_oper *)type;
    struct type_expr *argt;
    for (size_t i = 0; i < array_size(&op->args); i++) {
        argt = *(struct type_expr **)array_get(&op->args, i);
        type = prune(argt);
        if (type->kind == KIND_VAR)
            return true;
    }
    return false;
}

bool is_any_generic(struct array *types)
{
    for (size_t i = 0; i < array_size(types); i++) {
        struct type_expr *exp = *(struct type_expr **)array_get(types, i);
        if (is_generic(exp))
            return true;
    }
    return false;
}

string monomorphize(const char *fun_name, struct array *types)
{
    string sp;
    string_init_chars(&sp, "__");
    string_add_chars(&sp, fun_name);
    for (size_t i = 0; i < array_size(types); i++) {
        struct type_expr *type = *(struct type_expr **)array_get(types, i);
        string type_str = to_string(type);
        string_add_chars(&sp, "_");
        string_add(&sp, &type_str);
    }
    return sp;
}

struct type_expr *clone_type(struct type_expr *type)
{
    type = prune(type);
    struct type_expr *copy = 0;
    if (type->kind == KIND_VAR) {
        copy = (struct type_expr *)copy_type_var((struct type_var *)type);
    } else if (type->kind == KIND_OPER) {
        struct type_oper *oper = (struct type_oper *)type;
        struct array args;
        array_init(&args, sizeof(struct type_expr *));
        for (size_t i = 0; i < array_size(&oper->args); i++) {
            struct type_expr *arg = clone_type(*(struct type_expr **)array_get(&oper->args, i));
            array_push(&args, &arg);
        }
        copy = (struct type_expr *)create_type_oper(oper->base.name, oper->base.type, 0, &args);
    } else {
        assert(false);
    }
    return copy;
}

bool is_promotable_int(struct type_expr *type)
{
    return type->type == TYPE_CHAR || type->type == TYPE_CHAR;
}

u8 type_size(enum type type)
{
    return type == TYPE_DOUBLE ? 8 : 4;
}

bool is_empty_struct(struct type_expr *type)
{
    struct type_oper *to = (struct type_oper *)type;
    if(!is_aggregate_type(type->type)) 
        return false;
    if(array_size(&to->args) == 0){
        return true;
    }
    for(size_t i = 0; i < array_size(&to->args); i++){
        type = *(struct type_expr **)array_get(&to->args, i);
        if(!is_empty_struct(type)){
            return false;
        }
    }
    return true;
}

struct type_expr *is_single_element_struct(struct type_expr *type)
{
    struct type_oper *to = (struct type_oper *)type;
    if(array_size(&to->args) == 0){
        return 0;
    }
    struct type_expr *found = 0;
    for(size_t i = 0; i < array_size(&to->args); i++){
        type = *(struct type_expr **)array_get(&to->args, i);
        if(is_empty_struct(type)){
            continue;
        }
        if(found){
            //already found 
            return 0;
        }
        found = is_single_element_struct(type);
        if(!found){
            //not empty and no single element, then it's not single element
            return 0;
        }
    }
    return found;

}

symbol get_type_symbol(enum type type_enum)
{
    return type_symbols[type_enum].type_symbol;
}

symbol get_ref_type_symbol(enum type type_enum)
{
    return type_symbols[type_enum].ref_type_symbol;
}
