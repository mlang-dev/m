/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language type inference algorithms. 
 * references: http://lucacardelli.name/Papers/BasicTypechecking.pdf
 */
#include "sema/type.h"
#include "sema/type_size_info.h"
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
    "None",
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
    "tuple",
    "array",
    "union",
    "complex",
    "ref",
};

#define IS_TYPE_CONVERTIBLE(type)  (type > TYPE_UNIT && type < TYPE_STRING)

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

symbol _to_fun_type_name(struct type_context *tc, struct array *types)
{
    if(!array_size(types)){
        return 0;
    }
    struct type_item *result_type = array_back_ptr(types);
    if (!result_type) return 0;
    if (result_type->kind == KIND_VAR) return 0;
    string str;
    string_init(&str);
    struct type_item *param_type;
    for(size_t i = 0; i < array_size(types) - 1; i++){
        param_type = array_get_ptr(types, i);
        if(param_type->kind == KIND_VAR){
            string_deinit(&str);
            return 0;
        }
        string_add(&str, param_type->name);
        string_add_chars(&str, " ");
    }
    if(string_size(&str) == 0){
        string_add(&str, get_type_symbol(tc, TYPE_UNIT));
        string_add_chars(&str, " ");
    }
    string_add(&str, get_type_symbol(tc, TYPE_FUNCTION));
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

void _free_type_pair(void *type)
{
    struct type_item_pair *pair = type;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++)
            type_item_free(pair->ref_types[i][j]);
        type_item_free(pair->val_types[i]);
    }
}

symbol _merge_string(const char *str1, symbol str2)
{
    return str2;
    string str;
    string_init_chars(&str, str1);
    string_add(&str, str2);
    symbol sym = string_2_symbol(&str);
    string_deinit(&str);
    return sym;
}

void _free_type_item_var(void *elm)
{
    type_item_free(elm);
}

void type_item_free(struct type_item *type)
{
    if(!type) return;
    if(type->kind == KIND_OPER){
        array_deinit(&type->args);
    }
    if(type->type == TYPE_ARRAY){
        array_deinit(&type->dims);
    }
    FREE(type);
}

struct type_context *type_context_new()
{
    struct type_context *tc;
    MALLOC(tc, sizeof(*tc));
    for(int i = 0; i < TYPE_TYPES; i++){
        symbol type_symbol = to_symbol(_type_strings[i]);
        symbol mut_type_symbol = _merge_string("mut ", type_symbol);
        symbol ref_type_symbol = _merge_string("&", type_symbol);
        symbol ref_mut_type_symbol = _merge_string("&", mut_type_symbol);
        tc->type_symbols[i].type_symbols[0] = type_symbol;
        tc->type_symbols[i].type_symbols[1] = mut_type_symbol;
        tc->type_symbols[i].ref_type_symbols[0][0] = ref_type_symbol;
        tc->type_symbols[i].ref_type_symbols[0][1] = ref_mut_type_symbol;
        tc->type_symbols[i].ref_type_symbols[1][0] = _merge_string("mut ", ref_type_symbol);
        tc->type_symbols[i].ref_type_symbols[1][1] = _merge_string("mut ", ref_mut_type_symbol);
    }
    hashtable_init_with_value_size(&tc->symbol_2_type_items, sizeof(struct type_item_pair), _free_type_pair);
    hashtable_init_with_value_size(&tc->type_item_vars, 0, _free_type_item_var);
    hashtable_init_with_value_size(&tc->freshed_type_items, 0, _free_type_item_var);
    hashtable_init_with_value_size(&tc->symbol_2_int_types, sizeof(int), 0);
    for (int i = 0; i < TYPE_TYPES; i++) {
        hashtable_set_int(&tc->symbol_2_int_types, get_type_symbol(tc, i), i);
    }
    hashtable_init_with_value_size(&tc->ts_infos, sizeof(struct type_size_info), tsi_free);
    return tc;
}

