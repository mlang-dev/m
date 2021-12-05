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


// get parent func sret parameter if exists
LLVMValueRef _get_parent_call_sret_pointer(struct code_generator *cg, struct exp_node *node)
{
    struct ast_node *parent_ft = find_parent_proto(node);
    struct fun_info *fi = get_fun_info(parent_ft);
    bool has_sret = fi->iai.sret_arg_no != InvalidIndex;
    LLVMValueRef ret = 0;
    if (has_sret && node->is_ret) {
        LLVMValueRef fun = get_llvm_function(cg, parent_ft->ft->name);
        ret = LLVMGetParam(fun, fi->iai.sret_arg_no);
    }
    return ret;
}

LLVMValueRef emit_call_node(struct code_generator *cg, struct exp_node *node)
{
    struct ast_node *call = (struct ast_node *)node;
    assert(call->call->callee_func_type);
    symbol callee_name = get_callee(call);
    struct fun_info *fi = get_fun_info(call->call->callee_func_type);
    assert(fi);
    LLVMValueRef callee = get_llvm_function(cg, callee_name);
    assert(callee);
    size_t arg_count = array_size(&call->call->args);
    bool has_sret = fi->iai.sret_arg_no != InvalidIndex;
    size_t ir_arg_count =  has_sret ? arg_count + 1 : arg_count;
    LLVMValueRef *arg_values;
    MALLOC(arg_values, ir_arg_count * sizeof(LLVMValueRef));
    LLVMTypeRef sig_ret_type = get_llvm_type(fi->ret.type);
    struct ast_node *parent_ft = find_parent_proto(node);
    struct type_size_info ret_tsi = get_type_size_info(fi->ret.type);
    LLVMValueRef ret_alloca = 0;
    LLVMValueRef parent_fun = 0;
    if (parent_ft) { //TODO: JIT call code in global scope, no parent
        parent_fun = get_llvm_function(cg, parent_ft->ft->name);
    }

    if (has_sret) { //the first is return struct
        ret_alloca = _get_parent_call_sret_pointer(cg, node);
        if (!ret_alloca)
            ret_alloca = create_alloca(sig_ret_type, ret_tsi.align_bits / 8, parent_fun, "");
        arg_values[fi->iai.sret_arg_no] = ret_alloca;
    }
    for (size_t i = 0; i < arg_count; ++i) {
        struct ast_abi_arg *aaa = (struct ast_abi_arg *)array_get(&fi->args, i);
        struct ir_arg_range *iar = (struct ir_arg_range *)array_get(&fi->iai.args, i);
        struct exp_node *arg = *(struct exp_node **)array_get(&call->call->args, i);
        struct type_size_info tsi = get_type_size_info(arg->type);
        LLVMValueRef arg_value = emit_ir_code(cg, arg);
        switch (aaa->info.kind) {
        case AK_DIRECT: {
            if (!aaa->info.type)
                break; //FIXIME: is_variadic arg, no available ir arg
            LLVMTypeKind tk = LLVMGetTypeKind(aaa->info.type);
            if (tk != LLVMStructTypeKind && aaa->info.type == get_llvm_type(aaa->type) && aaa->info.align.direct_offset == 0) {
                break;
            }
            if (tk == LLVMStructTypeKind && aaa->info.kind == AK_DIRECT && aaa->info.can_be_flattened) {

            } else {
                assert(iar->ir_arg_num == 1);
                arg_value = create_coerced_load(cg->builder, arg_value, aaa->info.type, tsi.align_bits / 8);
            }
            break;
        }
        case AK_EXTEND:
        case AK_INDIRECT_ALIASED:
        case AK_IGNORE:
        case AK_EXPAND:
        case AK_COERCE_AND_EXPAND:
        case AK_INALLOCA:
        case AK_INDIRECT:
            break;
        }
        arg_values[i] = arg_value;
    }
    LLVMValueRef call_inst = LLVMBuildCall(cg->builder, callee, arg_values, (unsigned int)ir_arg_count, "");
    FREE(arg_values);

    if (has_sret) {
        add_call_arg_type_attribute(cg->context, call_inst, fi->iai.sret_arg_no, "sret", sig_ret_type);
        LLVMSetInstrParamAlignment(call_inst, fi->iai.sret_arg_no + 1 /*always shift with 1*/, ret_tsi.align_bits / 8);
        return ret_alloca;
    }

    if (!parent_ft)
        return call_inst;

    assert(parent_ft->node_type == FUNC_TYPE_NODE);
    //TODO: fix return has to be a valid type, not null
    if (fi->ret.info.type && sig_ret_type != fi->ret.info.type) {
        //create temp memory
        if (!ret_alloca)
            ret_alloca = create_alloca(sig_ret_type, ret_tsi.align_bits / 8, parent_fun, "");
        create_coerced_store(cg->builder, call_inst, ret_alloca, ret_tsi.align_bits / 8);
        return ret_alloca;
    }
    return call_inst;
}
