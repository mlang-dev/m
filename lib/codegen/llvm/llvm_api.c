#include "codegen/llvm/llvm_api.h"
#include "codegen/llvm/cg_llvm.h"
#include "sema/type.h"
#include <assert.h>
#include <llvm-c/Target.h>

struct address create_temp_mem(struct type_expr *to, unsigned align, symbol name)
{
    (void)to;
    (void)align;
    (void)name;
    struct address adr;
    adr.alignment = 0;
    adr.pointer = 0;
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
    // GEP to first element
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

bool _is_int_or_ptr(LLVMTypeKind tk)
{
    return tk == LLVMIntegerTypeKind || tk == LLVMPointerTypeKind;
}

/// TODO: fix this later
LLVMValueRef coerce_int_or_ptr(LLVMValueRef value, LLVMTypeRef dst_type)
{
    (void)value;
    (void)dst_type;
    assert(false);
    return 0;
}

/// store src value to dst location (dst pointer)
/// when dst pointer pointing to different type than source type, we need to convert
/// dst pointer type to pointer to source type, then load source value into the
/// converted dst pointer
void create_coerced_store(LLVMBuilderRef builder, LLVMValueRef src, LLVMValueRef dst, unsigned align)
{
    LLVMTypeRef src_type = LLVMTypeOf(src);
    LLVMTypeRef dst_type = LLVMGetElementType(LLVMTypeOf(dst));
    //LLVMTypeKind src_kind = LLVMGetTypeKind(src_type);
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
    /// TODO: address space casting from src to dst pointer
    // if (src_kind == LLVMPointerTypeKind && dst_kind == LLVMPointerTypeKind)
    uint64_t dst_size = LLVMABISizeOfType(td, dst_type);
    if (src_size <= dst_size) {
        // bit cast the src pointer
        LLVMTypeRef src_ptr_type = LLVMPointerType(src_type, 0);
        dst = LLVMBuildBitCast(builder, dst, src_ptr_type, "");
        _create_aggregate_store(builder, src, dst, align);
    }
}

LLVMValueRef create_coerced_load(LLVMBuilderRef builder, LLVMValueRef src, LLVMTypeRef dst_type, unsigned align)
{
    LLVMTypeRef src_type = LLVMGetElementType(LLVMTypeOf(src));
    LLVMValueRef load = 0;
    if (src_type == dst_type) {
        load = LLVMBuildLoad2(builder, src_type, src, "");
        LLVMSetAlignment(load, align);
        return load;
    }
    LLVMTypeKind src_type_kind = LLVMGetTypeKind(src_type);
    LLVMTypeKind dst_type_kind = LLVMGetTypeKind(dst_type);
    LLVMTargetDataRef td = get_llvm_data_layout();
    uint64_t dst_size = LLVMABISizeOfType(td, dst_type);
    if (src_type_kind == LLVMStructTypeKind) {
        src = _access_bytes_struct(builder, src, src_type, dst_size);
        src_type = LLVMGetElementType(LLVMTypeOf(src));
        src_type_kind = LLVMGetTypeKind(src_type);
    }
    uint64_t src_size = LLVMABISizeOfType(td, src_type);
    // for int, or pointer type do extension or truncation
    if (_is_int_or_ptr(dst_type_kind) && _is_int_or_ptr(src_type_kind)) {
        load = LLVMBuildLoad2(builder, src_type, src, "");
        return coerce_int_or_ptr(load, dst_type);
    }
    if (src_size >= dst_size) {
        LLVMTypeRef dst_ptr_type = LLVMPointerType(dst_type, 0);
        src = LLVMBuildBitCast(builder, src, dst_ptr_type, "");
        load = LLVMBuildLoad2(builder, dst_type, src, "");
        LLVMSetAlignment(load, align);
        return load;
    }

    // TODO: more
    assert(false);
    return 0;
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

void add_call_arg_type_attribute(LLVMContextRef context, LLVMValueRef call, unsigned arg_index, const char *attr, LLVMTypeRef type)
{
    unsigned attr_kind = LLVMGetEnumAttributeKindForName(attr, strlen(attr));
    LLVMAddCallSiteAttribute(call, arg_index + 1, LLVMCreateTypeAttribute(context, attr_kind, type));
}
