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
#include <llvm-c/Analysis.h>
#include <llvm-c/Target.h>
#include <llvm-c/Error.h>
#include <assert.h>

void *_create_jit_instance()
{
    // Create the JIT instance.
    LLVMOrcLLJITRef jit;

    LLVMErrorRef err = LLVMOrcCreateLLJIT(&jit, 0);
    if (err) {
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
        _destroy_jit_instance(jit->instance);
    }
    FREE(jit);
}

int _handle_error(LLVMErrorRef Err) {
  char *err_msg = LLVMGetErrorMessage(Err);
  fprintf(stderr, "Error: %s\n", err_msg);
  LLVMDisposeErrorMessage(err_msg);
  return 1;
}

void* jit_add_module(struct JIT *jit, void *module)
{
    char *message = 0;
    if(LLVMVerifyModule((LLVMModuleRef)module, LLVMPrintMessageAction, &message)) {
        printf("error: %s\n", message);
        exit(1);
    }
    if(message) {
        LLVMDisposeMessage(message);
    }
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)jit->instance;
    LLVMOrcThreadSafeContextRef tsc = LLVMOrcCreateNewThreadSafeContext();
    LLVMOrcThreadSafeModuleRef tsm = LLVMOrcCreateNewThreadSafeModule((LLVMModuleRef)module, tsc);
    LLVMOrcDisposeThreadSafeContext(tsc);
    LLVMOrcJITDylibRef jd = LLVMOrcLLJITGetMainJITDylib(j);
    LLVMOrcResourceTrackerRef rt = LLVMOrcJITDylibCreateResourceTracker(jd);
    //LLVMOrcDefinitionGeneratorRef dg;
#ifdef __APPLE__ //MacOS
    LLVMOrcCreateDynamicLibrarySearchGeneratorForProcess(&dg, '_', 0, 0);
    LLVMLoadLibraryPermanently("/usr/lib/libstdc++.so");
#else //Linux
    // if(LLVMOrcCreateDynamicLibrarySearchGeneratorForProcess(&dg, 0, 0, 0)) {
    //     assert(false); //something bad
    // }
    //LLVMLoadLibraryPermanently("/usr/lib/x86_64-linux-gnu/libstdc++.so.6");
#endif
    //LLVMOrcJITDylibAddGenerator(jd, dg);
    LLVMErrorRef err;
    if((err = LLVMOrcLLJITAddLLVMIRModuleWithRT(j, rt, tsm))){
        _handle_error(err);
        LLVMOrcDisposeThreadSafeModule(tsm);
        assert(false); //something bad
    };
    return rt;
}

void jit_remove_module(void *resource_tracker)
{
    LLVMOrcResourceTrackerRemove(resource_tracker);
    LLVMOrcReleaseResourceTracker(resource_tracker);
}

struct fun_pointer jit_find_symbol(struct JIT *jit, const char *symbol)
{
    LLVMOrcLLJITRef j = (LLVMOrcLLJITRef)jit->instance;
    LLVMOrcExecutorAddress addr;
    if (LLVMOrcLLJITLookup(j, &addr, symbol)) {
    }
    struct fun_pointer fp;
    fp.fp.address = (void*)addr;
    return fp;
}
