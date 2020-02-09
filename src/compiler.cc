#include "jit.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Target/TargetMachine.h"
#include "util.h"

using namespace llvm;
using namespace std;

int generate_object_file(Module* module, const char* filename);

int compile(const char* fn){
    auto filename = get_filename(fn);
    FILE* file = fopen(fn, "r");
    parser* parser = create_parser(true, file);
    code_generator* cg = create_code_generator(parser);
    create_builtins(parser, cg->context);
    create_module_and_pass_manager(cg, filename.c_str());
    generate_runtime_module(cg, parser);
    while(true){
        parse_next_token(parser);
        //fprintf(stderr, "got token: token type: %d\n", parser->curr_token.type);
        if (parser->curr_token.type == TOKEN_EOF){
            //dumpm(jit->cg->module.get());
            break;
        }
        switch(parser->curr_token.type){
            case TOKEN_IMPORT:{
                if (auto node= parse_import(parser)){
                    parser->ast->entry_module->nodes.push_back(node);
                    if(auto v = generate_code(cg, node)){
                        //dumpf(v);
                        //fprintf(stderr, "Parsed an import\n");
                    }
                }
                break;
            }
            default:{
                if(auto node=parse_exp_or_def(parser)){
                    auto def = generate_code(cg, node);
                }
                break;
            }
        }
    }
    fclose(file);
    filename += ".o";
    generate_object_file(cg->module.get(), filename.c_str());
    destroy_code_generator(cg);
    destroy_parser(parser);
    return 0;
}


int gof_initialize(){
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    return 0;
}

int gof_emit_file(Module* module, TargetMachine* target_machine, const char* filename){
    error_code ec;
    raw_fd_ostream dest(filename, ec, sys::fs::OF_None);
    if(ec){
        errs() << "Could not open file: " << ec.message();
        return 1;
    }

    legacy::PassManager pass;
    auto file_type = TargetMachine::CodeGenFileType::CGFT_ObjectFile;
    if(target_machine->addPassesToEmitFile(pass, dest, nullptr, file_type)){
        errs() << "Target machine can't emit an object file";
        return 1;
    }
    pass.run(*module);
    dest.flush();
    return 0;
}

TargetMachine* gof_create_target_machine(Module* module){
    auto target_triple = sys::getDefaultTargetTriple();
    module->setTargetTriple(target_triple);
    string error;
    auto target = TargetRegistry::lookupTarget(target_triple, error);
    if (!target){
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

int generate_object_file(Module* module, const char* filename){
    gof_initialize();
    auto target_machine = gof_create_target_machine(module);
    if (!target_machine)
        return 1;
    return gof_emit_file(module, target_machine, filename);
}