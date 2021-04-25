#include "codegen/ir_api.h"
#include "codegen/codegen.h"
#include "sema/type.h"
#include <assert.h>
#include <llvm-c/Target.h>

struct address create_temp_mem(struct type_oper *to, unsigned align, symbol name)
{
    struct address adr;
    return adr;
}

LLVMValueRef create_alloca(LLVMTypeRef type, unsigned align, LLVMValueRef fun, const char *var_name)
{
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMBasicBlockRef bb = LLVMGetEntryBasicBlock(fun);
    LLVMPositionBuilder(builder, bb, LLVMGetFirstInstruction(bb));
    LLVMValueRef alloca = LLVMBuildAlloca(builder, type, var_name);
    LLVMSetAlignment(alloca, align);
    LLVMDisposeBuilder(builder);
    return alloca;
}

/// Access some number of bytes from a struct point
LLVMValueRef _access_bytes_struct(LLVMBuilderRef builder, LLVMValueRef value, LLVMTypeRef type, uint64_t size)
{
    if (LLVMCountStructElementTypes(type) == 0)
        return value;
    LLVMTypeRef first_type = LLVMStructGetTypeAtIndex(type, 0);
    uint64_t struct_type_size = LLVMABISizeOfType(get_llvm_data_layout(), type);
    uint64_t first_type_size = LLVMABISizeOfType(get_llvm_data_layout(), first_type);
    if (first_type_size < size && first_type_size < struct_type_size)
        return value;
    //GEP to first element
    value = LLVMBuildStructGEP2(builder, type, value, 0, "coerce.dive");
    first_type = LLVMTypeOf(value);
    if (LLVMGetTypeKind(first_type) == LLVMStructTypeKind) {
        return _access_bytes_struct(builder, value, first_type, size);
    }
    return value;
}

void _create_aggregate_store(LLVMBuilderRef builder, LLVMValueRef value, LLVMValueRef store, unsigned align)
{
    LLVMTypeRef value_type = LLVMTypeOf(value);
    LLVMTypeKind value_kind = LLVMGetTypeKind(value_type);
    if (value_kind == LLVMStructTypeKind) {
        for (unsigned i = 0; i < LLVMCountStructElementTypes(value_type); ++i) {
            LLVMValueRef ele = LLVMBuildExtractValue(builder, value, i, "");
            LLVMValueRef ele_to = LLVMBuildStructGEP2(builder, LLVMTypeOf(store), store, i, "");
            LLVMBuildStore(builder, ele, ele_to);
        }
    } else {
        LLVMValueRef store_inst = LLVMBuildStore(builder, value, store);
        LLVMSetAlignment(store_inst, align);
    }
}

void create_coerced_store(LLVMBuilderRef builder, LLVMValueRef src, LLVMValueRef dst, unsigned align)
{
    LLVMTypeRef src_type = LLVMTypeOf(src);
    LLVMTypeRef dst_type = LLVMGetElementType(LLVMTypeOf(dst));
    LLVMTypeKind src_kind = LLVMGetTypeKind(src_type);
    LLVMTypeKind dst_kind = LLVMGetTypeKind(dst_type);
    LLVMTargetDataRef td = get_llvm_data_layout();
    if (src_type == dst_type) {
        LLVMBuildStore(builder, src, dst);
        return;
    }
    uint64_t src_size = LLVMABISizeOfType(td, src_type);
    if (dst_kind == LLVMStructTypeKind) {
        dst = _access_bytes_struct(builder, dst, dst_type, src_size);
        dst_type = LLVMGetElementType(LLVMTypeOf(dst));
        dst_kind = LLVMGetTypeKind(dst_type);
    }
    ///TODO: address space casting from src to dst pointer
    //if (src_kind == LLVMPointerTypeKind && dst_kind == LLVMPointerTypeKind)
    uint64_t dst_size = LLVMABISizeOfType(td, dst_type);
    if (src_size <= dst_size) {
        //bit cast the src pointer
        LLVMTypeRef src_ptr_type = LLVMPointerType(src_type, 0);
        dst = LLVMBuildBitCast(builder, dst, src_ptr_type, "");
        _create_aggregate_store(builder, src, dst, align);
    }
}

void add_fun_param_attribute(LLVMContextRef context, LLVMValueRef fun, unsigned arg_index, const char *attr)
{
    unsigned attr_kind = LLVMGetEnumAttributeKindForName(attr, strlen(attr));
    LLVMAddAttributeAtIndex(fun, arg_index + 1, LLVMCreateEnumAttribute(context, attr_kind, 0));
}

void add_fun_param_type_attribute(LLVMContextRef context, LLVMValueRef fun, unsigned arg_index, const char *attr, LLVMTypeRef type)
{
    unsigned attr_kind = LLVMGetEnumAttributeKindForName(attr, strlen(attr));
    LLVMAddAttributeAtIndex(fun, arg_index + 1, LLVMCreateTypeAttribute(context, attr_kind, type));
}