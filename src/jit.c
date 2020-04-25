/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * JIT (just-in-time compiler)
 */
#include <stdlib.h>

#include "jit.h"
#include "llvm-c/Core.h"
#include "llvm-c/Error.h"
#include "llvm-c/Initialization.h"
#include "llvm-c/Orc.h"
#include "llvm-c/Support.h"
#include "llvm-c/Target.h"

#include "clib/util.h"

void* _create_jit_instance()
{
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());

    // Initialize native target codegen and asm printer.
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();

    // Create the JIT instance.
    LLVMOrcLLJITRef jit;
  
    LLVMErrorRef err = LLVMOrcCreateLLJIT(&jit, 0);
    if (err) {
        LLVMShutdown();
        return NULL;
    }
    return jit;
}

void _destroy_jit_instance(void *instance)
{
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)instance;
    LLVMErrorRef err = LLVMOrcDisposeLLJIT(j);
    if (err) {
        ;//something bad again
    }
}


JIT* jit_new(code_generator* cg)
{
    JIT* jit = (JIT*)malloc(sizeof(JIT));
    jit->cg = cg;
    jit->instance = _create_jit_instance();
    return jit;
}

void jit_free(JIT* jit)
{
    if (jit->instance){
        //_destroy_jit_instance(jit->jit);
        //LLVMShutdown();
    }
    parser_free(jit->cg->parser);
    cg_free(jit->cg);
    free(jit);
}

void add_module(JIT *jit, void* module)
{
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)jit->instance;
    LLVMOrcJITDylibRef jd = LLVMOrcLLJITGetMainJITDylib(j);
    LLVMOrcThreadSafeContextRef tsc = LLVMOrcCreateNewThreadSafeContext();
    LLVMOrcThreadSafeModuleRef tsm = LLVMOrcCreateNewThreadSafeModule((LLVMModuleRef)jit->cg->module, tsc);
    LLVMOrcLLJITAddLLVMIRModule(j, jd, tsm);
}

target_address find_target_address(JIT *jit, const char *symbol)
{
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)jit->instance;
    LLVMOrcJITTargetAddress addr;
    if (LLVMOrcLLJITLookup(j, &addr, symbol)) {
    }
    return (target_address)addr;
}


