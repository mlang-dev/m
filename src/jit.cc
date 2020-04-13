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

JIT* create_jit(code_generator* cg)
{
    auto jit = new JIT();
    jit->cg = cg;
    jit->mjit = new llvm::orc::KaleidoscopeJIT();
    return jit;
}

void destroy_jit(JIT* jit)
{
    destroy_parser(jit->cg->parser);
    destroy_code_generator(jit->cg);
    delete jit->mjit;
    delete jit;
}
