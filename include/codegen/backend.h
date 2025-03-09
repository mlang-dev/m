/*
 * backend.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for backend data structures
 */
#ifndef __MLANG_BACKEND_H__
#define __MLANG_BACKEND_H__

#include "sema/sema_context.h"
#include "clib/hashtable.h"
#include "codegen/fun_info.h"
#include "codegen/codegen.h"
#include "codegen/mlir/cg_mlir.h"

#ifdef __cplusplus
extern "C" {
#endif

struct backend{
    /*
     *  custom code generator
     */
    void *cg;
    cg_free_fun cg_free;
};

struct backend *backend_init(struct sema_context *sema_context, cg_alloc_fun cg_alloc, cg_free_fun cg_free);
void backend_deinit(struct backend *be);

struct engine *engine_mlir_new(const char *sys_path, bool is_repl);

#ifdef __cplusplus
}
#endif

#endif
