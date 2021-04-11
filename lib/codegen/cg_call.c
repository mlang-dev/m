#include "codegen/cg_call.h"
#include "codegen/fun_info.h"
#include "codegen/ir_arg_info.h"

struct address zero_address()
{
    struct address adr;
    adr.pointer = 0;
    adr.alignment = 0;
    return adr;
}

void emit_call(struct call_node *call)
{
    struct fun_info *fi = get_fun_info(call->callee_decl);
    LLVMValueRef ir_fun_type = get_fun_type(fi);
    struct ir_arg_info iai;
    ir_arg_info_init(&iai);
    map_to_ir_arg_info(fi, &iai);
}