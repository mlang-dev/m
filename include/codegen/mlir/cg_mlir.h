/*
 * cg_mlir.h
 * 
 * Copyright (C) 2024 Ligang Wang <ligangwangs@gmail.com>
 * header file for MLIR codegen
 */
#ifndef __MLANG_CG_MLIR_H__
#define __MLANG_CG_MLIR_H__

#include "mlir-c/IR.h"
#include "sema/sema_context.h"
#include "codegen/codegen.h"

struct cg_mlir{
    struct codegen base;
    MlirContext context;
    MlirModule module;
};

struct cg_mlir *cg_mlir_new(struct sema_context *sema_context);
void cg_mlir_free(struct cg_mlir *cg);
MlirValue emit_mlir_code(struct cg_mlir *cg, struct ast_node *node);
void* create_mlir_module(void* gcg, const char *module_name);

#endif
