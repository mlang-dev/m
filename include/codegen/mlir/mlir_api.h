#ifndef __MLANG_MLIR_API_H__
#define __MLANG_MLIR_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "mlir-c/IR.h"
#include "llvm-c/Core.h"

void register_all_dialects(MlirContext ctx);
LLVMModuleRef lower_to_llvm_module(MlirContext ctx, MlirModule mlirModule, LLVMContextRef llvmCtx);
MlirOperation run_passes(MlirContext ctx, MlirModule module);

#if defined(__cplusplus)
}   
#endif

#endif
