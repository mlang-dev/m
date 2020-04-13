/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Compiling from mlang syntax to object file or ir/bitcode
 */
#include "compiler.h"
#include "jit.h"
#include "util.h"
#include "llvm-c/Core.h"
#include "llvm-c/Target.h"
#include "llvm-c/TargetMachine.h"
#include "llvm-c/BitWriter.h"

#include "env.h"

using namespace llvm;
using namespace std;

int generate_object_file(LLVMModuleRef module, const char* filename);
int generate_bitcode_file(LLVMModuleRef module, const char* filename);
int generate_ir_file(LLVMModuleRef module, const char* filename);

int compile(const char* fn, object_file_type file_type)
{
    auto filename = get_filename(fn);
    menv* env = env_new();
    parser* parser = parser_new(fn, false);
    code_generator* cg = cg_new(env, parser);
    create_builtins(parser, cg->context);
    create_module_and_pass_manager(cg, filename.c_str());
    generate_runtime_module(cg, parser);
    block_node* block = parse_block(parser, nullptr);
    if (block) {
        for (auto node : block->nodes) {
            generate_code(cg, node);
        }
        LLVMModuleRef module = wrap((llvm::Module*)cg->module);
        if (file_type == FT_OBJECT) {
            filename += ".o";
            generate_object_file(module, filename.c_str());
        } else if (file_type == FT_BITCODE) {
            filename += ".bc";
            generate_bitcode_file(module, filename.c_str());
        } else if (file_type == FT_IR) {
            filename += ".ir";
            generate_ir_file(module, filename.c_str());
        }
    } else {
        log(INFO, "no statement is found.");
    }
    cg_free(cg);
    parser_free(parser);
    env_free(env);
    return 0;
}

int gof_initialize()
{
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
    if (LLVMTargetMachineEmitToFile(target_machine, module, (char*)filename, LLVMCodeGenFileType::LLVMObjectFile, NULL)) {
        errs() << "Target machine can't emit an object file";
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
    //dumpm(module);
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
