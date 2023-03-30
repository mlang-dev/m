/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * JIT (just-in-time compiler)
 */

#include "clib/util.h"
#include "compiler/jit.h"
#include <llvm-c/Core.h>
#include <llvm-c/Initialization.h>
#include <llvm-c/LLJIT.h>
#include <llvm-c/Support.h>

void *_create_jit_instance()
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
        return 0;
    }
    return jit;
}

void _destroy_jit_instance(void *instance)
{
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)instance;
    LLVMErrorRef err = LLVMOrcDisposeLLJIT(j);
    if (err) {
        ; //something bad again
    }
}

struct JIT *jit_new(struct engine *engine)
{
    struct JIT *jit;
    MALLOC(jit, sizeof(*jit));
    jit->engine = engine;
    jit->instance = _create_jit_instance();
    return jit;
}

void jit_free(struct JIT *jit)
{
    if (jit->instance) {
        //_destroy_jit_instance(jit->jit);
        //LLVMShutdown();
    }
    FREE(jit);
}

void add_module(struct JIT *jit, void *module)
{
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)jit->instance;
    LLVMOrcJITDylibRef jd = LLVMOrcLLJITGetMainJITDylib(j);
    LLVMOrcThreadSafeContextRef tsc = LLVMOrcCreateNewThreadSafeContext();
    LLVMOrcThreadSafeModuleRef tsm = LLVMOrcCreateNewThreadSafeModule((LLVMModuleRef)module, tsc);
    LLVMOrcDefinitionGeneratorRef dg;
#ifdef __APPLE__ //MacOS
    LLVMOrcCreateDynamicLibrarySearchGeneratorForProcess(&dg, '_', 0, 0);
    LLVMLoadLibraryPermanently("/usr/lib/libstdc++.so");
#else //Linux
    LLVMOrcCreateDynamicLibrarySearchGeneratorForProcess(&dg, 0, 0, 0);
    LLVMLoadLibraryPermanently("/usr/lib/x86_64-linux-gnu/libstdc++.so.6");
#endif
    LLVMOrcJITDylibAddGenerator(jd, dg);
    LLVMOrcLLJITAddLLVMIRModule(j, jd, tsm);
    LLVMOrcDisposeThreadSafeContext(tsc);
}

struct fun_pointer find_target_address(struct JIT *jit, const char *symbol)
{
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)jit->instance;
    LLVMOrcExecutorAddress addr;
    if (LLVMOrcLLJITLookup(j, &addr, symbol)) {
    }
    struct fun_pointer fp;
    fp.fp.address = (void*)addr;
    return fp;
}