void type_context_free(struct type_context *tc)
{
    hashtable_deinit(&tc->symbol_2_type_items);
    hashtable_deinit(&tc->type_item_vars);
    hashtable_deinit(&tc->freshed_type_items);
    hashtable_deinit(&tc->symbol_2_int_types);
    hashtable_deinit(&tc->ts_infos);
    FREE(tc);
}

void struct_type_init(struct type_item *struct_type)
{
    struct_type->kind = KIND_OPER;
    struct_type->name = 0;
    struct_type->type = TYPE_STRUCT;
    array_init(&struct_type->args, sizeof(struct type_item *));
}

void struct_type_deinit(struct type_item *struct_type)
{
    struct_type->kind = KIND_OPER;
    struct_type->name = 0;
    struct_type->type = TYPE_STRUCT;
    array_deinit(&struct_type->args);
}

void struct_type_add_member(struct type_item *struct_type, struct type_item *type)
{
    assert(is_aggregate_type(struct_type));
    array_push(&struct_type->args, &type);
}

struct type_item *_create_type_oper(enum kind kind, symbol canon_name, symbol type_name, enum type type, enum Mut mut, struct type_item *val_type, struct array *args)
{
    struct type_item *oper;
    MALLOC(oper, sizeof(*oper));
    oper->kind = kind;
    oper->type = type;
    oper->name = type_name;
    oper->canon_name = canon_name;
    oper->val_type = val_type;
    oper->mut = mut;
    oper->is_variadic = false;
    oper->backend_type = 0;
    if(kind == KIND_OPER){
        if(args){
            oper->args = *args;
        } else {
            array_init(&oper->args, sizeof(struct type_item *));
        }
    }else{
        assert(kind == KIND_VAR);
        oper->instance = 0;
    }
    if(type == TYPE_ARRAY){
        array_init(&oper->dims, sizeof(u32));
    }
    return oper;
}

struct type_item *create_type_oper_var(struct type_context *tc, enum kind kind, symbol type_name, enum type type, struct type_item *val_type, struct array *args)
{
    //there is some var inside the type_oper types
    struct type_item *type_var = _create_type_oper(kind, type_name, type_name, type, Immutable, val_type, args);
    hashtable_set_p(&tc->type_item_vars, type_var, type_var);
    return type_var;
}

struct type_item *create_type_oper(struct type_context *tc, enum kind kind, symbol type_name, enum type type, enum Mut mut, struct array *args)
{
    struct type_item_pair *pair = hashtable_get_p(&tc->symbol_2_type_items, type_name);
    if(pair){
        if(args){
            //we own it now
            array_deinit(args);
        }
        return pair->val_types[mut];
    }
    struct type_item_pair tep = {0};
    symbol ref_type_name = to_ref_symbol(type_name);
    tep.val_types[0] = _create_type_oper(kind, type_name, type_name, type, Immutable, 0, args);
    struct array new_args;
    if(args){
        array_copy(&new_args, args);
        args = &new_args;
    }
    tep.val_types[1] = _create_type_oper(kind, type_name, type_name, type, Mutable, 0, args);
    tep.ref_types[0][0] = _create_type_oper(kind, type_name, ref_type_name, TYPE_REF, Immutable, tep.val_types[0], 0);
    tep.ref_types[0][1] = _create_type_oper(kind, type_name, ref_type_name, TYPE_REF, Immutable, tep.val_types[1], 0);
    tep.ref_types[1][0] = _create_type_oper(kind, type_name, ref_type_name, TYPE_REF, Mutable, tep.val_types[0], 0);
    tep.ref_types[1][1] = _create_type_oper(kind, type_name, ref_type_name, TYPE_REF, Mutable, tep.val_types[1], 0);
    hashtable_set_p(&tc->symbol_2_type_items, type_name, &tep);
    return tep.val_types[mut];
}

/*val_type: referenced value type: e.g. it's int for &int type */
struct type_item *create_ref_type(struct type_context *tc, struct type_item *val_type, enum Mut mut)
{
    create_type_oper(tc, KIND_OPER, val_type->name, val_type->type, Immutable, 0);
    struct type_item_pair *pair = hashtable_get_p(&tc->symbol_2_type_items, val_type->name);
    assert(pair);
    return pair->ref_types[mut][val_type->mut];
}

