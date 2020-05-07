/*
 * type.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m types
 */
#ifndef __MLANG_TYPE_H__
#define __MLANG_TYPE_H__

#include "clib/util.h"
#include "clib/string.h"
#include "clib/hashtable.h"

#ifdef __cplusplus
extern "C"{
#endif

#define FOREACH_TYPE(ENUM_ITEM) \
    ENUM_ITEM(TYPE_UNK)         \
    ENUM_ITEM(TYPE_GENERIC)     \
    ENUM_ITEM(TYPE_UNIT)        \
    ENUM_ITEM(TYPE_BOOL)        \
    ENUM_ITEM(TYPE_CHAR)        \
    ENUM_ITEM(TYPE_INT)         \
    ENUM_ITEM(TYPE_DOUBLE)      \
    ENUM_ITEM(TYPE_FUNCTION)    \
    ENUM_ITEM(TYPE_PRODUCT)

enum _Type { FOREACH_TYPE(GENERATE_ENUM) };
typedef enum _Type Type;

static const char* const TypeString[] = {
    "unkown",
    "generic",
    "()",
    "bool",
    "char",
    "int",
    "double",
    "->"
    "*"
};

#define FOREACH_KIND(ENUM_ITEM) \
    ENUM_ITEM(KIND_UNK)         \
    ENUM_ITEM(KIND_VAR)         \
    ENUM_ITEM(KIND_OPER)

enum _Kind { FOREACH_KIND(GENERATE_ENUM) };
typedef enum _Kind Kind; 

static const char* KindString[] = {
    FOREACH_KIND(GENERATE_ENUM_STRING)
};

//type variable or operator
typedef struct _type_exp {
    Kind kind; //type variable or type operator
    Type type;
}type_exp;

typedef struct _type_var {
    type_exp base;
    type_exp* instance;
    string name; //name of type exp: like "->" for function, "bool", "int", "double" for type variable
}type_var;

typedef struct _type_oper {
    type_exp base;
    array args; //array of type_exp*
}type_oper;

type_var* create_type_var();
type_oper* create_type_oper(Type type, array *args);
type_oper* create_nullary_type(Type type);
type_oper* create_type_fun(array *args);
void type_exp_free(type_exp* type);
bool occurs_in_type(type_var* var, type_exp* type2);
type_exp* retrieve_type(const char *name, array *nogen, struct hashtable *env); //env pointing to hashtable of (string, type_exp*)
void set_type(struct hashtable *env, const char *name, type_exp* type);
bool unify(type_exp* type1, type_exp* type2, array *nogens);
string to_string(type_exp* type);
type_exp* prune(type_exp* type);

#ifdef __cplusplus
}
#endif

#endif
