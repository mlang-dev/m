#include "codegen/abi_arg_info.h"
#include "codegen/llvm/cg_llvm.h"
#include "codegen/fun_info.h"
#include "sema/type_size_info.h"
#include <assert.h>
#include <llvm-c/Core.h>

enum Class {
    INTEGER, //This class consists of integral types that fit into one of the general purpose registers.
    SSE, // The class consists of types that fit into a vector register.
    SSEUP, //The class consists of types that fit into a vector register and can be passed and returned in the upper bytes of it.
    X87, //These classes consists of types that will be returned via the x87 FPU.
    X87UP,
    COMPLEX_X87, //This class consists of types that will be returned via the x87 FPU.
    NO_CLASS, //This class is used as initializer in the algorithms. It will be used for padding and empty structures and unions.
    MEMORY //This class consists of types that will be passed and returned in memory via the stack.
};

void _post_merge(struct target_info *ti, uint64_t aggregate_size, enum Class *low, enum Class *high)
{
    if (*high == MEMORY)
        *low = MEMORY;
    if (*high == X87UP && *low != X87 && ti->os == OS_MACOSX)
        *low = MEMORY;
    if (aggregate_size > 128 && (*low != SSE || *high != SSEUP))
        *low = MEMORY;
    if (*high == SSEUP && *low != SSE)
        *high = SSE;
}

enum Class _merge(enum Class accum, enum Class field)
{
    if (accum == field || field == NO_CLASS)
        return accum;
    if (field == MEMORY)
        return MEMORY;
    if (accum == NO_CLASS)
        return field;
    if (field == X87 || field == X87UP || field == COMPLEX_X87 || accum == X87 || accum == X87UP)
        return MEMORY;
    return SSE;
}

void _classify(struct codegen *cg, struct type_item *te, uint64_t offset_base, enum Class *low, enum Class *high)
{
    enum Class *current;
    *low = *high = NO_CLASS;
    current = offset_base < 64 ? low : high;
    *current = MEMORY;
    if (te->type < TYPE_STRUCT) {
        //builtin types
        if (te->type == TYPE_UNIT) {
            *current = NO_CLASS;
        } else if (is_int_type(te->type) || te->type == TYPE_STRING) {
            *current = INTEGER;
        } else if (te->type == TYPE_F64) {
            *current = SSE;
        } //TODO: LONG_DOUBLE
        return;
    } /*else if pointer then make current pointing to INTEGER*/
    //TODO: vector, complex, int type with specified bitwidth, constant array
    else if (te->type == TYPE_STRUCT) {
        struct type_size_info tsi = get_type_size_info(cg->sema_context->tc, te);
        uint64_t size = tsi.width_bits;
        if (size > 512)
            return;
        struct struct_layout *sl = layout_struct(cg->sema_context->tc, te, Product);
        *current = NO_CLASS;
        for (size_t i = 0; i < array_size(&sl->field_offsets); i++) {
            uint64_t offset = offset_base + *(uint64_t *)array_get(&sl->field_offsets, i);
            struct type_item *field_type = array_get_ptr(&te->args, i);
            uint64_t field_type_size = get_type_size(cg->sema_context->tc, field_type);
            assert(field_type_size);
            if (size > 128 && size != field_type_size) {
                *low = MEMORY;
                _post_merge(cg->target_info, size, low, high);
                return;
            }
            if (offset % field_type_size) {
                *low = MEMORY;
                _post_merge(cg->target_info, size, low, high);
                return;
            }
            enum Class field_low, field_high;
            _classify(cg, field_type, offset, &field_low, &field_high);
            *low = _merge(*low, field_low);
            *high = _merge(*high, field_high);
            if (*low == MEMORY || *high == MEMORY)
                break;
        }
        sl_free(sl);
        _post_merge(cg->target_info, size, low, high);
    }
}

