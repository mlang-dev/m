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
typedef LLVMTypeRef (*get_ir_type_func)(LLVMContextRef context, struct type_expr *type);
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

    /* 
     *  symboltable of <symbol, LLVMTypeRef>
     *  binding type name to IR Type
     */
    struct hashtable typename_2_irtypes;

    /* 
     *  symboltable of <symbol, LLVMValueRef>
     *  binding variable name to LLVMValueRef
     */
    struct hashtable varname_2_irvalues;

    /* generic data structures*/
    struct sema_context *sema_context;
    struct ops ops[TYPE_TYPES];

    /* 
     *  symboltable of <symbol, var ast_node*>
     *  starting with empty binding codegened global variable name to var ast_node*
     */
    struct hashtable cg_gvar_name_2_asts;

    /* 
     *  symboltable of <symbol, symbol>
     *  binding variable name to type name
     */
    struct hashtable varname_2_typename;


    /// target info
    struct target_info *target_info;

    fn_compute_fun_info compute_fun_info;
};


struct cg_llvm *llvm_cg_new(struct sema_context *sema_context);
void llvm_cg_free(struct cg_llvm *cg);

void emit_code(struct cg_llvm *cg, struct ast_node *node);
void emit_sp_code(struct cg_llvm *cg);
void create_ir_module(struct cg_llvm *cg, const char *module_name);
LLVMValueRef emit_ir_code(struct cg_llvm *cg, struct ast_node *node);
LLVMTargetMachineRef create_target_machine(LLVMModuleRef module);
LLVMContextRef get_llvm_context();
LLVMTypeRef get_llvm_type(struct type_expr *type);
LLVMTargetDataRef get_llvm_data_layout();
enum OS get_os();
LLVMModuleRef get_llvm_module();
struct cg_llvm *get_cg();
struct hashtable *get_type_size_infos();


#ifdef __cplusplus
}
#endif

#endif //__MLANG_CODEGEN_H__
