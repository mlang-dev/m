/*
 * cg_llvm.h
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
#include "codegen/llvm/llvm_api.h"
#include "codegen/target_info.h"
#include "codegen/fun_info.h"
#include "codegen/backend.h"
#include "sema/sema_context.h"
#include "sema/type.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LLVMValueRef (*binary_op)(LLVMBuilderRef builder, LLVMValueRef lhs, LLVMValueRef rhs,
    const char *name);
typedef LLVMValueRef (*unary_op)(LLVMBuilderRef builder, LLVMValueRef v, const char *name);
typedef LLVMValueRef (*cmp_op)(LLVMBuilderRef builder, int op,
    LLVMValueRef lhs, LLVMValueRef rhs, const char *name);
typedef LLVMTypeRef (*get_ir_type_func)(LLVMContextRef context, struct type_exp *type);
typedef LLVMValueRef (*get_const_func)(LLVMContextRef context, LLVMBuilderRef builder, void *value);
typedef LLVMValueRef (*get_zero_func)(LLVMContextRef context, LLVMBuilderRef builder);
typedef LLVMValueRef (*get_one_func)(LLVMContextRef context);

struct ops {
    get_ir_type_func get_type;
    get_const_func get_const;
    get_zero_func get_zero;
    get_one_func get_one;
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

struct cg_llvm {
    LLVMContextRef context;
    LLVMBuilderRef builder;
    LLVMModuleRef module;
    struct sema_context *sema_context;
    struct ops ops[TYPE_TYPES];

    /* 
     *  hashset of <symbol, symbol>
     *  built function symbol names
     */
    hashset builtins;

    /* 
     *  symboltable of <symbol, ast_node*>
     *  binding func_type name to func type ast_node*
     */
    struct hashtable protos;

    /* 
     *  symboltable of <symbol, var ast_node*>
     *  binding global variable name to var ast_node*
     */
    struct hashtable gvs;

    /* 
     *  symboltable of <symbol, LLVMValueRef>
     *  binding variable name to LLVMValueRef
     */
    struct hashtable varname_2_irvalues;

    /* 
     *  symboltable of <symbol, symbol>
     *  binding variable name to type name
     */
    struct hashtable varname_2_typename;

    /* 
     *  symboltable of <symbol, LLVMTypeRef>
     *  binding type name to IR Type
     */
    struct hashtable typename_2_irtypes;

    /* 
     *  symboltable of <symbol, ast_node*>, type ast_node
     *  binding type name to AST definition node
     */
    struct hashtable typename_2_ast;

    /// target info
    struct target_info *target_info;
};


struct cg_llvm *cg_new(struct sema_context *sema_context);
void cg_free(struct cg_llvm *cg);

void emit_code(struct cg_llvm *cg, struct ast_node *node);
void emit_sp_code(struct cg_llvm *cg);


void create_ir_module(struct cg_llvm *cg, const char *module_name);
LLVMValueRef emit_ir_code(struct cg_llvm *cg, struct ast_node *node);
LLVMTargetMachineRef create_target_machine(LLVMModuleRef module);
LLVMContextRef get_llvm_context();
LLVMTypeRef get_llvm_type(struct type_exp *type);
LLVMTargetDataRef get_llvm_data_layout();
enum OS get_os();
LLVMModuleRef get_llvm_module();
struct cg_llvm *get_cg();
struct hashtable *get_type_size_infos();
void compute_fun_info_llvm(struct target_info *ti, struct fun_info *fi);


#ifdef __cplusplus
}
#endif

#endif //__MLANG_CODEGEN_H__