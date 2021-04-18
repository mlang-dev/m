/*
 * cg_var.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for LLVM IR codegen of variable declare/definitions
 */
#ifndef __MLANG_CG_VAR_H__
#define __MLANG_CG_VAR_H__

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

LLVMValueRef emit_var_node(struct code_generator *cg, struct exp_node *node);
LLVMValueRef get_global_variable(struct code_generator *cg, const char *name);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CG_VAR_H__
