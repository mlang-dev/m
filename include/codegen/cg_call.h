/*
 * abi_ir_arg.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface mapping from abi arg infos lowing to ir args
 */
#ifndef __MLANG_CG_CALL_H__
#define __MLANG_CG_CALL_H__

#include "clib/array.h"
#include "parser/ast.h"
#include <llvm-c/Core.h>

#ifdef __cplusplus
extern "C" {
#endif

struct address {
    LLVMValueRef pointer;
    unsigned alignment;
};

struct address zero_address();
void emit_call(struct call_node *call);

#ifdef __cplusplus
}
#endif

#endif
