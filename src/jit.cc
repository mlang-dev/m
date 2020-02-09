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
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

JIT* create_jit(code_generator* cg){
    auto jit = new JIT();
    jit->cg = cg;
    jit->mjit = new llvm::orc::MJIT();
    return jit;
}

void destroy_jit(JIT* jit){
    delete jit->mjit;
    delete jit;
}

