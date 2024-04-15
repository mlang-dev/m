/*
 * Copyright (C) 2024 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine targeting MLIR middle-end
 */
#include "compiler/engine.h"
#include "sema/frontend.h"
#include "codegen/mlir/cg_mlir.h"

struct codegen *_cg_mlir_new(struct sema_context *context)
{
    return (struct codegen *)cg_mlir_new(context);
}

void _cg_mlir_free(struct codegen *cg)
{
    cg_mlir_free((struct cg_mlir*)cg);
}

struct engine *engine_mlir_new(const char *sys_path, bool is_repl)
{
    struct engine *engine;
    MALLOC(engine, sizeof(*engine));
    engine->fe = frontend_sys_init(sys_path, is_repl);
    engine->be = backend_init(engine->fe->sema_context, _cg_mlir_new, _cg_mlir_free);
    return engine;
}
