
#include "mlir-c/IR.h"
#include "mlir-c/Dialect/LLVM.h"
#include "codegen/mlir/cg_mlir.h"

struct cg_mlir *cg_mlir_new(struct sema_context *sema_context)
{
    struct cg_mlir *cg;
    MALLOC(cg, sizeof(*cg));
    cg->context = mlirContextCreate();
    mlirDialectHandleRegisterDialect(mlirGetDialectHandle__llvm__(), cg->context);
    mlirContextGetOrLoadDialect(cg->context, mlirStringRefCreateFromCString("llvm"));    
    return cg;
}

void cg_mlir_free(struct cg_mlir *cg)
{
    mlirContextDestroy(cg->context);
    free(cg);
}
