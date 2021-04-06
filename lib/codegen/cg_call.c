#include "codegen/cg_call.h"
#include "codegen/fun_info.h"

struct address zero_address()
{
    struct address adr;
    adr.pointer = 0;
    adr.alignment = 0;
    return adr;
}

void _map_to_ir_arg_info(struct fun_info *fi, struct ir_arg_info *iai)
{
    unsigned ir_arg_no = 0;
    if (fi->ret.info.kind == AK_INDIRECT)
        iai->sret_arg_no = ir_arg_no++;

    unsigned arg_no = 0;
    unsigned arg_num = array_size(&fi->args);
    for (unsigned i = 0; i < arg_num; i++) {
        struct ast_abi_arg *aa = (struct ast_abi_arg *)array_get(&fi->args, i);
        iai->args;
    }
}

void emit_call(struct call_node *call)
{
    struct fun_info *fi = get_fun_info(get_callee(call), call->base.type);
    struct ir_arg_info iai;
    iai.sret_arg_no = InvalidIndex;
    iai.total_ir_args = 0;
}