bool _bits_contain_no_user_data(struct type_context *tc, struct type_item *type, unsigned start_bit, unsigned end_bit)
{
    uint64_t type_size_bits = get_type_size(tc, type);
    if (type_size_bits <= start_bit)
        return true;

    //TODO: for array type
    //struct type
    if (type->type == TYPE_STRUCT) {
        struct struct_layout *sl = layout_struct(tc, type, Product);
        for (unsigned i = 0; i < array_size(&type->args); i++) {
            unsigned field_offset = (unsigned)*(uint64_t *)array_get(&sl->field_offsets, i);
            if (field_offset >= end_bit)
                break;
            unsigned field_start = field_offset < start_bit ? start_bit - field_offset : 0;
            if (_bits_contain_no_user_data(tc, array_get(&type->args, i), field_start, end_bit - field_offset)){
                sl_free(sl);
                return false;
            }
        }
        sl_free(sl);
        return true;
    }
    return false;
}

bool _contains_float_at_offset(struct cg_llvm *cg, LLVMTypeRef ir_type, unsigned ir_offset)
{
    LLVMTargetDataRef dl = LLVMGetModuleDataLayout(cg->module);
    LLVMTypeKind tk = LLVMGetTypeKind(ir_type);
    if (ir_offset == 0 && tk == LLVMFloatTypeKind)
        return true;
    if (tk == LLVMStructTypeKind) {
        unsigned field_index = LLVMElementAtOffset(dl, ir_type, ir_offset);
        ir_offset -= (unsigned) LLVMOffsetOfElement(dl, ir_type, field_index);
        return _contains_float_at_offset(cg, LLVMStructGetTypeAtIndex(ir_type, field_index), ir_offset);
    }
    if (tk == LLVMArrayTypeKind) {
        LLVMTypeRef element_type = LLVMGetElementType(ir_type);
        uint64_t element_size = LLVMABISizeOfType(dl, element_type);
        ir_offset -= (unsigned)(ir_offset / element_size * element_size);
        return _contains_float_at_offset(cg, element_type, ir_offset);
    }
    return false;
}

LLVMTypeRef _get_sse_type_at_offset(struct cg_llvm *cg, LLVMTypeRef ir_type, unsigned ir_offset, struct type_item *source_type, unsigned source_offset)
{
    struct type_context *tc = cg->base.sema_context->tc;
    if (_bits_contain_no_user_data(tc, source_type, source_offset * 8 + 32, source_offset * 8 + 64))
        return LLVMFloatTypeInContext(cg->context);

    if (_contains_float_at_offset(cg, ir_type, ir_offset) && _contains_float_at_offset(cg, ir_type, ir_offset + 4))
        return LLVMVectorType(LLVMFloatTypeInContext(cg->context), 2);
    return LLVMDoubleTypeInContext(cg->context);
}

LLVMTypeRef _get_int_type_at_offset(struct cg_llvm *cg, LLVMTypeRef ir_type, unsigned ir_offset, struct type_item *source_type, unsigned source_offset)
{
    struct type_context *tc = cg->base.sema_context->tc;
    LLVMTargetDataRef dl = LLVMGetModuleDataLayout(cg->module);
    LLVMTypeKind tk = LLVMGetTypeKind(ir_type);
    if (ir_offset == 0) {
        unsigned int_width = tk == LLVMIntegerTypeKind ? LLVMGetIntTypeWidth(ir_type) : 0;
        if ((tk == LLVMPointerTypeKind && LLVMPointerSize(dl) == 8)
            || int_width == 64) {
            return ir_type;
        }

        if (int_width == 8 || int_width == 16 || int_width == 32 || (tk == LLVMPointerTypeKind && LLVMPointerSize(dl) != 8)) {
            unsigned bit_width = tk == LLVMPointerTypeKind ? 32 : int_width;
            if (_bits_contain_no_user_data(tc, source_type, source_offset * 8 + bit_width, source_offset * 8 + 64))
                return ir_type;
        }
    }
    if (tk == LLVMStructTypeKind) {
        if (ir_offset < LLVMSizeOfTypeInBits(dl, ir_type) / 8) {
            unsigned field_index = LLVMElementAtOffset(dl, ir_type, ir_offset);
            ir_offset -= (unsigned)LLVMOffsetOfElement(dl, ir_type, field_index);
            return _get_int_type_at_offset(cg, LLVMStructGetTypeAtIndex(ir_type, field_index), ir_offset, source_type, source_offset);
        }
    }
    //TODO: array type

    //
    uint64_t type_size_bytes = get_type_size(tc, source_type) / 8;
    uint64_t bit_width = ((type_size_bytes - source_offset) < 8 ? type_size_bytes - source_offset : 8) * 8;
    return LLVMIntTypeInContext(cg->context, (unsigned)bit_width);
}

