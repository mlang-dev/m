#include <assert.h>
#include "mlir-c/IR.h"
#include "mlir-c/RegisterEverything.h"
#include "mlir-c/BuiltinAttributes.h"
#include "mlir-c/Dialect/LLVM.h"
#include "mlir-c/BuiltinTypes.h"
#include "codegen/mlir/cg_mlir.h"

struct cg_mlir *cg_mlir_new(struct sema_context *sema_context)
{
    struct cg_mlir *cg;
    MALLOC(cg, sizeof(*cg));
    cg->base.sema_context = sema_context;
    cg->context = mlirContextCreate();
    MlirDialectRegistry registry = mlirDialectRegistryCreate();
    mlirRegisterAllDialects(registry);
    mlirContextAppendDialectRegistry(cg->context, registry);
    mlirDialectRegistryDestroy(registry);
    mlirRegisterAllPasses();
    cg->module = mlirModuleCreateEmpty(mlirLocationUnknownGet(cg->context));

    return cg;
}

void cg_mlir_free(struct cg_mlir *cg)
{
    mlirModuleDestroy(cg->module);
    mlirContextDestroy(cg->context);
    free(cg);
}

MlirValue _emit_mlir_operation(struct cg_mlir *cg, MlirStringRef op_name, MlirType type, int value)
{
    MlirOperationState opState = mlirOperationStateGet(op_name, mlirLocationUnknownGet(cg->context));
    MlirAttribute valueAttr = mlirIntegerAttrGet(type, value);
    const char *myString = "value";
    MlirStringRef strRef = mlirStringRefCreate(myString, strlen(myString));
    MlirNamedAttribute namedAttr = mlirNamedAttributeGet(mlirIdentifierGet(cg->context, strRef), valueAttr);
    mlirOperationStateAddAttributes(&opState, 1, &namedAttr); //inerting one attribute
    mlirOperationStateAddResults(&opState, 1, &type);
    MlirOperation constOp = mlirOperationCreate(&opState);
    MlirBlock moduleBody = mlirModuleGetBody(cg->module); //get the body of the module
    mlirBlockInsertOwnedOperation(moduleBody, 0, constOp); //insert the operation into the module
    return mlirOperationGetResult(constOp, 0);
}

MlirValue _emit_global_var(struct cg_mlir *cg, const char* name, MlirType type)
{
    const char *op_name = "memref.global";
    MlirStringRef strOpName = mlirStringRefCreate(op_name, strlen(op_name));
    MlirOperationState opState = mlirOperationStateGet(strOpName, mlirLocationUnknownGet(cg->context));
    MlirAttribute symNameAttr = mlirStringAttrGet(cg->context, mlirStringRefCreate(name, strlen(name))); 
    MlirAttribute initialValue = mlirIntegerAttrGet(type, 0);
    MlirAttribute typeAttr = mlirTypeAttrGet(type);
    MlirIdentifier sym_name_id = mlirIdentifierGet(cg->context, mlirStringRefCreate("sym_name", strlen("sym_name")));
    MlirIdentifier type_id = mlirIdentifierGet(cg->context, mlirStringRefCreate("type", strlen("type")));
    MlirIdentifier initial_value_id = mlirIdentifierGet(cg->context, mlirStringRefCreate("initial_value", strlen("initial_value")));
    mlirOperationStateAddAttributes(&opState, 3, (MlirNamedAttribute[]){
        mlirNamedAttributeGet(sym_name_id, symNameAttr),
        mlirNamedAttributeGet(type_id, typeAttr),
        mlirNamedAttributeGet(initial_value_id, initialValue)
    }); //inerting one attribute
    mlirOperationStateAddResults(&opState, 1, &type);
    MlirOperation op = mlirOperationCreate(&opState);
    MlirBlock moduleBody = mlirModuleGetBody(cg->module); //get the body of the module
    mlirBlockInsertOwnedOperation(moduleBody, 0, op); //insert the operation into the module
    return mlirOperationGetResult(op, 0);
}

