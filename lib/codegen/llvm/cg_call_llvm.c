/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation for function calls
 */
#include "codegen/llvm/cg_call_llvm.h"
#include "codegen/llvm/cg_fun_llvm.h"
#include "codegen/llvm/cg_llvm.h"
#include "codegen/fun_info.h"
#include "codegen/llvm/llvm_api.h"
#include "codegen/target_arg_info.h"
#include "codegen/type_size_info.h"
#include <assert.h>


// get parent func sret parameter if exists
LLVMValueRef _get_parent_call_sret_pointer(struct cg_llvm *cg, struct ast_node *node)
{
    struct ast_node *parent_func = *(struct ast_node**)stack_top(&cg->base.sema_context->func_stack);
    struct ast_node *parent_ft = parent_func->func->func_type;
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, parent_ft->type);
    bool has_sret = fi->tai.sret_arg_no != InvalidIndex;
    LLVMValueRef ret = 0;
    if (has_sret && node->is_ret) {
        LLVMValueRef fun = get_llvm_function(cg, parent_ft->ft->name);
        ret = LLVMGetParam(fun, fi->tai.sret_arg_no);
    }
    return ret;
}

LLVMValueRef emit_call_node(struct cg_llvm *cg, struct ast_node *node)
{
    assert(node->call->callee_func_type);
    symbol callee_name = get_callee(node);
    LLVMValueRef callee = get_llvm_function(cg, callee_name);
    assert(callee);
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, node->call->callee_func_type->type);
    assert(fi);
    size_t arg_count = array_size(&node->call->arg_block->block->nodes);
    bool has_sret = fi->tai.sret_arg_no != InvalidIndex;
    size_t ir_arg_count =  has_sret ? arg_count + 1 : arg_count;
    LLVMValueRef *arg_values;
    MALLOC(arg_values, ir_arg_count * sizeof(LLVMValueRef));
    LLVMTypeRef sig_ret_type = get_llvm_type(fi->ret.type);
    struct ast_node *parent_func = *(struct ast_node**)stack_top(&cg->base.sema_context->func_stack);
    struct ast_node *parent_ft = parent_func->func->func_type;
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
        arg_values[fi->tai.sret_arg_no] = ret_alloca;
    }
    for (size_t i = 0; i < arg_count; ++i) {
        struct abi_arg_info *aai = array_get(&fi->args, i);
        struct target_arg_range *tar = array_get(&fi->tai.args, i);
        struct ast_node *arg = array_get_ptr(&node->call->arg_block->block->nodes, i);
        struct type_size_info tsi = get_type_size_info(arg->type);
        LLVMValueRef arg_value = emit_ir_code(cg, arg);
        switch (aai->kind) {
        case AK_DIRECT: {
            if (!aai->target_type)
                break; //FIXIME: is_variadic arg, no available ir arg
            LLVMTypeKind tk = LLVMGetTypeKind(aai->target_type);
            if (tk != LLVMStructTypeKind && aai->target_type == get_llvm_type(aai->type) && aai->align.direct_offset == 0) {
                break;
            }
            if (tk == LLVMStructTypeKind && aai->kind == AK_DIRECT && aai->can_be_flattened) {

            } else {
                assert(tar->target_arg_num == 1);
                arg_value = create_coerced_load(cg->builder, arg_value, aai->target_type, tsi.align_bits / 8);
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
    LLVMValueRef call_inst = LLVMBuildCall2(cg->builder, LLVMGetElementType(LLVMTypeOf(callee)), callee, arg_values, (unsigned int)ir_arg_count, "");
    FREE(arg_values);

    if (has_sret) {
        add_call_arg_type_attribute(cg->context, call_inst, fi->tai.sret_arg_no, "sret", sig_ret_type);
        LLVMSetInstrParamAlignment(call_inst, fi->tai.sret_arg_no + 1 /*always shift with 1*/, ret_tsi.align_bits / 8);
        return ret_alloca;
    }

    if (!parent_ft)
        return call_inst;

    assert(parent_ft->node_type == FUNC_TYPE_NODE);
    //TODO: fix return has to be a valid type, not null
    if (fi->ret.target_type && sig_ret_type != fi->ret.target_type) {
        //create temp memory
        if (!ret_alloca)
            ret_alloca = create_alloca(sig_ret_type, ret_tsi.align_bits / 8, parent_fun, "");
        create_coerced_store(cg->builder, call_inst, ret_alloca, ret_tsi.align_bits / 8);
        return ret_alloca;
    }
    return call_inst;
}
