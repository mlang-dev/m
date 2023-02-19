/*
 * cg_var_llvm.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for LLVM IR codegen of variable declare/definitions
 */
#ifndef __MLANG_CG_VAR_LLVM_H__
#define __MLANG_CG_VAR_LLVM_H__

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

LLVMValueRef emit_var_node(struct cg_llvm *cg, struct ast_node *node);
LLVMValueRef get_global_variable(struct cg_llvm *cg, symbol gv_name);
LLVMValueRef emit_record_init_node(struct cg_llvm *cg, struct ast_node *member_values, bool is_ret, const char *name);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CG_VAR_H__
