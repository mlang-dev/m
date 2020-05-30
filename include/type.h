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
#include "clib/util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_TYPE(ENUM_ITEM) \
    ENUM_ITEM(TYPE_UNK)         \
    ENUM_ITEM(TYPE_GENERIC)     \
    ENUM_ITEM(TYPE_UNIT)        \
    ENUM_ITEM(TYPE_BOOL)        \
    ENUM_ITEM(TYPE_CHAR)        \
    ENUM_ITEM(TYPE_INT)         \
    ENUM_ITEM(TYPE_DOUBLE)      \
    ENUM_ITEM(TYPE_STRING)      \
    ENUM_ITEM(TYPE_FUNCTION)    \
    ENUM_ITEM(TYPE_PRODUCT)     \
    ENUM_ITEM(TYPE_TYPES)

enum type { FOREACH_TYPE(GENERATE_ENUM) };

extern const char* const type_strings[TYPE_TYPES];

#define FOREACH_KIND(ENUM_ITEM) \
    ENUM_ITEM(KIND_UNK)         \
    ENUM_ITEM(KIND_VAR)         \
    ENUM_ITEM(KIND_OPER)

enum kind { FOREACH_KIND(GENERATE_ENUM) };

extern const char* kind_strings[];

//type variable or operator
struct type_exp {
    enum kind kind; //type variable or type operator
    enum type type;
};

struct type_var {
    struct type_exp base;
    struct type_exp* instance;
    string name; //name of type exp: like "->" for function, "bool", "int", "double" for type variable
};

struct type_oper {
    struct type_exp base;
    struct array args; //struct array of struct type_exp*
};

struct type_var* create_type_var();
struct type_oper* create_type_oper(enum type type, struct array* args);
struct type_oper* create_nullary_type(enum type type);
struct type_oper* create_type_fun(struct array* args);
void type_exp_free(struct type_exp* type);
bool occurs_in_type(struct type_var* var, struct type_exp* type2);
struct type_exp* retrieve_type(const char* name, struct array* nongens, struct hashtable* env); //env pointing to hashtable of (string, struct type_exp*)
void set_type(struct hashtable* env, const char* name, struct type_exp* type);
bool unify(struct type_exp* type1, struct type_exp* type2, struct array* nongens);
string to_string(struct type_exp* type);
enum type get_type(struct type_exp* type);
struct type_exp* prune(struct type_exp* type);
bool is_generic(struct type_exp* type);
bool is_any_generic(struct array* types);
string monomorphize(const char* fun_name, struct array* types);
struct type_exp* clone(struct type_exp* type);

#ifdef __cplusplus
}
#endif

#endif
