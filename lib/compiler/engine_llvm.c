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
    return (struct codegen *)llvm_cg_new(context);
}

void _cg_llvm_free(struct codegen *cg)
{
    llvm_cg_free((struct cg_llvm*)cg);
}

struct engine *engine_llvm_new(bool is_repl)
{
    struct engine *engine;
    MALLOC(engine, sizeof(*engine));
    engine->fe = frontend_llvm_init("mlib/stdio.m", "mlib/math.m", is_repl);
    engine->be = backend_init(engine->fe->sema_context, _cg_llvm_new, _cg_llvm_free);
    return engine;
}
