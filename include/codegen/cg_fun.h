/*
 * cg_fun.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for LLVM IR codegen of functions
 */
#ifndef __MLANG_CG_FUN_H__
#define __MLANG_CG_FUN_H__

#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

#include "clib/hashset.h"
#include "clib/hashtable.h"
#include "codegen/target_info.h"
#include "sema/sema_context.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

LLVMValueRef emit_prototype_node(struct code_generator *cg, struct exp_node *node, struct fun_info **out_fi);
LLVMValueRef emit_function_node(struct code_generator *cg, struct exp_node *node);
LLVMValueRef emit_entry_block_alloca(
    LLVMTypeRef type,
    LLVMValueRef fun,
    const char *var_name);

LLVMValueRef get_llvm_function(struct code_generator *cg, const char *name);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CG_FUN_H__