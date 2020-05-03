/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Compiling from mlang syntax to object file or ir/bitcode
 */
#include "compiler.h"
#include "jit.h"
#include "clib/util.h"
#include "llvm-c/Core.h"
#include "llvm-c/Target.h"
#include "llvm-c/TargetMachine.h"
#include "llvm-c/BitWriter.h"

#include "env.h"

int generate_object_file(LLVMModuleRef module, const char* filename);
int generate_bitcode_file(LLVMModuleRef module, const char* filename);
int generate_ir_file(LLVMModuleRef module, const char* filename);

int compile(const char* fn, object_file_type file_type)
{
    string filename;
    string_init_chars(&filename, fn);
    string_substr(&filename, '.');
    menv* env = env_new();
    parser* parser = parser_new(fn, false, NULL);
    code_generator* cg = cg_new(env, parser);
    create_builtins(parser, cg->context);
    create_module_and_pass_manager(cg, string_get(&filename));
    generate_runtime_module(cg, parser);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    //analyze(env->type_sys, (exp_node*)block);
    if (block) {
        for (size_t i = 0; i < array_size(&block->nodes); i ++) {
            exp_node* node = *(exp_node**)array_get(&block->nodes, i);
            generate_code(cg, node);
        }
        LLVMModuleRef module = (LLVMModuleRef)cg->module;
        if (file_type == FT_OBJECT) {
            string_add_chars(&filename, ".o");
            generate_object_file(module, string_get(&filename));
        } else if (file_type == FT_BITCODE) {
            string_add_chars(&filename, ".bc");
            generate_bitcode_file(module, string_get(&filename));
        } else if (file_type == FT_IR) {
            string_add_chars(&filename, ".ir");
            generate_ir_file(module, string_get(&filename));
        }
    } else {
        log_info(INFO, "no statement is found.");
    }
    cg_free(cg);
    parser_free(parser);
    env_free(env);
    string_deinit(&filename);
    return 0;
}

int gof_initialize()
{
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());

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

int gof_emit_file(LLVMModuleRef module, LLVMTargetMachineRef target_machine, const char* filename)
{
    if (LLVMTargetMachineEmitToFile(target_machine, module, (char*)filename, LLVMObjectFile, NULL)) {
        printf("Target machine can't emit an object file\n");
        return 2;
    }
    printf("generated obj file: %s\n", filename);
    return 0;
}


int generate_object_file(LLVMModuleRef module, const char* filename)
{
    gof_initialize();
    LLVMTargetMachineRef target_machine = (LLVMTargetMachineRef)create_target_machine(module);
    if (!target_machine)
        return 1;
    return gof_emit_file(module, target_machine, filename);
}

int generate_bitcode_file(LLVMModuleRef module, const char* filename)
{
    LLVMWriteBitcodeToFile(module, filename);
    printf("generated bc file: %s\n", filename);
    return 0;
}

int generate_ir_file(LLVMModuleRef module, const char* filename)
{
    LLVMPrintModuleToFile(module, filename, NULL);
    return 0;
}
