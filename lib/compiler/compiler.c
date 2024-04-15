/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Compiling from mlang syntax to object file or ir/bitcode
 */
#include "compiler/compiler.h"
#include "clib/util.h"
#include "compiler/jit.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

int gof_emit_file(LLVMModuleRef module, LLVMTargetMachineRef target_machine, const char *filename)
{
    printf("emitting object file: %s\n", filename);
    if (LLVMTargetMachineEmitToFile(target_machine, module, (char *)filename, LLVMObjectFile, 0)) {
        printf("Target machine can't emit an object file\n");
        return 2;
    }
    printf("generated obj file: %s\n", filename);
    return 0;
}

int gof_initialize(void)
{
    //LLVMInitializeCore(LLVMGetGlobalPassRegistry());

    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    // InitializeAllTargetInfos();
    // InitializeAllTargets();
    // InitializeAllTargetMCs();
    // InitializeAllAsmParsers();
    // InitializeAllAsmPrinters();
    return 0;
}

int generate_object_file(LLVMModuleRef module, const char *filename)
{
    gof_initialize();
    LLVMTargetDataRef target_data = 0;
    LLVMTargetMachineRef target_machine = create_target_machine(module, &target_data);
    if (!target_machine)
        return 1;
    int result = gof_emit_file(module, target_machine, filename);
    LLVMDisposeTargetMachine(target_machine);
    LLVMDisposeTargetData(target_data);
    return result;
}

int generate_bitcode_file(LLVMModuleRef module, const char *filename)
{
    LLVMWriteBitcodeToFile(module, filename);
    printf("generated bc file: %s\n", filename);
    return 0;
}

int generate_ir_file(LLVMModuleRef module, const char *filename)
{
    LLVMPrintModuleToFile(module, filename, 0);
    return 0;
}

int compile(const char *sys_path, const char *source_file, enum object_file_type file_type, const char *output_filepath)
{
    string filename;
    string_init_chars(&filename, source_file);
    string_substr(&filename, '.');
    struct engine *engine = engine_llvm_new(sys_path, false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    create_ir_module(cg, string_get(&filename));
    struct ast_node *block = parse_file(engine->fe->parser, source_file);
    analyze(cg->base.sema_context, block);
    emit_code(cg, block);
    if (block) {
        for (size_t i = 0; i < array_size(&block->block->nodes); i++) {
            struct ast_node *node = array_get_ptr(&block->block->nodes, i);
            emit_ir_code(cg, node);
        }
        if (file_type == FT_OBJECT) {
            string_add_chars(&filename, ".o");
            if(!output_filepath) output_filepath = string_get(&filename);
            generate_object_file(cg->module, output_filepath);
        } else if (file_type == FT_BITCODE) {
            string_add_chars(&filename, ".bc");
            if(!output_filepath) output_filepath = string_get(&filename);
            generate_bitcode_file(cg->module, output_filepath);
        } else if (file_type == FT_IR) {
            string_add_chars(&filename, ".ir");
            if(!output_filepath) output_filepath = string_get(&filename);
            generate_ir_file(cg->module, output_filepath);
        }
    } else {
        log_info(INFO, "no statement is found.");
    }
    engine_free(engine);
    string_deinit(&filename);
    return 0;
}

void free_ir_string(char *ir_string)
{
    LLVMDisposeMessage(ir_string);
}
