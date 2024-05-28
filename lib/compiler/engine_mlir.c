/*
 * Copyright (C) 2024 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine targeting MLIR middle-end
 */
#include "compiler/engine.h"
#include "sema/frontend.h"
#include "sema/analyzer.h"
#include "codegen/mlir/cg_mlir.h"
#include "llvm-c/Core.h"
#include <llvm-c/Support.h>
#include <llvm-c/TargetMachine.h>
#include "mlir-c/IR.h"
#include "mlir-c/Pass.h"
#include "mlir-c/Conversion.h"
#include "mlir-c/Target/LLVMIR.h"
#include "codegen/llvm/cg_llvm.h"
#include "codegen/mlir/mlir_api.h"


void emit_mlir_sp_code(struct cg_mlir *cg)
{
    for(size_t i = 0; i < array_size(&cg->base.sema_context->new_specialized_asts); i++){
        struct ast_node *new_sp = array_get_ptr(&cg->base.sema_context->new_specialized_asts, i);
        emit_mlir_code(cg, new_sp);
    }
    array_reset(&cg->base.sema_context->new_specialized_asts);
}

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
    emit_mlir_sp_code(cg);
    emit_mlir_code(cg, ast_node);
    
    LLVMContextRef llvmCtx = LLVMContextCreate();
    LLVMModuleRef llvm_module = lower_to_llvm_module(cg->context, cg->module, llvmCtx);
    LLVMTargetDataRef target_data;
    LLVMTargetMachineRef target_machine = create_target_machine(llvm_module, &target_data);
    char * llvm_ir = LLVMPrintModuleToString(llvm_module);
    LLVMDisposeModule(llvm_module);
    LLVMContextDispose(llvmCtx);
    LLVMDisposeTargetData(target_data);
    LLVMDisposeTargetMachine(target_machine);
    return llvm_ir;
}

struct engine *engine_mlir_new(const char *sys_path, bool is_repl)
{
    struct engine *engine;
    MALLOC(engine, sizeof(*engine));
    engine->fe = frontend_sys_init(sys_path, is_repl);
    engine->be = backend_init(engine->fe->sema_context, _cg_mlir_new, _cg_mlir_free);
    engine->emit_ir_string = _cg_mlir_emit_ir_string;
    engine->create_ir_module = create_mlir_module;
    return engine;
}
