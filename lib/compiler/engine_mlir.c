/*
 * Copyright (C) 2024 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine targeting MLIR middle-end
 */
#include "compiler/engine.h"
#include "sema/frontend.h"
#include "sema/analyzer.h"
#include "codegen/mlir/cg_mlir.h"


struct codegen *_cg_mlir_new(struct sema_context *context)
{
    return (struct codegen *)cg_mlir_new(context);
}

void _cg_mlir_free(struct codegen *cg)
{
    cg_mlir_free((struct cg_mlir*)cg);
}

char *_cg_mlir_emit_ir_string(void *gcg, struct ast_node *ast_node)
{
    struct cg_mlir *cg = (struct cg_mlir *)gcg;
    if (!ast_node)
        return 0;
    analyze(cg->base.sema_context, ast_node);
    //emit_sp_code(cg);
    emit_mlir_code(cg, ast_node);
    return 0;//LLVMPrintModuleToString(cg->module);
}

struct engine *engine_mlir_new(const char *sys_path, bool is_repl)
{
    struct engine *engine;
    MALLOC(engine, sizeof(*engine));
    engine->fe = frontend_sys_init(sys_path, is_repl);
    engine->be = backend_init(engine->fe->sema_context, _cg_mlir_new, _cg_mlir_free);
    engine->emit_ir_string = _cg_mlir_emit_ir_string;
    return engine;
}
