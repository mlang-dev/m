/*
 * codegen.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for LLVM IR codegen
 */
#ifndef __MLANG_CODEGEN_H__
#define __MLANG_CODEGEN_H__

#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

#include "clib/hashset.h"
#include "clib/hashtable.h"
#include "codegen/ir_api.h"
#include "codegen/target_info.h"
#include "sema/sema_context.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LLVMValueRef (*binary_op)(LLVMBuilderRef builder, LLVMValueRef lhs, LLVMValueRef rhs,
    const char *name);
typedef LLVMValueRef (*unary_op)(LLVMBuilderRef builder, LLVMValueRef v, const char *name);
typedef LLVMValueRef (*cmp_op)(LLVMBuilderRef builder, unsigned short op,
    LLVMValueRef lhs, LLVMValueRef rhs, const char *name);
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

struct code_generator {
    /* 
     *  value type env: symboltable of <symbol, LLVMValueRef>
     *  binding variable name to LLVMValueRef
     */
    struct hashtable varname_2_irvalues;

    LLVMContextRef context;
    LLVMBuilderRef builder;
    LLVMModuleRef module;

    struct hashtable protos; //hashtable of char*, prototype_node*
    struct hashtable gvs; //hashtable of char* and var_node*

    struct ops ops[TYPE_TYPES];
    hashset builtins; //hashtable of char*
    struct hashtable typename_2_irtypes; /*hashtable of <string, struct LLVMTypeRef*/
    struct hashtable typename_2_ast; /*hashtable of <string type name, struct type_node*/
    struct hashtable varname_2_typename; /*hashtable of <string variable, string type*/

    /// target info
    struct target_info *target_info;
    struct hashtable type_size_infos; /*hashtable of <symbol, struct type_size_info>*/
    struct hashtable fun_infos; /*hashtable of <symbol, struct fun_info>*/
    struct sema_context *sema_context;
};

#define is_int_type(type) (type == TYPE_INT || type == TYPE_BOOL || type == TYPE_CHAR)

struct code_generator *cg_new(struct sema_context *sema_context);
void cg_free(struct code_generator *cg);

void create_ir_module(struct code_generator *cg, const char *module_name);
LLVMValueRef emit_ir_code(struct code_generator *cg, struct exp_node *node);
LLVMTargetMachineRef create_target_machine(LLVMModuleRef module);
LLVMContextRef get_llvm_context();
LLVMTypeRef get_llvm_type(struct type_exp *type);
LLVMTypeRef get_llvm_type_for_abi(struct type_exp *type);
LLVMTargetDataRef get_llvm_data_layout();
enum OS get_os();
LLVMModuleRef get_llvm_module();
struct hashtable *get_type_size_infos();
struct hashtable *get_fun_infos();

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CODEGEN_H__
