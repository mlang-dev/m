/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENV_H__
#define __MLANG_ENV_H__

#include "clib/array.h"
#include "clib/hashset.h"
#include "clib/symboltable.h"
#include "parser/parser.h"
#include "sema/sema_context.h"
#include <llvm-c/Core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef LLVMValueRef (*binary_op)(LLVMBuilderRef, LLVMValueRef LHS, LLVMValueRef RHS,
    const char *Name);
typedef LLVMValueRef (*unary_op)(LLVMBuilderRef, LLVMValueRef v, const char *Name);
typedef LLVMValueRef (*cmp_op)(LLVMBuilderRef, unsigned short Op,
    LLVMValueRef LHS, LLVMValueRef RHS,
    const char *Name);
typedef LLVMTypeRef (*get_ir_type)(LLVMContextRef context, struct type_exp *type);
typedef LLVMValueRef (*get_const)(LLVMContextRef context, LLVMBuilderRef builder, void *value);
typedef LLVMValueRef (*get_zero)(LLVMContextRef context, LLVMBuilderRef builder);
typedef LLVMValueRef (*get_one)(LLVMContextRef context);

struct ops {
    get_ir_type get_type;
    get_const get_const;
    get_zero get_zero;
    get_one get_one;
    binary_op add;
    binary_op sub;
    binary_op mul;
    binary_op div;
    binary_op rem;
    cmp_op cmp;
    unsigned short cmp_lt;
    unsigned short cmp_gt;
    unsigned short cmp_eq;
    unsigned short cmp_neq;
    unsigned short cmp_le;
    unsigned short cmp_ge;
    binary_op or_op;
    binary_op and_op;
    unary_op not_op;
    unary_op neg_op;
};

struct env {
    LLVMContextRef context;
    LLVMBuilderRef builder;
    struct hashtable named_values; //hashtable of string, void*
    LLVMModuleRef module;

    struct hashtable protos; //hashtable of char*, prototype_node*
    struct hashtable gvs; //hashtable of char* and var_node*

    struct ops ops[TYPE_TYPES];
    hashset builtins; //hashtable of char*
    struct hashtable specialized_nodes; /*hashtable of <string, struct exp_node*>*/
    struct hashtable ext_types; /*hashtable of <string, struct LLVMTypeRef*/
    struct hashtable ext_nodes; /*hashtable of <string type name, struct type_node*/
    struct hashtable ext_vars; /*hashtable of <string variable, string struct type*/

    /// target info
    struct target_info *target_info;
    struct hashtable type_size_infos; /*hashtable of <symbol, struct type_size_info>*/
    struct hashtable fun_infos; /*hashtable of <symbol, struct fun_info>*/
    struct sema_context *sema_context;
};

struct env *env_new(bool is_repl);
void env_free(struct env *env);
struct env *get_env();

#ifdef __cplusplus
}
#endif

#endif
