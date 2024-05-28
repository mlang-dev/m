#include "codegen/mlir/mlir_api.h"
#include "mlir-c/IR.h"
#include "mlir-c/Pass.h"
#include "mlir-c/Conversion.h"
#include "mlir-c/Target/LLVMIR.h"
#include "mlir-c/Dialect/MemRef.h"
#include "mlir-c/Dialect/LLVM.h"
#include "mlir-c/RegisterEverything.h"
#include <stdio.h>
#include <stdlib.h>

static void printToStderr(MlirStringRef str, void *userData) {
  (void)userData;
  fwrite(str.data, 1, str.length, stderr);
}

void register_all_dialects(MlirContext ctx) {
  mlirRegisterAllPasses();

  MlirDialectRegistry registry = mlirDialectRegistryCreate();
  mlirRegisterAllDialects(registry);
  mlirContextAppendDialectRegistry(ctx, registry);
  mlirDialectRegistryDestroy(registry);
  mlirDialectHandleRegisterDialect(mlirGetDialectHandle__llvm__(), ctx);
  mlirDialectHandleRegisterDialect(mlirGetDialectHandle__memref__(), ctx);
  mlirRegisterAllLLVMTranslations(ctx);
}

MlirOperation run_passes(MlirContext ctx, MlirModule module) {
  //run PM
  MlirPassManager pm = mlirPassManagerCreate(ctx);
  mlirPassManagerAddOwnedPass(pm, mlirCreateConversionConvertToLLVMPass());
  //mlirPassManagerAddOwnedPass(pm, mlirCreateConversionFinalizeMemRefToLLVMConversionPass());
  MlirOperation operation = mlirModuleGetOperation(module);
  // if (mlirOperationVerify(operation)) {
  //   fprintf(stderr, "Verification of module failed\n");
  //   exit(1);
  // }  
  mlirPassManagerRunOnOp(pm, operation);
  mlirPassManagerDestroy(pm);
  return operation;
}

// Function to convert an MLirModule to LLVMModuleRef
LLVMModuleRef lower_to_llvm_module(MlirContext ctx, MlirModule mlirModule, LLVMContextRef llvmCtx) {
    // Run the pass manager on the module
    //MlirOperation moduleOp = run_passes(ctx, mlirModule);
    MlirOperation moduleOp = mlirModuleGetOperation(mlirModule);
    // Convert the MLIR LLVM dialect module to an LLVM IR module.
    // This step typically involves C++ API, but here we assume there's a corresponding C function.
    // Note: As of last checks, this step doesn't have a direct C API and would need to be handled in C++ or exposed via a custom C API.
    mlirOperationPrint(moduleOp, printToStderr, 0);
    LLVMModuleRef llvmModule = mlirTranslateModuleToLLVMIR(moduleOp, llvmCtx);
    LLVMDumpModule(llvmModule);
    return llvmModule;
}
