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

const char *const _type_strings[TYPE_TYPES] = {
    "",
    "...",
    "()",
    "bool",
    "char",
    "i8",
    "u8",
    "i16",
    "u16",
    "i32",
    "u32",
    "i64",
    "u64",
    "int",
    "f32",
    "f64",
    "string",
    "->",
    "struct",
    "array",
    "union",
    "complex",
    "ref",
};

#define IS_TYPE_CONVERTIBLE(type)  (type > TYPE_UNIT && type < TYPE_STRING)

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

struct type_expr_pair {
    struct type_expr *val_type;
    struct type_expr *ref_type;
};

symbol to_ref_symbol(symbol type_symbol)
{
    symbol ref_symbol;
    string str;
    string_init_chars(&str, "&");
    string_add(&str, type_symbol);
    ref_symbol = to_symbol(string_get(&str));
    string_deinit(&str);
    return ref_symbol;
}

symbol _to_fun_type_name(struct array *types)
{
    if(!array_size(types)){
        return 0;
    }
    struct type_expr *result_type = *(struct type_expr**)array_back(types);
    if (result_type->kind == KIND_VAR) return 0;
    string str;
    string_init(&str);
    struct type_expr *param_type;
    for(size_t i = 0; i < array_size(types) - 1; i++){
        param_type = *(struct type_expr**)array_get(types, i);
        if(param_type->kind == KIND_VAR){
            string_deinit(&str);
            return 0;
        }
        string_add(&str, param_type->name);
        string_add_chars(&str, " ");
    }
    if(string_size(&str) == 0){
        string_add(&str, get_type_symbol(TYPE_UNIT));
        string_add_chars(&str, " ");
    }
    string_add(&str, get_type_symbol(TYPE_FUNCTION));
    string_add_chars(&str, " ");
    string_add(&str, result_type->name);
    symbol fun_type_symbol = to_symbol(string_get(&str));
    string_deinit(&str);
    return fun_type_symbol;
}

symbol to_array_type_name(symbol element_type_name, struct array *dims)
{
    string str;
    string_init(&str);
    string_add(&str, element_type_name);
    char buffer [sizeof(u32)*8+1];
    if(!array_size(dims)){
        string_add_chars(&str, "[]");
    }
    for(size_t i = 0; i < array_size(dims); i++){
        string_add_chars(&str, "[");
        u32 dim_size = *(u32*)array_get(dims, i);
        sprintf(buffer, "%d", dim_size);
        string_add_chars(&str, buffer);
        string_add_chars(&str, "]");
    }

    symbol array_type_symbol = to_symbol(string_get(&str));
    string_deinit(&str);
    return array_type_symbol;
}
/*symbol 2 type expr pairs*/
struct hashtable _symbol_2_type_exprs; 
/*type variables: collect type variables*/
struct hashtable _type_expr_vars; 

void _free_type_pair(void *type)
{
    struct type_expr_pair *pair = type;
    type_expr_free(pair->ref_type);
    type_expr_free(pair->val_type);
}

void _free_type_expr_var(void *elm)
{
    type_expr_free(elm);
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
    hashtable_init_with_value_size(&_symbol_2_type_exprs, sizeof(struct type_expr_pair), _free_type_pair);
    hashtable_init_with_value_size(&_type_expr_vars, 0, _free_type_expr_var);
}

void types_deinit()
{
    hashtable_deinit(&_symbol_2_type_exprs);
    hashtable_deinit(&_type_expr_vars);
}

void struct_type_init(struct type_expr *struct_type)
{
    struct_type->kind = KIND_OPER;
    struct_type->name = 0;
    struct_type->type = TYPE_STRUCT;
    array_init(&struct_type->args, sizeof(struct type_expr *));
}

void struct_type_deinit(struct type_expr *struct_type)
{
    struct_type->kind = KIND_OPER;
    struct_type->name = 0;
    struct_type->type = TYPE_STRUCT;
    array_deinit(&struct_type->args);
}

