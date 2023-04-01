/*
 * ir_api.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface of IR interfaces
 */
#ifndef __MLANG_IR_API_H__
#define __MLANG_IR_API_H__

#include <llvm-c/Core.h>


#ifdef __cplusplus
extern "C" {
#endif

struct address {
    LLVMValueRef pointer;
    unsigned alignment;
};

struct cg_llvm;
LLVMValueRef create_alloca(LLVMTypeRef type, unsigned align, LLVMValueRef fun, const char *var_name);
void create_coerced_store(struct cg_llvm *cg, LLVMBuilderRef builder, LLVMValueRef src, LLVMValueRef dst, unsigned align);
LLVMValueRef create_coerced_load(struct cg_llvm *cg, LLVMBuilderRef builder, LLVMValueRef src, LLVMTypeRef dst_type, unsigned align);
void add_fun_param_attribute(LLVMContextRef context, LLVMValueRef fun, unsigned arg_index, const char *attr);
void add_fun_param_type_attribute(LLVMContextRef context, LLVMValueRef fun, unsigned arg_index, const char *attr, LLVMTypeRef type);
void add_call_arg_type_attribute(LLVMContextRef context, LLVMValueRef call, unsigned arg_index, const char *attr, LLVMTypeRef type);
    LLVMValueRef coerce_int_or_ptr(LLVMValueRef value, LLVMTypeRef dst_type);

#ifdef __cplusplus
}
#endif

#endif