struct type_item *_create_type_var(symbol name, enum Mut mut)
{
    return _create_type_oper(KIND_VAR, name, name, 0, mut, 0, 0);
}

struct type_item *create_type_var(struct type_context *tc, enum Mut mut)
{
    string name = get_id_name();
    symbol type_name = to_symbol(string_get(&name));
    struct type_item *type_var = _create_type_var(type_name, mut);
    hashtable_set_p(&tc->type_item_vars, type_var, type_var);
    return type_var;
}

struct type_item *copy_type_var(struct type_item *var)
{
    struct type_item *copy_var = _create_type_var(var->name, var->mut);
    copy_var->instance = var->instance;
    copy_var->type = var->type;
    return copy_var;
}

struct type_item *create_unit_type(struct type_context *tc)
{
    symbol type_name = get_type_symbol(tc, TYPE_UNIT);
    return create_type_oper(tc, KIND_OPER, type_name, TYPE_UNIT, Immutable, 0);
}

struct type_item *create_type_oper_struct(struct type_context *tc, symbol type_name, enum Mut mut, struct array *args)
{
    return create_type_oper(tc, KIND_OPER, type_name, TYPE_STRUCT, mut, args);
}

struct type_item *create_type_oper_tuple(struct type_context *tc, enum Mut mut, struct array *args)
{
    struct type_item* ti = _create_type_oper(KIND_OPER, 0, 0, TYPE_TUPLE, mut, 0, args);
    string type_name = to_string(tc, ti);
    ti->name = string_2_symbol(&type_name);
    ti->canon_name = ti->name;
    string_deinit(&type_name);
    hashtable_set_p(&tc->freshed_type_items, ti, ti);
    return ti;
}

struct type_item *create_type_oper_union(struct type_context *tc, symbol type_name, enum Mut mut, struct array *args)
{
    return create_type_oper(tc, KIND_OPER, type_name, TYPE_VARIANT, mut, args);
}

struct type_item *create_nullary_type(struct type_context *tc, enum type type)
{
    symbol type_symbol = get_type_symbol(tc, type);
    return create_type_oper(tc, KIND_OPER, type_symbol, type, Immutable, 0);
}

struct type_item *create_type_fun(struct type_context *tc, bool is_variadic, struct array *args)
{
    symbol type_name = get_type_symbol(tc, TYPE_FUNCTION);
    symbol fun_type_name = _to_fun_type_name(tc, args);
    struct type_item *type = 0;
    if(fun_type_name){
        type = create_type_oper(tc, KIND_OPER, fun_type_name, TYPE_FUNCTION, Immutable, args);
    } else {
        //we still have type variable, could be generic function
        type = _create_type_oper(KIND_OPER, type_name, type_name, TYPE_FUNCTION, Immutable, 0, args);
        hashtable_set_p(&tc->type_item_vars, type, type);
    }
    type->is_variadic = is_variadic;
    return type;
}

struct type_item *create_array_type(struct type_context *tc, struct type_item *element_type, struct array *dims)
{
    symbol array_type_name = to_array_type_name(element_type->name, dims);
    struct type_item *type = create_type_oper(tc, KIND_OPER, array_type_name, TYPE_ARRAY, Mutable, 0);
    array_deinit(&type->dims);
    type->dims = *dims;
    type->val_type = element_type;
    return type;
}

//wrap as function type with signature: () -> oper
struct type_item *wrap_as_fun_type(struct type_context *tc, struct type_item *oper)
{
    struct array fun_sig;
    array_init(&fun_sig, sizeof(struct type_item *));
    array_push(&fun_sig, &oper);
    return create_type_fun(tc, oper->is_variadic, &fun_sig);
}

struct type_item *tep_find_type_item(struct type_item_pair *pair, enum Mut mut, bool is_ref, enum Mut referent_mut)
{
    if(pair){
        return is_ref ? pair->ref_types[mut][referent_mut] : pair->val_types[mut];
    }
    return 0;
}

