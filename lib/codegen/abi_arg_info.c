#include "codegen/abi_arg_info.h"
#include "codegen/codegen.h"
#include "sema/type_size_info.h"
#include <assert.h>

struct abi_arg_info _create_direct(struct type_item *type, TargetType target_type, unsigned direct_offset, TargetType padding_type, bool can_be_flattened)
{
    struct abi_arg_info aai;
    aai.type = type;
    aai.kind = AK_DIRECT;
    aai.target_type = target_type;
    aai.padding.padding_type = padding_type;
    aai.align.direct_offset = direct_offset;
    aai.can_be_flattened = can_be_flattened;
    return aai;
}

struct abi_arg_info _create_indirect(struct type_item *type, unsigned indirect_align, bool indirect_byval, bool indirect_realign, TargetType padding_type)
{
    struct abi_arg_info aai;
    aai.type = type;
    aai.kind = AK_INDIRECT;
    aai.target_type = 0;
    aai.padding.padding_type = padding_type;
    aai.indirect_byval = indirect_byval;
    aai.align.indirect_align = indirect_align;
    aai.indirect_realign = indirect_realign;
    return aai;
}

struct abi_arg_info create_expand(struct type_item *type, bool padding_inreg, TargetType padding_type)
{
    struct abi_arg_info aai;
    aai.type = type;
    aai.kind = AK_DIRECT;
    aai.target_type = 0;
    aai.padding.padding_type = padding_type;
    aai.padding_inreg = padding_inreg;
    return aai;
}

struct abi_arg_info create_ignore(struct type_item *type)
{
    struct abi_arg_info aai;
    aai.type = type;
    aai.kind = AK_IGNORE;
    aai.target_type = 0;
    aai.padding.padding_type = 0;
    aai.align.direct_offset = 0;
    aai.can_be_flattened = false;
    return aai;
}

struct abi_arg_info create_extend(struct target_info *ti, struct type_item *ret_type)
{
    (void)ret_type;
    struct abi_arg_info aai;
    aai.type = ret_type;
    aai.kind = AK_EXTEND;
    aai.target_type = ti->extend_type;
    aai.padding.padding_type = 0;
    aai.align.direct_offset = 0;
    aai.sign_ext = true;
    return aai;
}

struct abi_arg_info create_natural_align_indirect(struct type_context *tc, struct type_item *ret_type, bool indirect_byval)
{
    uint64_t align_bytes = get_type_align(tc, ret_type);
    return _create_indirect(ret_type, (unsigned)align_bytes, indirect_byval, false, 0);
}

struct abi_arg_info create_indirect_return_result(struct codegen *cg, struct type_item *ret_type)
{
    struct type_context *tc = cg->sema_context->tc;
    if (ret_type->type < TYPE_STRUCT) {
        if (is_promotable_int(ret_type))
            return create_extend(cg->target_info, ret_type);
        else
            return create_direct(ret_type);
    }
    return create_natural_align_indirect(tc, ret_type, false);
}

struct abi_arg_info create_indirect_result(struct codegen *cg, struct type_item *type, unsigned free_int_regs)
{
    struct target_info *ti = cg->target_info;
    struct type_context *tc = cg->sema_context->tc;
    if (type->type < TYPE_STRUCT) {
        if (is_promotable_int(type))
            return create_extend(ti, type);
        else
            return create_direct(type);
    }
    unsigned align_bytes = (unsigned)get_type_align(tc, type);
    if (align_bytes < 8)
        align_bytes = 8;
    if (free_int_regs == 0) {
        uint64_t size = get_type_size(tc, type);
        if (align_bytes == 8 && size <= 64){
            return _create_direct(type, ti->get_size_int_type(cg, (unsigned)size), 0, 0, true);
        }
    }
    return _create_indirect(type, align_bytes, true, false, 0);
}

struct abi_arg_info create_direct(struct type_item *type)
{
    return _create_direct(type, 0, 0, 0, true);
}

struct abi_arg_info create_direct_type_offset(struct type_item *type, TargetType target_type, unsigned offset)
{
    return _create_direct(type, target_type, offset, 0, true);
}

struct abi_arg_info create_direct_type(struct type_item *type, TargetType target_type)
{
    return _create_direct(type, target_type, 0, 0, true);
}

bool can_have_padding_type(struct abi_arg_info *aai)
{
    return aai->kind == AK_DIRECT || aai->kind == AK_EXTEND || aai->kind == AK_INDIRECT || aai->kind == AK_INDIRECT_ALIASED;
}

TargetType get_padding_type(struct abi_arg_info *aai)
{
    return can_have_padding_type(aai) ? aai->padding.padding_type : 0;
}

void get_coerce_and_expand_types(struct abi_arg_info *aai, TargetType *types)
{
    assert(aai->kind == AK_COERCE_AND_EXPAND);
    /*
    if (LLVMGetTypeKind(aai->padding.coerce_and_expand_type) == LLVMStructTypeKind)
        LLVMGetStructElementTypes(aai->padding.coerce_and_expand_type, types);
    else
        types[0] = aai->padding.coerce_and_expand_type;
    */
}

bool can_have_coerce_to_type(struct abi_arg_info *aai)
{
    return aai->kind == AK_DIRECT || aai->kind == AK_EXTEND || aai->kind == AK_COERCE_AND_EXPAND;
}
