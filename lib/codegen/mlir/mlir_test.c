#include "mlir-c/IR.h"
#include "mlir-c/RegisterEverything.h"
#include "mlir-c/BuiltinAttributes.h"
#include "mlir-c/Dialect/LLVM.h"
#include "mlir-c/BuiltinTypes.h"

#include "mlir-c/Pass.h"
#include "mlir-c/Conversion.h"
#include "mlir-c/Target/LLVMIR.h"
#include "mlir-c/Dialect/MemRef.h"

#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include "codegen/mlir/mlir_api.h"

// CHECK-LABEL: testToLLVMIR()
static void testToLLVMIR(MlirContext ctx) {
  fprintf(stderr, "testToLLVMIR()\n");

  // const char *moduleString = "llvm.func @add(%arg0: i64, %arg1: i64) -> i64 { \
  //                               %0 = llvm.add %arg0, %arg1  : i64 \
  //                               llvm.return %0 : i64 \
  //                            }";
  //const char *moduleString = "memref.global @gv0 : memref<2xf32> = uninitialized";
//   const char *moduleString = "\"builtin.module\"() ({\
// \"memref.global\"() {initial_value = dense<[10]> : tensor<1xi32>, sym_name = \"m\", type = memref<1xi32>} : () -> ()\
// }) : () -> ()";
//   const char *moduleString = "\"builtin.module\"() ({\
//   \"llvm.mlir.global\"() ({}) {linkage=\"external\", global_type = i32, sym_name = \"m\", value = 10 : i32} : () -> ()\
// }) : () -> ()";
  //const char *moduleString = "llvm.mlir.global @m(42 : i32) : i32";
  const char *moduleString = "\n"
"\"builtin.module\" () ({\n"
"\"llvm.mlir.global\"() ({}) {linkage=common, global_type = i32, sym_name = \"m\", value = 10 : i32} : () -> ()\n"
"}) : () -> ()";
  MlirModule module =
      mlirModuleCreateParse(ctx, mlirStringRefCreateFromCString(moduleString));

  MlirOperation operation = run_passes(ctx, module);

  LLVMContextRef llvmCtx = LLVMContextCreate();
  LLVMModuleRef llvmModule = mlirTranslateModuleToLLVMIR(operation, llvmCtx);

  // clang-format off
  // CHECK: define i64 @add(i64 %[[arg1:.*]], i64 %[[arg2:.*]]) {
  // CHECK-NEXT:   %[[arg3:.*]] = add i64 %[[arg1]], %[[arg2]]
  // CHECK-NEXT:   ret i64 %[[arg3]]
  // CHECK-NEXT: }
  // clang-format on
  LLVMDumpModule(llvmModule);

  LLVMDisposeModule(llvmModule);
  mlirModuleDestroy(module);
  LLVMContextDispose(llvmCtx);
}

void test_mlir_to_llvm(void)
{
  MlirContext ctx = mlirContextCreate();
  register_all_dialects(ctx);
  testToLLVMIR(ctx);
  mlirContextDestroy(ctx);
}
