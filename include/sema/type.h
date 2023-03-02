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
    ENUM_ITEM(TYPE_TUPLE)       \
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

enum Mut {
    Immutable = 0,
    Mutable
};

enum ADTKind {
    Sum = 0,    //union
    Product //struct
};

//type variable or operator
struct type_item {
    enum kind kind; //type variable or type operator
    enum type type;
    enum Mut mut; //mutability of the data of the type
    struct type_item* val_type;// val_type the reference type is referred to or element type of the array
    symbol name; //name of type exp: like "->" for function, "bool", "int", "f64" for type variable
    symbol canon_name; //base name of the type, invariant among different variances of types: e.g. alwasy int for int, &int, &mut int, mut &mut int etc.
    union {
        struct type_item *instance; //used for KIND_VAR
        //used for KIND_OPER struct array of struct type_item*
        struct array args; 
    }; 
    struct array dims;  //dimensions for array type
};


struct type_item_pair {
    struct type_item *val_types[2];     //immutability
    struct type_item *ref_types[2][2];  //refer and val immutability
};

struct type_expr_item {
    symbol ident; 
    struct type_item *type; 
};

void types_init();
void types_deinit();
struct type_item *create_type_var(enum Mut mut);
struct type_item *create_type_oper_var(enum kind kind, symbol type_name, enum type type, struct type_item *val_type, struct array *args);
struct type_item *create_type_oper_struct(symbol type_name, enum Mut mut, struct array *args);
struct type_item *create_type_oper_tuple(enum Mut mut, struct array *args);
struct type_item *create_type_oper_union(symbol type_name, enum Mut mut, struct array *args);
struct type_item *create_nullary_type(enum type type);
struct type_item *create_type_fun(struct array *args);
struct type_item *create_unit_type();
struct type_item *wrap_as_fun_type(struct type_item *oper);
struct type_item *create_ref_type(struct type_item *val_type, enum Mut mut);
struct type_item *create_array_type(struct type_item *element_type, struct array *dims);
void type_item_free(struct type_item *type);

bool occurs_in_type(struct type_item *var, struct type_item *type2);
struct type_item *get_symbol_type(symboltable *st, struct array *nongens, symbol name);
void push_symbol_type(symboltable *st, symbol name, void *type);
struct type_item *unify(struct type_item *type1, struct type_item *type2, struct array *nongens);
string to_string(struct type_item *type);
enum type get_type(struct type_item *type);
struct type_item *prune(struct type_item *type);
bool is_generic(struct type_item *type);
bool is_any_generic(struct array *types);
string monomorphize(const char *fun_name, struct array *types);
bool is_promotable_int(struct type_item *type);
u8 type_size(enum type type);
bool is_empty_struct(struct type_item *type);
struct type_item *is_single_element_struct(struct type_item *type);
symbol get_type_symbol(enum type type_enum);
enum type get_type_enum_from_symbol(symbol type_name);
symbol to_ref_symbol(symbol type_symbol);
symbol get_ref_symbol(symbol type_name);
symbol to_array_type_name(symbol element_type_name, struct array *dims);
//anonymous function
void struct_type_init(struct type_item *struct_type);
void struct_type_deinit(struct type_item *struct_type);
void struct_type_add_member(struct type_item *struct_type, struct type_item *type);

struct type_item *fresh(struct type_item *type, struct array *nongens);
struct type_item_pair *get_type_item_pair(symbol type_name);
struct type_item *tep_find_type_item(struct type_item_pair *pair, enum Mut mut, bool is_ref, enum Mut referent_mut);
struct type_item *find_type_item(struct type_item *oper, enum Mut mut);

#define is_int_type(type) (type >= TYPE_BOOL && type <= TYPE_INT)
#define is_aggregate_type(node_type) (node_type->type==TYPE_STRUCT || node_type->type==TYPE_UNION || node_type->type == TYPE_ARRAY || node_type->type == TYPE_TUPLE)
#define is_ref_type(node_type) (node_type->type == TYPE_REF)
#define is_adt(te)    (te->type==TYPE_STRUCT || te->type == TYPE_UNION || te->type == TYPE_TUPLE)
#define is_adt_or_ref(te) (is_adt(te) || (te->type == TYPE_REF && is_adt(te->val_type)))
#define is_record_like_type(ti) (ti->type == TYPE_STRUCT || ti->type == TYPE_TUPLE)

#ifdef __cplusplus
}
#endif

#endif
