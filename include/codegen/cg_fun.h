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
#include "clib/symbol.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/target_info.h"
#include "parser/ast.h"
#include "sema/sema_context.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

LLVMValueRef emit_prototype_node(struct code_generator *cg, struct exp_node *node, struct fun_info **out_fi);
LLVMValueRef emit_function_node(struct code_generator *cg, struct exp_node *node);
LLVMValueRef get_llvm_function(struct code_generator *cg, symbol fun_name);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CG_FUN_H__
