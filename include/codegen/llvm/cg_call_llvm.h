/*
 * cg_call_llvm.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface emitting call site
 */

#ifndef __MLANG_CG_CALL_LLVM_H__
#define __MLANG_CG_CALL_LLVM_H__

#include <llvm-c/Core.h>

#include "clib/array.h"
#include "codegen/llvm/cg_llvm.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

LLVMValueRef emit_call_node(struct cg_llvm *cg, struct ast_node *node);

#ifdef __cplusplus
}
#endif

#endif