struct type_item *find_type_item(struct type_context *tc, struct type_item *oper, enum Mut mut)
{
    if(oper->mut == mut || oper->type == TYPE_ARRAY){
        return oper;
    }
    struct type_item_pair *pair = hashtable_get_p(&tc->symbol_2_type_items, oper->canon_name);
    struct type_item *type = tep_find_type_item(pair, mut, oper->type == TYPE_REF, oper->val_type ? oper->val_type->mut : Immutable);
    return type ? type : oper;
}

struct type_item *_prune(struct type_context *tc, struct type_item *type, enum Mut mut)
{
    if (!type) return type;
    if (type->kind == KIND_VAR) {
        assert(type != type->instance);
        if (type->instance) {
            type->instance = _prune(tc, type->instance, type->mut);
            return type->instance;
        }
    } else {
        assert(type->kind == KIND_OPER);
        struct type_item *argt;
        for (unsigned i = 0; i < array_size(&type->args); ++i) {
            argt = array_get_ptr(&type->args, i);
            if(!argt) continue;
            struct type_item *element_type = _prune(tc, argt, argt->mut);
            array_set(&type->args, i, &element_type);
        }
        /*after pruned all vars*/
        type = find_type_item(tc, type, mut);
        if(type->type == TYPE_FUNCTION){
            type->name = _to_fun_type_name(tc, &type->args);
        }
    }
    return type;
}

struct type_item *prune(struct type_context *tc, struct type_item *type)
{
    if(!type) return 0;
    return _prune(tc, type, type->mut);
}

bool _occurs_in_type_list(struct type_context *tc, struct type_item *var, struct array *list)
{
    for (unsigned i = 0; i < array_size(list); i++) {
        struct type_item *type = array_get_ptr(list, i);
        if (occurs_in_type(tc, var, type))
            return true;
    }
    return false;
}

/* whether the type variable is in any of the type2, true - found*/
bool occurs_in_type(struct type_context *tc, struct type_item *var, struct type_item *type2)
{
    type2 = prune(tc, type2);
    if(!type2) return false;
    if (type2->kind == KIND_VAR) {
        return var == type2;
    }
    return _occurs_in_type_list(tc, var, &type2->args);
}

bool _is_variadic(struct type_context *tc, struct array *args)
{
    size_t size = array_size(args);
    if (size > 1) {
        struct type_item *exp = array_get_ptr(args, size - 2);
        return get_type(tc, exp) == TYPE_GENERIC;
    }
    return false;
}

bool _is_valid_args_size(struct type_context *tc, struct array *args1, struct array *args2)
{
    if (_is_variadic(tc, args1)) {
        return array_size(args2) >= array_size(args1) - 1;
    } else if (_is_variadic(tc, args2)) {
        return array_size(args1) >= array_size(args2) - 1;
    }
    return array_size(args1) == array_size(args2);
}

struct type_item *unify(struct type_context *tc, struct type_item *type1, struct type_item *type2, struct array *nongens)
{
    type1 = prune(tc, type1);
    type2 = prune(tc, type2);
    if(!type1 || !type2) return 0;
    /*type1 and type2 are the same one*/
    if (type1 == type2)
        return type1;
    if (type2->kind == KIND_VAR){
        if (type1->kind != KIND_VAR){
            return unify(tc, type2, type1, nongens);
        } else {
            /*type1 is non-generic, type2 is generic, swap the arguments, and the first is generic, the second one is not*/
            if (_occurs_in_type_list(tc, type1, nongens) && !_occurs_in_type_list(tc, type2, nongens))
                return unify(tc, type2, type1, nongens);
            else {
                type1->instance = type2;//assign the type2 to type1 variable
            }
        }
    }
    if (type1->kind == KIND_VAR) {
        assert(!occurs_in_type(tc, type1, type2) || type1 == type2);
        type1->instance = type2;
    } else {
        /*type1 is known type: KIND_OPER*/
        if (!_is_valid_args_size(tc, &type1->args, &type2->args)) return 0;
        if ((type1->val_type != type2->val_type || type1->type != type2->type) && (!IS_TYPE_CONVERTIBLE(type1->type) || !IS_TYPE_CONVERTIBLE(type2->type)))
            return 0;
        size_t arg_size1 = array_size(&type1->args);
        size_t arg_size2 = array_size(&type2->args);
        size_t arg_size = MIN(arg_size1, arg_size2);
        for (size_t i = 0; i < arg_size; i++) {
            unify(tc, array_get_ptr(&type1->args, i == arg_size - 1 ? arg_size1 - 1 : i),
                array_get_ptr(&type2->args, i == arg_size - 1 ? arg_size2 - 1 : i), nongens);
        }
    }
    type1 = prune(tc, type1);
    return type1->type >= type2->type ? type1 : type2;
}

