/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Compiling from mlang syntax to object file or ir/bitcode
 */
#include "compiler.h"
#include "jit.h"
#include "util.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;
using namespace std;

int generate_object_file(Module* module, const char* filename);
int generate_bitcode_file(Module* module, const char* filename);

int compile(const char* fn, object_file_type file_type)
{
    auto filename = get_filename(fn);
    parser* parser = create_parser(fn, false);
    code_generator* cg = create_code_generator(parser);
    create_builtins(parser, cg->context);
    create_module_and_pass_manager(cg, filename.c_str());
    generate_runtime_module(cg, parser);
    block_node* block = parse_block(parser, nullptr);
    if (block) {
        for (auto node : block->nodes) {
            generate_code(cg, node);
        }
        if (file_type == FT_OBJECT) {
            filename += ".o";
            generate_object_file(cg->module.get(), filename.c_str());
        } else if (file_type == FT_BITCODE) {
            filename += ".bc";
            generate_bitcode_file(cg->module.get(), filename.c_str());
        } else if (file_type == FT_IR) {
            filename += ".bc";
            generate_bitcode_file(cg->module.get(), filename.c_str());
        }
    } else {
        log(INFO, "no statement is found.");
    }
    destroy_code_generator(cg);
    destroy_parser(parser);
    return 0;
}

int gof_initialize()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    // InitializeAllTargetInfos();
    // InitializeAllTargets();
    // InitializeAllTargetMCs();
    // InitializeAllAsmParsers();
    // InitializeAllAsmPrinters();
    return 0;
}

int gof_emit_file(Module* module, TargetMachine* target_machine, const char* filename)
{
    error_code ec;
    raw_fd_ostream dest(filename, ec, sys::fs::OF_None);
    if (ec) {
        errs() << "Could not open file: " << ec.message();
        return 1;
    }

    legacy::PassManager pass;
    auto file_type = CodeGenFileType::CGFT_ObjectFile;
    if (target_machine->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
        errs() << "Target machine can't emit an object file";
        return 2;
    }
    pass.run(*module);
    dest.flush();
    printf("generated obj file: %s\n", filename);
    return 0;
}

TargetMachine* gof_create_target_machine(Module* module)
{
    auto target_triple = sys::getDefaultTargetTriple();
    module->setTargetTriple(target_triple);
    string error;
    auto target = TargetRegistry::lookupTarget(target_triple, error);
    if (!target) {
        errs() << error;
        return nullptr;
    }
    auto cpu = "generic";
    auto features = "";
    TargetOptions opt;
    auto rm = Optional<Reloc::Model>();
    auto target_machine = target->createTargetMachine(target_triple, cpu, features, opt, rm);
    module->setDataLayout(target_machine->createDataLayout());
    return target_machine;
}

int generate_object_file(Module* module, const char* filename)
{
    gof_initialize();
    auto target_machine = gof_create_target_machine(module);
    if (!target_machine)
        return 1;
    //dumpm(module);
    return gof_emit_file(module, target_machine, filename);
}

int generate_bitcode_file(Module* module, const char* filename)
{
    error_code ec;
    raw_fd_ostream dest(filename, ec, sys::fs::OF_None);
    if (ec) {
        errs() << "Could not open file: " << ec.message();
        return 1;
    }
    llvm::WriteBitcodeToFile(*module, dest);
    dest.flush();
    printf("generated bc file: %s\n", filename);
    return 0;
}

int generate_ir_file(Module* module, const char* filename)
{
    error_code ec;
    raw_fd_ostream dest(filename, ec, sys::fs::OF_None);
    if (ec) {
        errs() << "Could not open file: " << ec.message();
        return 1;
    }
    module->print(dest, nullptr);
    dest.flush();
    printf("generated ir file: %s\n", filename);
    return 0;
}