///given a high and low type that can ideally be used as two register pair to pass or return
///to return a first class aggregate to represent them, e.g. {i32*, float} as {low, high}
LLVMTypeRef _get_x86_64_byval_arg_pair(struct cg_llvm *cg, LLVMTypeRef low, LLVMTypeRef high, LLVMTargetDataRef dl)
{
    uint64_t low_size = LLVMABISizeOfType(dl, low);
    unsigned high_align = LLVMABIAlignmentOfType(dl, high);
    uint64_t high_start = align_to(low_size, high_align);
    if (high_start != 8) {
        if (LLVMGetTypeKind(low) == LLVMFloatTypeKind)
            low = LLVMDoubleTypeInContext(cg->context);
        else {
            low = LLVMInt64TypeInContext(cg->context);
        }
    }
    LLVMTypeRef pair[2];
    pair[0] = low;
    pair[1] = high;
    return LLVMStructType(pair, 2, false);
}

struct abi_arg_info _classify_return_type(struct cg_llvm *cg, struct type_item *ret_type)
{
    enum Class low, high;
    struct target_info *ti = cg->base.target_info;
    _classify(&cg->base, ret_type, 0, &low, &high);
    LLVMTypeRef result_type = 0;
    LLVMTypeRef complex[2];
    switch (low) {
    case NO_CLASS:
        if (high == NO_CLASS)
            return create_ignore(ret_type);
        break;
    case SSEUP:
    case X87UP:
        assert(false);
    case MEMORY:
        return create_indirect_return_result(&cg->base, ret_type);
    case INTEGER:
        result_type = _get_int_type_at_offset(cg, get_backend_type(cg, ret_type), 0, ret_type, 0);
        if (high == NO_CLASS && LLVMGetTypeKind(result_type) == LLVMIntegerTypeKind) {
            if (is_promotable_int(ret_type))
                return create_extend(ti, ret_type);
        }
        break;
    // AMD64-ABI 3.2.3p4: Rule 4. If the class is SSE, the next
    // available SSE register of the sequence %xmm0, %xmm1 is used.
    case SSE:
        result_type = _get_sse_type_at_offset(cg, get_backend_type(cg, ret_type), 0, ret_type, 0);
        break;
    // AMD64-ABI 3.2.3p4: Rule 6. If the class is X87, the value is
    // returned on the X87 stack in %st0 as 80-bit x87 number.
    case X87:
        result_type = LLVMX86FP80TypeInContext(cg->context);
        break;

    // AMD64-ABI 3.2.3p4: Rule 8. If the class is COMPLEX_X87, the real
    // part of the value is returned in %st0 and the imaginary part in
    // %st1.
    case COMPLEX_X87:
        complex[0] = LLVMX86FP80TypeInContext(cg->context);
        complex[1] = LLVMX86FP80TypeInContext(cg->context);
        result_type = LLVMStructType(complex, 2, false);
        break;
    }

    LLVMTypeRef high_part = 0;
    switch (high) {
    case MEMORY:
    case X87:
        assert(false);
    case COMPLEX_X87:
    case NO_CLASS:
        break;
    case INTEGER:
        high_part = _get_int_type_at_offset(cg, get_backend_type(cg, ret_type), 8, ret_type, 8);
        if (low == NO_CLASS)
            return create_direct_type_offset(ret_type, high_part, 8);
        break;
    case SSE:
        high_part = _get_sse_type_at_offset(cg, get_backend_type(cg, ret_type), 8, ret_type, 8);
        if (low == NO_CLASS)
            return create_direct_type_offset(ret_type, high_part, 8);
        break;
    case SSEUP:
        //TODO: vector type
        break;
    case X87UP:
        if (low != X87) {
            high_part = _get_sse_type_at_offset(cg, get_backend_type(cg, ret_type), 8, ret_type, 8);
            if (low == NO_CLASS)
                return create_direct_type_offset(ret_type, high_part, 8);
        }
        break;
    }