bool _is_generic(struct type_context *tc, struct type_item *var, struct array *nongens)
{
    return !_occurs_in_type_list(tc, var, nongens);
}

bool _all_is_oper(struct array *arr)
{
    for (size_t i = 0; i < array_size(arr); i++) {
        struct type_item *type = array_get(arr, i);
        if (type->kind != KIND_OPER)
            return false;
    }
    return true;
}

/* 
    for any generic type in the type, create a type variable thunk for it
    we must share non-generic types.
    A type variable occurring in the type of an expression e is generic WRT e iff it does not occur in 
    the type of the binder of any fun expression enclosing e.
    A (type) variable is generic if it does not appear in the type of the variables of any enclosing fun binder. 
*/
struct type_item *_freshrec(struct type_context *tc, struct type_item *type, struct array *nongens, struct hashtable *type_vars)
{
    type = prune(tc, type);
    if (type->kind == KIND_VAR) {
        if (_is_generic(tc, type, nongens)) {
            struct type_item *temp = hashtable_get_p(type_vars, type);
            if (!temp) {
                temp = create_type_var(tc, type->mut);
                hashtable_set_p(type_vars, type, temp);
            }
            return temp;
        } else {
            //non-generic returning shared one
            return type;
        }
    }
    if (array_size(&type->args) == 0 || _all_is_oper(&type->args))
        return type;
    struct array refreshed;
    array_init(&refreshed, sizeof(struct type_item *));
    //refresh each components
    for (size_t i = 0; i < array_size(&type->args); i++) {
        struct type_item *arg_type = array_get_ptr(&type->args, i);
        struct type_item *new_arg_type = _freshrec(tc, arg_type, nongens, type_vars);
        array_push(&refreshed, &new_arg_type);
    }
    struct type_item *new_type = 0;
    if (type->type == TYPE_STRUCT) {
        new_type = create_type_oper_struct(tc, type->name, type->mut, &refreshed);
    } else if (type->type == TYPE_TUPLE){
        new_type = create_type_oper_tuple(tc, type->mut, &refreshed);
    } else {
        new_type = create_type_oper_var(tc, KIND_OPER, type->name, type->type, 0, &refreshed);
    }
    return new_type;
}

struct type_item *fresh(struct type_context *tc, struct type_item *type, struct array *nongens)
{
    struct hashtable type_vars;
    hashtable_init(&type_vars);
    struct type_item *result = _freshrec(tc, type, nongens, &type_vars);
    hashtable_deinit(&type_vars);
    return result;
}


void push_symbol_type(symboltable *st, symbol name, void *type)
{
    symboltable_push(st, name, type);
}

enum type get_type(struct type_context *tc, struct type_item *type)
{
    type = prune(tc, type);
    assert(type && type->type >= 0 && type->type < TYPE_TYPES);
    return type->type;
}

enum type get_return_type(struct type_context *tc, struct type_item *type)
{
    type = prune(tc, type);
    assert(type && type->type >= 0 && type->type < TYPE_TYPES);
    if (type->type == TYPE_FUNCTION){
        type = array_back_ptr(&type->args);
    } 
    return type->type;
}

