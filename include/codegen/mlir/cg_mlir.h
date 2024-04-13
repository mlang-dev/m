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
};

struct cg_mlir *cg_mlir_new(struct sema_context *sema_context);
void cg_mlir_free(struct cg_mlir *cg);

#endif
