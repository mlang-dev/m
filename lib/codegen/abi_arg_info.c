#include "codegen/abi_arg_info.h"
#include "codegen/codegen.h"
#include "codegen/type_size_info.h"
#include <assert.h>

struct abi_arg_info _create_direct(LLVMTypeRef type, unsigned direct_offset, LLVMTypeRef padding_type, bool can_be_flattened)
{
    struct abi_arg_info aai;
    aai.kind = AK_DIRECT;
    aai.type = type;
    aai.padding_type = padding_type;
    aai.direct_offset = direct_offset;
    aai.can_be_flattened = can_be_flattened;
    return aai;
}

struct abi_arg_info _create_indirect(unsigned indirect_align, bool indirect_byval, bool indirect_realign, LLVMTypeRef padding_type)
{
    struct abi_arg_info aai;
    aai.kind = AK_INDIRECT;
    aai.padding_type = padding_type;
    aai.indirect_byval = indirect_byval;
    aai.indirect_align = indirect_align;
    aai.indirect_realign = indirect_realign;
    return aai;
}

struct abi_arg_info create_expand(bool padding_inreg, LLVMTypeRef padding_type)
{
    struct abi_arg_info aai;
    aai.kind = AK_DIRECT;
    aai.padding_type = padding_type;
    aai.padding_inreg = padding_inreg;
    return aai;
}

struct abi_arg_info create_ignore()
{
    struct abi_arg_info aai;
    aai.kind = AK_IGNORE;
    aai.type = 0;
    aai.padding_type = 0;
    aai.direct_offset = 0;
    aai.can_be_flattened = false;
    return aai;
}

struct abi_arg_info create_extend(struct type_exp *ret_type, LLVMTypeRef llvm_type)
{
    struct abi_arg_info aai;
    aai.kind = AK_EXTEND;
    aai.type = llvm_type;
    aai.padding_type = 0;
    aai.direct_offset = 0;
    aai.sign_ext = true;
    return aai;
}

struct abi_arg_info create_natural_align_indirect(struct type_exp *ret_type)
{
    uint64_t align_bytes = get_type_align(ret_type) / 8;
    return _create_indirect(align_bytes, true, false, 0);
}

struct abi_arg_info create_indirect_return_result(struct type_exp *ret_type)
{
    if (ret_type->type < TYPE_EXT) {
        if (is_promotable_int(ret_type))
            return create_extend(ret_type, 0);
        else
            return create_direct();
    }
    return create_natural_align_indirect(ret_type);
}

struct abi_arg_info create_indirect_result(struct type_exp *type, unsigned free_int_regs)
{
    if (type->type < TYPE_EXT) {
        if (is_promotable_int(type))
            return create_extend(type, 0);
        else
            return create_direct();
    }
    unsigned align_bytes = get_type_align(type) / 8;
    if (align_bytes < 8)
        align_bytes = 8;
    if (free_int_regs == 0) {
        uint64_t size = get_type_size(type);
        if (align_bytes == 8 && size <= 64)
            return _create_direct(LLVMIntTypeInContext(get_llvm_context(), size), 0, 0, true);
    }
    return _create_indirect(align_bytes, true, false, 0);
}

struct abi_arg_info create_direct()
{
    return _create_direct(0, 0, 0, true);
}

struct abi_arg_info create_direct_type_offset(LLVMTypeRef type, unsigned offset)
{
    return _create_direct(type, offset, 0, true);
}

struct abi_arg_info create_direct_type(LLVMTypeRef type)
{
    return _create_direct(type, 0, 0, true);
}

bool can_have_padding_type(struct abi_arg_info *aai)
{
    return aai->kind == AK_DIRECT || aai->kind == AK_EXTEND || aai->kind == AK_INDIRECT || aai->kind == AK_INDIRECT_ALIASED;
}

LLVMTypeRef get_padding_type(struct abi_arg_info *aai)
{
    return can_have_padding_type(aai) ? aai->padding_type : 0;
}

void get_coerce_and_expand_types(struct abi_arg_info *aai, LLVMTypeRef *types)
{
    assert(aai->kind == AK_COERCE_AND_EXPAND);
    if (LLVMGetTypeKind(aai->coerce_and_expand_type) == LLVMStructTypeKind)
        LLVMGetStructElementTypes(aai->coerce_and_expand_type, types);
    else
        types[0] = aai->coerce_and_expand_type;
}