void struct_type_add_member(struct type_expr *struct_type, struct type_expr *type)
{
    assert(is_aggregate_type(struct_type));
    array_push(&struct_type->args, &type);
}

struct type_expr *_create_type_oper(enum kind kind, symbol type_name, enum type type, struct type_expr *val_type, struct array *args)
{
    struct type_expr *oper;
    MALLOC(oper, sizeof(*oper));
    oper->kind = kind;
    oper->type = type;
    oper->name = type_name;
    oper->val_type = val_type;
    if(kind == KIND_OPER){
        if(args){
            oper->args = *args;
        } else {
            array_init(&oper->args, sizeof(struct type_expr *));
        }
    }else{
        assert(kind == KIND_VAR);
        oper->instance = 0;
    }
    return oper;
}

struct type_expr *create_type_oper_var(enum kind kind, symbol type_name, enum type type, struct type_expr *val_type, struct array *args)
{
    //there is some var inside the type_oper types
    struct type_expr *type_var = _create_type_oper(kind, type_name, type, val_type, args);
    hashtable_set_p(&_type_expr_vars, type_var, type_var);
    return type_var;
}

struct type_expr *create_type_oper(enum kind kind, symbol type_name, enum type type, struct array *args)
{
    struct type_expr_pair *pair = hashtable_get_p(&_symbol_2_type_exprs, type_name);
    if(pair){
        if(args){
            //we own it now
            array_deinit(args);
        }
        return pair->val_type;
    }
    struct type_expr_pair tep;
    symbol ref_type_name = to_ref_symbol(type_name);
    tep.val_type = _create_type_oper(kind, type_name, type, 0, args);
    tep.ref_type = _create_type_oper(kind, ref_type_name, TYPE_REF, tep.val_type, 0);
    hashtable_set_p(&_symbol_2_type_exprs, type_name, &tep);
    return tep.val_type;
}

/*val_type: referenced value type: e.g. it's int for &int type */
struct type_expr *create_ref_type(struct type_expr *val_type)
{
    create_type_oper(KIND_OPER, val_type->name, val_type->type, 0);
    struct type_expr_pair *pair = hashtable_get_p(&_symbol_2_type_exprs, val_type->name);
    assert(pair);
    return pair->ref_type;
}

struct type_expr *_create_type_var(symbol name)
{
    return _create_type_oper(KIND_VAR, name, 0, 0, 0);
}

struct type_expr *create_type_var()
{
    string name = get_id_name();
    symbol type_name = to_symbol(string_get(&name));
    struct type_expr *type_var = _create_type_var(type_name);
    hashtable_set_p(&_type_expr_vars, type_var, type_var);
    return type_var;
}

struct type_expr *copy_type_var(struct type_expr *var)
{
    struct type_expr *copy_var = _create_type_var(var->name);
    copy_var->instance = var->instance;
    copy_var->type = var->type;
    return copy_var;
}

struct type_expr *create_unit_type()
{
    symbol type_name = get_type_symbol(TYPE_UNIT);
    return create_type_oper(KIND_OPER, type_name, TYPE_UNIT, 0);
}

struct type_expr *create_type_oper_struct(symbol type_name, struct array *args)
{
    return create_type_oper(KIND_OPER, type_name, TYPE_STRUCT, args);
}

struct type_expr *create_nullary_type(enum type type, symbol type_symbol)
{
    return create_type_oper(KIND_OPER, type_symbol, type, 0);
}

struct type_expr *create_type_fun(struct array *args)
{
    symbol type_name = get_type_symbol(TYPE_FUNCTION);
    symbol fun_type_name = _to_fun_type_name(args);
    if(fun_type_name){
        return create_type_oper(KIND_OPER, fun_type_name, TYPE_FUNCTION, args);
    } else {
        //we still have type variable, could be generic function
        struct type_expr *type_var = _create_type_oper(KIND_OPER, type_name, TYPE_FUNCTION, 0, args);
        hashtable_set_p(&_type_expr_vars, type_var, type_var);
        return type_var;
    }
}