string to_string(struct type_context *tc, struct type_item *type)
{
    string typestr;
    string_init_chars(&typestr, "");
    if (!type) {
        string_add_chars(&typestr, "type mismatch");
        return typestr;
    }
    type = prune(tc, type);
    if (type->kind == KIND_VAR) {
        if (type->instance) {
            return to_string(tc, type->instance);
        } else {
            string_copy(&typestr, type->name);
            return typestr;
        }
    } else if (type->kind == KIND_OPER) {
        if (array_size(&type->args) == 0) { /* nullary operator, e.g. builtin types: int, f64*/
            string_add(&typestr, type->name);
            return typestr;
        } else if (is_adt(type) && type->name) {
            string_copy(&typestr, type->name);
            return typestr;
        } else {
            ARRAY_STRING(array_type_strs);
            for (size_t i = 0; i < array_size(&type->args); i++) {
                string type_str = to_string(tc, array_get_ptr(&type->args, i));
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

bool is_generic(struct type_context *tc, struct type_item *type)
{
    type = prune(tc, type);
    if (type->kind == KIND_VAR)
        return true;
    struct type_item *argt;
    for (size_t i = 0; i < array_size(&type->args); i++) {
        argt = array_get_ptr(&type->args, i);
        type = prune(tc, argt);
        if (!type)
            return false;
        if (type->kind == KIND_VAR)
            return true;
    }
    return false;
}

bool is_any_generic(struct type_context *tc, struct array *types)
{
    for (size_t i = 0; i < array_size(types); i++) {
        struct type_item *exp = array_get_ptr(types, i);
        if (is_generic(tc, exp))
            return true;
    }
    return false;
}

string monomorphize(struct type_context *tc, const char *fun_name, struct array *types)
{
    string sp;
    string_init_chars(&sp, "__");
    string_add_chars(&sp, fun_name);
    for (size_t i = 0; i < array_size(types); i++) {
        struct type_item *type = array_get_ptr(types, i);
        string type_str = to_string(tc, type);
        string_add_chars(&sp, "_");
        string_add(&sp, &type_str);
    }
    return sp;
}

bool is_promotable_int(struct type_item *type)
{
    return type->type == TYPE_CHAR || type->type == TYPE_CHAR;
}

u8 type_size(enum type type)
{
    return type == TYPE_F64 ? 8 : 4;
}

bool is_empty_struct(struct type_item *type)
{
    if(!is_aggregate_type(type)) 
        return false;
    if(array_size(&type->args) == 0){
        return true;
    }
    for(size_t i = 0; i < array_size(&type->args); i++){
        type = array_get_ptr(&type->args, i);
        if(!is_empty_struct(type)){
            return false;
        }
    }
    return true;
}

struct type_item *is_single_element_struct(struct type_item *type)
{
    if(array_size(&type->args) == 0){
        return 0;
    }
    struct type_item *found = 0;
    for(size_t i = 0; i < array_size(&type->args); i++){
        type = array_get_ptr(&type->args, i);
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

symbol get_type_symbol(struct type_context *tc, enum type type_enum)
{
    return tc->type_symbols[type_enum].type_symbols[0];
}

enum type get_type_enum_from_symbol(struct type_context *tc, symbol type_name)
{
     struct type_item_pair *pair = hashtable_get_p(&tc->symbol_2_type_items, type_name);
    if(pair)
        return pair->val_types[0]->type;
    return TYPE_NULL;
}

symbol get_ref_symbol(struct type_context *tc, symbol type_name)
{
     struct type_item_pair *pair = hashtable_get_p(&tc->symbol_2_type_items, type_name);
    return pair ? pair->ref_types[0][0]->name : 0;
}

struct type_item_pair *get_type_item_pair(struct type_context *tc, symbol type_name)
{
    return hashtable_get_p(&tc->symbol_2_type_items, type_name);
}

u64 get_array_size(struct type_item *type)
{
    u32 size = 1;
    if(type->type != TYPE_ARRAY)
        return size;
    for(u32 i=0; i<array_size(&type->dims); i++){
        size *= *(u32*)array_get(&type->dims, i);
    }
    return size * get_array_size(type->val_type);
}
