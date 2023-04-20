/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine targeting llvm backend
 */
#include "compiler/engine.h"
#include "sema/frontend.h"
#include "codegen/llvm/cg_llvm.h"

struct codegen *_cg_llvm_new(struct sema_context *context)
{
    return (struct codegen *)cg_llvm_new(context);
}

void _cg_llvm_free(struct codegen *cg)
{
    cg_llvm_free((struct cg_llvm*)cg);
}

void _cg_llvm_reset(struct codegen *cg, struct sema_context *context)
{
    cg_llvm_reset((struct cg_llvm*)cg, context);
}

struct engine *engine_llvm_new(const char *sys_path, bool is_repl)
{
    struct engine *engine;
    MALLOC(engine, sizeof(*engine));
    engine->fe = frontend_llvm_init(sys_path, is_repl);
    engine->be = backend_init(engine->fe->sema_context, _cg_llvm_new, _cg_llvm_free, _cg_llvm_reset);
    return engine;
}
