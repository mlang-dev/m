/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * JIT (just-in-time compiler)
 */
#include "jit.h"
#include "util.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"

using namespace llvm;

JIT* jit_new(code_generator* cg)
{
    auto jit = new JIT();
    jit->cg = cg;
    jit->mjit = new llvm::orc::KaleidoscopeJIT();
    return jit;
}

void jit_free(JIT* jit)
{
    parser_free(jit->cg->parser);
    cg_free(jit->cg);
    delete jit->mjit;
    delete jit;
}
