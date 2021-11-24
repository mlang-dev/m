#include "clib/util.h"
#include "codegen/abi_arg_info.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"

struct abi_arg_info _winx86_64_classify(struct type_exp *te, unsigned *free_sse_regs, bool is_return_type, bool is_vector_call, bool is_reg_call)
{
    (void)free_sse_regs;
    (void)is_vector_call;
    (void)is_reg_call;
    if (te->type == TYPE_UNIT)
        return create_ignore();
    struct type_size_info tsi = get_type_size_info(te);
    unsigned width = (unsigned)tsi.width_bits;
    //uint64_t align = tsi.align_bits / 8;
    if (te->type == TYPE_EXT) {
        if (!is_return_type) {
            //has flexiable array member
            //return natural align indirect
        }
        // MS x64 ABI requirement: "Any argument that doesn't fit in 8 bytes, or is
        // not 1, 2, 4, or 8 bytes, must be passed by reference."
        if (width > 64 || !is_power_of2_64(width)) {
            return create_natural_align_indirect(te, false);
        }
        //coerce it into the small integer type
        return create_direct_type(LLVMIntTypeInContext(get_llvm_context(), width));
    } else if (te->type == TYPE_BOOL) {
        return create_extend(te);
    }

    //TODO: Member pointer, complex type, big int etc
    return create_direct();
}
///compute abi info
void winx86_64_compute_fun_info(struct fun_info *fi)
{
    unsigned free_sse_regs = 0;
    fi->ret.info = _winx86_64_classify(fi->ret.type, &free_sse_regs, true, false, false);
    unsigned zero_sse_regs = 0;
    bool is_vector_call = false;
    for (unsigned arg_no = 0; arg_no < array_size(&fi->args); arg_no++) {
        // Vectorcall in x64 only permits the first 6 arguments to be passed as
        // XMM/YMM registers. After the sixth argument, pretend no vector
        // registers are left.
        struct ast_abi_arg *arg = array_get(&fi->args, arg_no);
        unsigned *maybe_free_sse_regs = (is_vector_call && arg_no >= 6) ? &zero_sse_regs : &free_sse_regs;
        arg->info = _winx86_64_classify(arg->type, maybe_free_sse_regs, false, false, false);
    }
}
