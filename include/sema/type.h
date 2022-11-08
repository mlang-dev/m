/*
 * type.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m types
 */
#ifndef __MLANG_TYPE_H__
#define __MLANG_TYPE_H__

#include "clib/hashtable.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/symboltable.h"
#include "clib/util.h"
#include "clib/typedef.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_TYPE(ENUM_ITEM) \
    ENUM_ITEM(TYPE_NULL)        \
    ENUM_ITEM(TYPE_GENERIC)     \
    ENUM_ITEM(TYPE_UNIT)        \
    ENUM_ITEM(TYPE_BOOL)        \
    ENUM_ITEM(TYPE_CHAR)        \
    ENUM_ITEM(TYPE_I8)          \
    ENUM_ITEM(TYPE_U8)          \
    ENUM_ITEM(TYPE_I16)         \
    ENUM_ITEM(TYPE_U16)         \
    ENUM_ITEM(TYPE_I32)         \
    ENUM_ITEM(TYPE_U32)         \
    ENUM_ITEM(TYPE_I64)         \
    ENUM_ITEM(TYPE_U64)         \
    ENUM_ITEM(TYPE_INT)         \
    ENUM_ITEM(TYPE_F32)         \
    ENUM_ITEM(TYPE_F64)         \
    ENUM_ITEM(TYPE_STRING)      \
    ENUM_ITEM(TYPE_FUNCTION)    \
    ENUM_ITEM(TYPE_STRUCT)      \
    ENUM_ITEM(TYPE_ARRAY)       \
    ENUM_ITEM(TYPE_UNION)       \
    ENUM_ITEM(TYPE_COMPLEX)     \
    ENUM_ITEM(TYPE_REF)         \
    ENUM_ITEM(TYPE_TYPES)

enum type { FOREACH_TYPE(GENERATE_ENUM) };

#define FOREACH_KIND(ENUM_ITEM) \
    ENUM_ITEM(KIND_NULL)        \
    ENUM_ITEM(KIND_VAR)         \
    ENUM_ITEM(KIND_OPER)

enum kind { FOREACH_KIND(GENERATE_ENUM) };

extern const char *kind_strings[];

//type variable or operator
struct type_expr {
    enum kind kind; //type variable or type operator
    enum type type;
    struct type_expr* val_type;// val_type the reference type is referred to or element type of the array
    symbol name; //name of type exp: like "->" for function, "bool", "int", "f64" for type variable
    union {
        struct type_expr *instance; //used for KIND_VAR
        //used for KIND_OPER struct array of struct type_expr*
        struct array args; 
    }; 
    struct array dims;  //dimensions for array type
};

void types_init();
void types_deinit();
struct type_expr *create_type_var();
struct type_expr *_create_type_oper(enum kind kind, symbol type_name, enum type type, struct type_expr *ref_type, struct array *args);
struct type_expr *create_type_oper_struct(symbol type_name, struct array *args);
struct type_expr *create_nullary_type(enum type type, symbol type_symbol);
struct type_expr *create_type_fun(struct array *args);
struct type_expr *create_unit_type();
struct type_expr *wrap_as_fun_type(struct type_expr *oper);
struct type_expr *create_ref_type(struct type_expr *val_type);
struct type_expr *create_array_type(struct type_expr *element_type, struct array *dims);
void type_exp_free(struct type_expr *type);
bool occurs_in_type(struct type_expr *var, struct type_expr *type2);
struct type_expr *get_symbol_type(symboltable *st, struct array *nongens, symbol name);
void push_symbol_type(symboltable *st, symbol name, void *type);
bool unify(struct type_expr *type1, struct type_expr *type2, struct array *nongens);
string to_string(struct type_expr *type);
enum type get_type(struct type_expr *type);
struct type_expr *prune(struct type_expr *type);
bool is_generic(struct type_expr *type);
bool is_any_generic(struct array *types);
string monomorphize(const char *fun_name, struct array *types);
bool is_promotable_int(struct type_expr *type);
u8 type_size(enum type type);
bool is_empty_struct(struct type_expr *type);
struct type_expr *is_single_element_struct(struct type_expr *type);
symbol get_type_symbol(enum type type_enum);
enum type get_type_enum_from_symbol(symbol type_name);
symbol to_ref_symbol(symbol type_symbol);
symbol get_ref_symbol(symbol type_name);
symbol to_array_type_name(symbol element_type_name, struct array *dims);
//anonymous function
void struct_type_init(struct type_expr *struct_type);
void struct_type_deinit(struct type_expr *struct_type);
void struct_type_add_member(struct type_expr *struct_type, struct type_expr *type);
bool is_array_size_same(struct type_expr *type1, struct type_expr *type2);

#define is_int_type(type) (type >= TYPE_BOOL && type <= TYPE_INT)
#define is_aggregate_type(node_type) (node_type->type==TYPE_STRUCT || node_type->type == TYPE_ARRAY)
#define is_ref_type(node_type) (node_type->type == TYPE_REF)

#ifdef __cplusplus
}
#endif

#endif