MlirValue _emit_mlir_literal_node(struct cg_mlir *cg, struct ast_node *node)
{
    assert(node->type);
    assert(node->node_type == LITERAL_NODE);
    struct type_context *tc = cg->base.sema_context->tc;
    enum type type = get_type(tc, node->type);
    MlirType mlir_type = mlirIntegerTypeGet(cg->context, 32);
    MlirValue value = {0};
    if (is_int_type(type)){
        const char *myString = "llvm.mlir.constant";
        MlirStringRef strRef = mlirStringRefCreate(myString, strlen(myString));
        value = _emit_mlir_operation(cg, strRef, mlir_type, node->liter->int_val);
    }
    else if (type == TYPE_F64){
        //value = &node->liter->double_val;
    }
    else if (type == TYPE_STRING) {
        //value = (void *)node->liter->str_val;
    }
    return value;
}

MlirValue _emit_mlir_var_node(struct cg_mlir *cg, struct ast_node *node)
{
    return _emit_global_var(cg, string_get(node->var->var->ident->name), mlirIntegerTypeGet(cg->context, 32));
}

MlirValue emit_mlir_code(struct cg_mlir *cg, struct ast_node *node)
{
    if(node->transformed) 
        node = node->transformed;
    MlirValue value = {0};
    switch(node->node_type){
        case LITERAL_NODE:
            value = _emit_mlir_literal_node(cg, node);
            break;
        // case IDENT_NODE:
        //     value = _emit_ident_node(cg, node);
        //     break;
        // case NEW_NODE:
        //     value = _emit_new_node(cg, node);
        //     break;
        // case DEL_NODE:
        //     value = _emit_del_node(cg, node);
        //     break;
        case VAR_NODE:
            value = _emit_mlir_var_node(cg, node);
            break;
        // case ADT_INIT_NODE:
        //     value = emit_struct_init_node(cg, node, false, "");
        //     break;
        // case ARRAY_INIT_NODE:
        //     value = emit_array_init_node(cg, node, false, "");
        //     break;        
        // case UNARY_NODE:
        //     value = _emit_unary_node(cg, node);
        //     break;
        // case MEMBER_INDEX_NODE:
        //     if(node->index->object->type->type == TYPE_ARRAY)
        //         value = _emit_array_index(cg, node);
        //     else
        //         value = _emit_field_access_node(cg, node);
        //     break;
        // case ASSIGN_NODE:
        //     value = _emit_assign_node(cg, node);
        //     break;
        // case BINARY_NODE:
        //     value = _emit_binary_node(cg, node);
        //     break;
        // case IF_NODE:
        //     value = _emit_condition_node(cg, node);
        //     break;
        // case WHILE_NODE:
        //     value = _emit_while_node(cg, node);
        //     break;
        // case FOR_NODE:
        //     value = _emit_for_node(cg, node);
        //     break;
        // case JUMP_NODE:
        //     value = _emit_jump_node(cg, node);
        //     break;
        // case CALL_NODE:
        //     value = emit_call_node(cg, node);
        //     break;
        // case FUNC_TYPE_NODE:
        //     value = emit_func_type_node(cg, node);
        //     break;
        // case FUNC_NODE:
        //     value = emit_function_node(cg, node);
        //     break;
        // case BLOCK_NODE:
        //     value = _emit_block_node(cg, node);
        //     break;
        case CAST_NODE:
        case MATCH_NODE:
            break;

        case TYPE_EXPR_ITEM_NODE:
        case MATCH_CASE_NODE:
        case WILDCARD_NODE:
        case VARIANT_NODE:
        case VARIANT_TYPE_ITEM_NODE:
        case ARRAY_TYPE_NODE:
        case TYPE_ITEM_NODE:
        case TYPE_NODE:
        case STRUCT_NODE:
        case NULL_NODE:
        case IMPORT_NODE:
        case MEMORY_NODE:
        case RANGE_NODE:
        case TOTAL_NODE:
        case TOKEN_NODE:
            break;
        default:
            break;
    }
    return value;
}

void* create_mlir_module(void* gcg, const char *module_name)
{
    // struct cg_mlir *cg = (struct cg_mlir *)gcg;
    // MlirModule module = mlirModuleCreateWithName(mlirLocationUnknownGet(cg->context), module_name);
    // return module;
    return 0;
}
