#include "codegen/abi_arg_info.h"

struct abi_arg_info create_direct(LLVMTypeRef type, unsigned direct_offset, LLVMTypeRef padding_type, bool can_be_flattened)
{
    struct abi_arg_info aai;
    aai.kind = Direct;
    aai.type = type;
    aai.padding_type = padding_type;
    aai.direct_offset = direct_offset;
    aai.can_be_flattened = can_be_flattened;
    return aai;
}

struct abi_arg_info create_indirect(unsigned indirect_align, bool indirect_byval, bool indirect_realign, LLVMTypeRef padding_type)
{
    struct abi_arg_info aai;
    aai.kind = Indirect;
    aai.padding_type = padding_type;
    aai.indirect_byval = indirect_byval;
    aai.indirect_align = indirect_align;
    aai.indirect_realign = indirect_realign;
    return aai;
}

struct abi_arg_info create_expand(bool padding_inreg, LLVMTypeRef padding_type)
{
    struct abi_arg_info aai;
    aai.kind = Expand;
    aai.padding_type = padding_type;
    aai.padding_inreg = padding_inreg;
    return aai;
}