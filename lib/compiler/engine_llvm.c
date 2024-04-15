/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine targeting llvm backend
 */
#include "compiler/engine.h"
#include "sema/frontend.h"
#include "codegen/llvm/cg_llvm.h"
#include "sema/analyzer.h"

struct codegen *_cg_llvm_new(struct sema_context *context)
{
    return (struct codegen *)cg_llvm_new(context);
}

void _cg_llvm_free(struct codegen *cg)
{
    cg_llvm_free((struct cg_llvm*)cg);
}

char *_cg_llvm_emit_ir_string(void *gcg, struct ast_node *ast_node)
{
    struct cg_llvm *cg = (struct cg_llvm *)gcg;
    if (!ast_node)
        return 0;
    analyze(cg->base.sema_context, ast_node);
    emit_sp_code(cg);
    emit_ir_code(cg, ast_node);
    return LLVMPrintModuleToString(cg->module);
}

struct engine *engine_llvm_new(const char *sys_path, bool is_repl)
{
    struct engine *engine;
    MALLOC(engine, sizeof(*engine));
    engine->fe = frontend_sys_init(sys_path, is_repl);
    engine->be = backend_init(engine->fe->sema_context, _cg_llvm_new, _cg_llvm_free);
    engine->emit_ir_string = _cg_llvm_emit_ir_string;
    return engine;
}
