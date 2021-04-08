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
#include "codegen/env.h"
#include "codegen/target_info.h"
#include "parser/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

void create_ir_module(struct env *cg, const char *module_name);
LLVMValueRef emit_ir_code(struct env *cg, struct exp_node *node);
void generate_runtime_module(struct env *cg);
LLVMTargetMachineRef create_target_machine(LLVMModuleRef module);
LLVMContextRef get_llvm_context();
LLVMTypeRef get_llvm_type(struct type_exp *type);
LLVMTypeRef get_llvm_type_for_abi(struct type_exp *type);
LLVMTargetDataRef get_llvm_data_layout();
enum OS get_os();
LLVMModuleRef get_llvm_module();
struct hashtable *get_type_size_infos();
struct hashtable *get_fun_infos();

#define is_int_type(type) (type == TYPE_INT || type == TYPE_BOOL || type == TYPE_CHAR)

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CODEGEN_H__