struct type_expr *create_array_type(struct type_expr *element_type, struct array *dims)
{
    symbol array_type_name = to_array_type_name(element_type->name, dims);
    struct type_expr *type = create_type_oper(KIND_OPER, array_type_name, TYPE_ARRAY, 0);
    type->dims = *dims;
    type->val_type = element_type;
    return type;
}

//wrap as function type with signature: () -> oper
struct type_expr *wrap_as_fun_type(struct type_expr *oper)
{
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_expr *));
    array_push(&fun_sig, &oper);
    return create_type_fun(&fun_sig);
}

void type_expr_free(struct type_expr *type)
{
    if(type->kind == KIND_OPER){
        array_deinit(&type->args);
    }
    FREE(type);
}

struct type_expr *prune(struct type_expr *type)
{
    if (!type)
        return type;
    if (type->kind == KIND_VAR) {
        assert(type != type->instance);
        if (type->instance) {
            type->instance = prune(type->instance);
            return type->instance;
        }
    } else {
        assert(type->kind == KIND_OPER);
        struct type_expr *argt;
        for (unsigned i = 0; i < array_size(&type->args); ++i) {
            argt = *(struct type_expr **)array_get(&type->args, i);
            struct type_expr *element_type = prune(argt);
            array_set(&type->args, i, &element_type);
        }
    }
    return type;
}

bool _occurs_in_type_list(struct type_expr *var, struct array *list)
{
    for (unsigned i = 0; i < array_size(list); i++) {
        struct type_expr *type = *(struct type_expr **)array_get(list, i);
        if (occurs_in_type(var, type))
            return true;
    }
    return false;
}

