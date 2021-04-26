/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation for function calls
 */
#include "codegen/cg_call.h"
#include "codegen/cg_fun.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/ir_api.h"
#include "codegen/ir_arg_info.h"
#include "codegen/type_size_info.h"
#include <assert.h>

LLVMValueRef emit_call_node(struct code_generator *cg, struct exp_node *node)
{
    struct call_node *call = (struct call_node *)node;
    assert(call->callee_decl);
    symbol callee_name = get_callee(call);
    struct fun_info *fi = get_fun_info(call->callee_decl);
    assert(fi);
    LLVMValueRef callee = get_llvm_function(cg, callee_name);
    assert(callee);
    unsigned arg_count = array_size(&call->args);
    LLVMValueRef *arg_values = malloc(arg_count * sizeof(LLVMValueRef));
    for (size_t i = 0; i < arg_count; ++i) {
        struct ast_abi_arg *aaa = (struct ast_abi_arg *)array_get(&fi->args, i);
        struct ir_arg_range *iar = (struct ir_arg_range *)array_get(&fi->iai.args, i);
        struct exp_node *arg = *(struct exp_node **)array_get(&call->args, i);
        struct type_size_info tsi = get_type_size_info(arg->type);
        LLVMValueRef arg_value = emit_ir_code(cg, arg);
        switch (aaa->info.kind) {
        case AK_DIRECT: {
            if (!aaa->info.type)
                break; //FIXIME: is_variadic arg, no available ir arg
            LLVMTypeKind tk = LLVMGetTypeKind(aaa->info.type);
            if (tk != LLVMStructTypeKind && aaa->info.type == get_llvm_type(aaa->type) && aaa->info.direct_offset == 0) {
                break;
            }
            if (tk == LLVMStructTypeKind && aaa->info.kind == AK_DIRECT && aaa->info.can_be_flattened) {

            } else {
                assert(iar->ir_arg_num == 1);
                arg_value = create_coerced_load(cg->builder, arg_value, aaa->info.type, tsi.align_bits / 8);
            }
            break;
        }
        }
        arg_values[i] = arg_value;
    }
    LLVMValueRef value = LLVMBuildCall(cg->builder, callee, arg_values, array_size(&call->args), "");
    LLVMTypeRef sig_ret_type = get_llvm_type(fi->ret.type);
    struct prototype_node *parent_proto = node->parent;
    assert(parent_proto->base.node_type == PROTOTYPE_NODE);
    LLVMValueRef parent_fun = get_llvm_function(cg, parent_proto->name);
    if (sig_ret_type != fi->ret.info.type) {
        struct type_size_info tsi = get_type_size_info(fi->ret.type);
        //create temp memory
        LLVMValueRef alloca = create_alloca(sig_ret_type, tsi.align_bits / 8, parent_fun, "");
        create_coerced_store(cg->builder,  value, alloca, tsi.align_bits / 8);
        value = alloca;
    }
    free(arg_values);
    return value;
}
