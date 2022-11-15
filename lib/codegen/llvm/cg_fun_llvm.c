/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation for functions
 */
#include <assert.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "codegen/abi_arg_info.h"
#include "codegen/llvm/cg_fun_llvm.h"
#include "codegen/llvm/cg_llvm.h"
#include "codegen/fun_info.h"
#include "codegen/llvm/llvm_api.h"
#include "codegen/type_size_info.h"
#include "sema/type.h"
#include <llvm-c/Support.h>

struct address emit_address_at_offset(struct cg_llvm *cg, struct address adr, struct abi_arg_info *info)
{
    (void)cg;
    unsigned offset = info->align.direct_offset;
    if (offset) {
    }
    return adr;
}

void _emit_argument_allocas(struct cg_llvm *cg, struct ast_node *node,
    struct fun_info *fi, LLVMValueRef fun)
{
    struct type_expr *proto_type = node->type;
    //assert (LLVMCountParams(fun) == array_size(&proto_type->args) - 1);
    unsigned param_count = (unsigned)array_size(&fi->args);
    struct array params;
    array_init(&params, sizeof(struct address));
    for (unsigned i = 0; i < param_count; i++) {
        struct ast_node *param = *(struct ast_node **)array_get(&node->ft->params->block->nodes, i);
        //struct type_expr *type_exp = *(struct type_expr **)array_get(&proto_type->args, i);
        struct abi_arg_info *aai = (struct abi_arg_info *)array_get(&fi->args, i);
        struct target_arg_range *tar = (struct target_arg_range *)array_get(&fi->tai.args, i);
        unsigned first_ir_arg = tar->first_arg_index;
        unsigned target_arg_num = tar->target_arg_num;
        struct address param_value;
        LLVMValueRef arg_value = LLVMGetParam(fun, first_ir_arg);
        param_value.pointer = 0;
        param_value.alignment = 0;
        switch (aai->kind) {
        case AK_INDIRECT:
        case AK_INDIRECT_ALIASED: {
            assert(target_arg_num == 1);
            param_value.pointer = arg_value;
            param_value.alignment = aai->align.indirect_align;
            if (proto_type->type < TYPE_STRUCT) { //aggregate
                //
                if (aai->indirect_realign || aai->kind == AK_INDIRECT_ALIASED) {
                    //realign the value, if the address is aliased, copy the param to ensure
                    //a unique address
                } else {
                }
            }
            array_push(&params, &param_value);
            hashtable_set_p(&cg->varname_2_irvalues, param->var->var->ident->name, param_value.pointer);
            break;
        }
        case AK_DIRECT: {
            LLVMValueRef alloca = 0;
            struct type_size_info tsi = get_type_size_info(aai->type);
            unsigned align = tsi.align_bits / 8;
            LLVMTypeRef sig_type = get_llvm_type(aai->type);
            if (LLVMGetTypeKind(aai->target_type) != LLVMStructTypeKind && aai->align.direct_offset == 0
                && aai->target_type == sig_type) {
                alloca = create_alloca(
                    aai->target_type, align, fun, string_get(param->var->var->ident->name));
                LLVMBuildStore(cg->builder, arg_value, alloca);
            } else {
                //TODO: if struct type
                //aaa->info.type
                size_t length;
                string arg_name;
                string_init_chars(&arg_name, LLVMGetValueName2(arg_value, &length));
                string_add_chars(&arg_name, ".coerce");
                LLVMSetValueName2(arg_value, string_get(&arg_name), string_size(&arg_name));
                alloca = create_alloca(
                    sig_type, align, fun, string_get(param->var->var->ident->name));
                create_coerced_store(cg->builder, arg_value, alloca, align);
            }
            hashtable_set_p(&cg->varname_2_irvalues, param->var->var->ident->name, alloca);
            break;
        }
        default: {
            #ifdef _WIN32
            assert(false);
            #endif
        }
        }
    }
    array_deinit(&params);
}

LLVMValueRef emit_func_type_node(struct cg_llvm *cg, struct ast_node *node)
{
    return emit_func_type_node_fi(cg, node, 0);
}

//compute abi info