/* whether the type variable is in any of the type2, true - found*/
bool occurs_in_type(struct type_expr *var, struct type_expr *type2)
{
    type2 = prune(type2);
    if (type2->kind == KIND_VAR) {
        return var == type2;
    }
    return _occurs_in_type_list(var, &type2->args);
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

struct type_expr *unify(struct type_expr *type1, struct type_expr *type2, struct array *nongens)
{
    type1 = prune(type1);
    type2 = prune(type2);
    assert(type1 && type2);
    /*type1 and type2 are the same one*/
    if (type1 == type2)
        return type1;
    if (type2->kind == KIND_VAR){
        if (type1->kind != KIND_VAR){
            return unify(type2, type1, nongens);
        } else {
            /*type1 is non-generic, type2 is generic, swap the arguments, and the first is generic, the second one is not*/
            if (_occurs_in_type_list(type1, nongens) && !_occurs_in_type_list(type2, nongens))
                return unify(type2, type1, nongens);
            else {
                type1->instance = type2;//assign the type2 to type1 variable
            }
        }
    }
    if (type1->kind == KIND_VAR) {
        assert(!occurs_in_type(type1, type2) || type1 == type2);
        type1->instance = type2;
    } else {
        /*type1 is known type: KIND_OPER*/
        if (!_is_valid_args_size(&type1->args, &type2->args)) return 0;
        if ((type1->val_type != type2->val_type || type1->type != type2->type) && (!IS_TYPE_CONVERTIBLE(type1->type) || !IS_TYPE_CONVERTIBLE(type2->type)))
            return 0;
        size_t arg_size1 = array_size(&type1->args);
        size_t arg_size2 = array_size(&type2->args);
        size_t arg_size = MIN(arg_size1, arg_size2);
        for (size_t i = 0; i < arg_size; i++) {
            unify(*(struct type_expr **)array_get(&type1->args, i == arg_size - 1 ? arg_size1 - 1 : i),
                *(struct type_expr **)array_get(&type2->args, i == arg_size - 1 ? arg_size2 - 1 : i), nongens);
        }
    }
    return type1->type >= type2->type ? type1 : type2;
}

bool _is_generic(struct type_expr *var, struct array *nongens)
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
        if (_is_generic(type, nongens)) {
            struct type_expr *temp = hashtable_get_p(type_vars, type);
            if (!temp) {
                temp = create_type_var();
                hashtable_set_p(type_vars, type, temp);
            }
            return temp;
        } else {
            return type;
        }
    }
    if (array_size(&type->args) == 0 || _all_is_oper(&type->args))
        return type;
    struct array refreshed;
    array_init(&refreshed, sizeof(struct type_expr *));
    for (size_t i = 0; i < array_size(&type->args); i++) {
        struct type_expr *arg_type = *(struct type_expr **)array_get(&type->args, i);
        struct type_expr *new_arg_type = _freshrec(arg_type, nongens, type_vars);
        array_push(&refreshed, &new_arg_type);
    }
    if (type->type == TYPE_STRUCT) {
        return create_type_oper_struct(type->name, &refreshed);
    }
    return create_type_oper_var(KIND_OPER, type->name, type->type, 0, &refreshed);
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
        if (type->instance) {
            return to_string(type->instance);
        } else {
            string_copy(&typestr, type->name);
            return typestr;
        }
    } else if (type->kind == KIND_OPER) {
        if (array_size(&type->args) == 0) { /* nullary operator, e.g. builtin types: int, f64*/
            string_add(&typestr, type->name);
            return typestr;
        } else if (type->type == TYPE_STRUCT) {
            string_copy(&typestr, type->name);
            return typestr;
        } else {
            ARRAY_STRING(array_type_strs);
            for (size_t i = 0; i < array_size(&type->args); i++) {
                string type_str = to_string(*(struct type_expr **)array_get(&type->args, i));
                array_push(&array_type_strs, &type_str);
            }
            struct array subarray;
            array_copy_size(&subarray, &array_type_strs, array_size(&array_type_strs) - 1);
            typestr = string_join(&subarray, " * ");
            if (type->type == TYPE_FUNCTION) {
                if (array_size(&subarray) == 0)
                    string_add_chars(&typestr, "()");
                string_add_chars(&typestr, " -> ");
            } else {
                string_add_chars(&typestr, " * ");
            }
            string_add(&typestr, (string *)array_back(&array_type_strs));
            array_deinit(&array_type_strs);
            array_deinit(&subarray);
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
    struct type_expr *argt;
    for (size_t i = 0; i < array_size(&type->args); i++) {
        argt = *(struct type_expr **)array_get(&type->args, i);
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

bool is_promotable_int(struct type_expr *type)
{
    return type->type == TYPE_CHAR || type->type == TYPE_CHAR;
}

u8 type_size(enum type type)
{
    return type == TYPE_F64 ? 8 : 4;
}

bool is_empty_struct(struct type_expr *type)
{
    if(!is_aggregate_type(type)) 
        return false;
    if(array_size(&type->args) == 0){
        return true;
    }
    for(size_t i = 0; i < array_size(&type->args); i++){
        type = *(struct type_expr **)array_get(&type->args, i);
        if(!is_empty_struct(type)){
            return false;
        }
    }
    return true;
}

struct type_expr *is_single_element_struct(struct type_expr *type)
{
    if(array_size(&type->args) == 0){
        return 0;
    }
    struct type_expr *found = 0;
    for(size_t i = 0; i < array_size(&type->args); i++){
        type = *(struct type_expr **)array_get(&type->args, i);
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

enum type get_type_enum_from_symbol(symbol type_name)
{
     struct type_expr_pair *pair = hashtable_get_p(&_symbol_2_type_exprs, type_name);
    if(pair)
        return pair->val_type->type;
    return TYPE_NULL;
}

symbol get_ref_symbol(symbol type_name)
{
     struct type_expr_pair *pair = hashtable_get_p(&_symbol_2_type_exprs, type_name);
    return pair ? pair->ref_type->name : 0;
}