    if (high_part){
        LLVMTargetDataRef dl = LLVMGetModuleDataLayout(cg->module);
        result_type = _get_x86_64_byval_arg_pair(cg, result_type, high_part, dl);
    }
    return create_direct_type(ret_type, result_type);
}

struct abi_arg_info _classify_argument_type(struct cg_llvm *cg, struct type_item *type, unsigned free_int_regs, unsigned *needed_int, unsigned *needed_sse)
{
    //struct abi_arg_info aa;
    struct target_info *ti = cg->base.target_info;
    LLVMTargetDataRef dl = LLVMGetModuleDataLayout(cg->module);

    enum Class low, high;
    _classify(&cg->base, type, 0, &low, &high);
    *needed_int = 0;
    *needed_sse = 0;
    LLVMTypeRef result_type = 0;
    switch (low) {
    case NO_CLASS:
        if (high == NO_CLASS)
            return create_ignore(type);
        break;
    case MEMORY:
    case X87:
    case COMPLEX_X87:
        return create_indirect_result(&cg->base, type, free_int_regs);
    case SSEUP:
    case X87UP:
        assert(false);
    case INTEGER:
        ++(*needed_int);
        result_type = _get_int_type_at_offset(cg, get_backend_type(cg, type), 0, type, 0);
        if (high == NO_CLASS && LLVMGetTypeKind(result_type) == LLVMIntegerTypeKind) {
            if (is_promotable_int(type))
                return create_extend(ti, type);
        }
        break;
    case SSE: {
        LLVMTypeRef ir_type = get_backend_type(cg, type);
        result_type = _get_sse_type_at_offset(cg, ir_type, 0, type, 0);
        ++(*needed_sse);
    }
    }

    LLVMTypeRef high_part = 0;
    switch (high) {
    case MEMORY:
    case X87:
    case COMPLEX_X87:
        assert(false);
    case NO_CLASS:
        break;
    case INTEGER:
        ++(*needed_int);
        high_part = _get_int_type_at_offset(cg, get_backend_type(cg, type), 8, type, 8);
        if (low == NO_CLASS)
            return create_direct_type_offset(type, high_part, 8);
        break;
    case X87UP:
    case SSE:
        high_part = _get_sse_type_at_offset(cg, get_backend_type(cg, type), 8, type, 8);
        if (low == NO_CLASS)
            return create_direct_type_offset(type, high_part, 8);
        ++(*needed_sse);
        break;
    case SSEUP:
        //TODO: vector type
        break;
    }
    if (high_part)
        result_type = _get_x86_64_byval_arg_pair(cg, result_type, high_part, dl);

    return create_direct_type(type, result_type);
}

///compute abi info
void x86_64_compute_fun_info(struct cg_llvm *cg, struct fun_info *fi)
{
    unsigned free_int_regs = 6;
    unsigned free_sse_regs = 8;
    unsigned needed_int, needed_sse;
    fi->ret = _classify_return_type(cg, fi->ret.type);
    if (fi->ret.kind == AK_INDIRECT)
        --free_int_regs;
    if (fi->is_chain_call)
        ++free_int_regs;
    //unsigned required_args = fi->required_args;    
    for (unsigned arg_no = 0; arg_no < array_size(&fi->args); arg_no++) {
        //bool is_named_arg = arg_no < required_args;
        struct abi_arg_info *aai = array_get(&fi->args, arg_no);
        *aai = _classify_argument_type(cg, aai->type, free_int_regs, &needed_int, &needed_sse);

        if (free_int_regs >= needed_int && free_sse_regs >= needed_sse) {
            free_int_regs -= needed_int;
            free_sse_regs -= needed_sse;
        } else {
            *aai = create_indirect_result(&cg->base, aai->type, free_int_regs);
        }
    }
}