LLVMValueRef emit_func_type_node_fi(struct cg_llvm *cg, struct ast_node *node, struct fun_info **out_fi)
{
    assert(node->type);
    struct type_expr *proto_type = node->type;
    assert(proto_type->kind == KIND_OPER);
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, node);
    if (out_fi)
        *out_fi = fi;
    assert(fi);
    LLVMTypeRef fun_type = create_target_fun_type(cg->base.target_info, fi);
    LLVMValueRef fun = LLVMAddFunction(cg->module, string_get(node->ft->name), fun_type);
    if (fi->tai.sret_arg_no != InvalidIndex) {
        LLVMValueRef ai = LLVMGetParam(fun, fi->tai.sret_arg_no);
        const char *sret_var = "agg.result";
        LLVMSetValueName2(ai, sret_var, strlen(sret_var));
        add_fun_param_attribute(cg->context, fun, fi->tai.sret_arg_no, "noalias");
        add_fun_param_type_attribute(cg->context, fun, fi->tai.sret_arg_no, "sret", get_llvm_type(fi->ret.type));
    }
    unsigned param_count = (unsigned)array_size(&fi->args);
    for (unsigned i = 0; i < param_count; i++) {
        LLVMValueRef param = LLVMGetParam(fun, i);
        struct ast_node *fun_param = *(struct ast_node **)array_get(&node->ft->params->block->nodes, i);
        LLVMSetValueName2(param, string_get(fun_param->var->var->ident->name), string_size(fun_param->var->var->ident->name));
        struct abi_arg_info *aai = (struct abi_arg_info *)array_get(&fi->args, i);
        if (aai->type->type == TYPE_STRUCT)
            hashtable_set_p(&cg->varname_2_typename, fun_param->var->var->ident->name, aai->type->name);
    }
    return fun;
}

LLVMValueRef emit_function_node(struct cg_llvm *cg, struct ast_node *node)
{
    if (is_generic(node->type)) {
        return 0;
    }
    stack_push(&cg->base.sema_context->func_stack, &node);
    assert(node->type->kind == KIND_OPER);
    hashtable_clear(&cg->varname_2_irvalues);
    struct fun_info *fi = 0;
    LLVMValueRef fun = emit_func_type_node_fi(cg, node->func->func_type, &fi);
    assert(fun && fi);

    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(cg->context, fun, "entry");
    LLVMPositionBuilderAtEnd(cg->builder, bb);
    _emit_argument_allocas(cg, node->func->func_type, fi, fun);
    LLVMValueRef ret_val = 0;
    //handle ret value
    // if (fi->ret.type->type == TYPE_UNIT) {

    // } else if (fi->ret.info.kind == AK_INDIRECT) {
    //     // sret struct type return type
    // } else {
    //     LLVMTypeRef ret_type = get_llvm_type(fi->ret.type);
    //     if (ret_type != fi->ret.info.type) {
    //         struct type_size_info tsi = get_type_size_info(fi->ret.type);
    //         unsigned align = tsi.align_bits / 8;
    //         LLVMValueRef ret_alloca = create_alloca(ret_type, align, fun, "retval");
    //     }
    // }
    for (size_t i = 0; i < array_size(&node->func->body->block->nodes); i++) {
        struct ast_node *stmt = *(struct ast_node **)array_get(&node->func->body->block->nodes, i);
        ret_val = emit_ir_code(cg, stmt);
    }
    if (!ret_val || !fi->ret.target_type) {
        //struct type_expr *ret_type = get_ret_type(fun_node);
        //enum type type = get_type(ret_type);
        //ret_val = cg->ops[type].get_zero(cg->context, cg->builder);
        LLVMBuildRetVoid(cg->builder);
    } else {

        LLVMTypeRef ret_type = LLVMTypeOf(ret_val);
        if (LLVMGetTypeKind(ret_type) != LLVMGetTypeKind(fi->ret.target_type)) {
            // assuming cast struct to
            struct type_size_info tsi = get_type_size_info(fi->ret.type);
            LLVMTypeRef ret_ptr = LLVMPointerType(fi->ret.target_type, 0);
            assert(LLVMGetTypeKind(ret_type) == LLVMGetTypeKind(ret_ptr));
            //cast struct pointer to int pointer
            ret_val = LLVMBuildBitCast(cg->builder, ret_val, ret_ptr, "");
            //load int from int pointer
            ret_val = LLVMBuildLoad2(cg->builder, fi->ret.target_type, ret_val, "");
            LLVMSetAlignment(ret_val, tsi.align_bits / 8);
        }
        LLVMBuildRet(cg->builder, ret_val);
    }
    struct ast_node *saved_node = *(struct ast_node **)stack_pop(&cg->base.sema_context->func_stack);
    assert(node == saved_node);
    return fun;
}

LLVMValueRef get_llvm_function(struct cg_llvm *cg, symbol fun_name)
{
    const char *name = string_get(fun_name);
    LLVMValueRef f = LLVMGetNamedFunction(cg->module, name);
    if (f)
        return f;
    struct ast_node *fp = hashtable_get_p(&cg->base.sema_context->func_types, fun_name);
    if (fp)
        return emit_func_type_node(cg, fp);
    return 0;
}
