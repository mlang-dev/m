/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation for functions
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "codegen/cg_fun.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"
#include "sema/type.h"
#include <llvm-c/Support.h>

void _emit_argument_allocas(struct code_generator *cg, struct prototype_node *node,
    struct fun_info *fi, LLVMValueRef fun)
{
    struct type_oper *proto_type = (struct type_oper *)node->base.type;
    //assert (LLVMCountParams(fun) == array_size(&proto_type->args) - 1);
    unsigned param_count = array_size(&fi->args);
    struct array params;
    array_init(&params, sizeof(struct aligned_address));
    for (unsigned i = 0; i < param_count; i++) {
        struct var_node *param = (struct var_node *)array_get(&node->fun_params, i);
        struct type_exp *type_exp = *(struct type_exp **)array_get(&proto_type->args, i);
        struct ast_abi_arg *aaa = (struct ast_abi_arg *)array_get(&fi->args, i);
        struct ir_arg_range *iar = (struct ir_arg_range *)array_get(&fi->iai.args, i);
        unsigned first_ir_arg = iar->first_arg_index;
        unsigned arg_num = iar->arg_num;
        struct aligned_address param_value;
        param_value.pointer = 0;
        param_value.alignment = 0;
        switch (aaa->info.kind) {
        case AK_INDIRECT:
        case AK_INDIRECT_ALIASED: {
            assert(arg_num == 1);
            param_value.pointer = LLVMGetParam(fun, first_ir_arg);
            param_value.alignment = aaa->info.indirect_align;
            if (proto_type->base.type < TYPE_EXT) { //aggregate
                //
                if (aaa->info.indirect_realign || aaa->info.kind == AK_INDIRECT_ALIASED) {
                    //realign the value, if the address is aliased, copy the param to ensure
                    //a unique address
                } else {
                }
            }
            array_push(&params, &param_value);
        }
        }
        //TODO: fix the inconsistency enum type type = get_type(param->base.type);

        // Create a debug descriptor for the variable.
        /*DIScope *Scope = KSDbgInfo.LexicalBlocks.back();
    DIFile *Unit = DBuilder->createFile(KSDbgInfo.TheCU->getFilename(),
                                        KSDbgInfo.TheCU->getDirectory());
    DILocalVariable *D = DBuilder->createLocalVariable(
                                                       dwarf::DW_TAG_arg_variable,
    Scope, Args[Idx], Unit, Line, KSDbgInfo.getDoubleTy(), Idx);

    DBuilder->insertDeclare(Alloca, D, DBuilder->createExpression(),
                            DebugLoc::get(Line, 0, Scope),
                            Builder.GetInsertBlock());
    */
        if (!param_value.alignment) {
            //not indirect pointer, copy value to alloca
            enum type type = get_type(type_exp);
            LLVMValueRef alloca = emit_entry_block_alloca(
                cg->ops[type].get_type(cg->context, type_exp), fun, string_get(param->var_name));
            LLVMBuildStore(cg->builder, LLVMGetParam(fun, i), alloca);
            hashtable_set(&cg->named_values, string_get(param->var_name), alloca);
        } else {
            hashtable_set(&cg->named_values, string_get(param->var_name), LLVMGetParam(fun, i));
        }
    }
    array_deinit(&params);
}

LLVMValueRef emit_entry_block_alloca(
    LLVMTypeRef type,
    LLVMValueRef fun,
    const char *var_name)
{
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMBasicBlockRef bb = LLVMGetEntryBasicBlock(fun);
    LLVMPositionBuilder(builder, bb, LLVMGetFirstInstruction(bb));
    LLVMValueRef alloca = LLVMBuildAlloca(builder, type, var_name);
    LLVMDisposeBuilder(builder);
    return alloca;
}

LLVMValueRef emit_prototype_node(struct code_generator *cg, struct exp_node *node, struct fun_info **out_fi)
{
    struct prototype_node *proto = (struct prototype_node *)node;
    assert(proto->base.type);
    hashtable_set(&cg->protos, string_get(proto->name), proto);
    struct type_oper *proto_type = (struct type_oper *)proto->base.type;
    assert(proto_type->base.kind == KIND_OPER);
    struct fun_info *fi = get_fun_info(proto);
    if (out_fi)
        *out_fi = fi;
    assert(fi);
    LLVMTypeRef fun_type = get_fun_type(fi);
    LLVMValueRef fun = LLVMAddFunction(cg->module, string_get(proto->name), fun_type);
    if (fi->iai.sret_arg_no != InvalidIndex) {
        LLVMValueRef ai = LLVMGetParam(fun, fi->iai.sret_arg_no);
        const char *sret_var = "agg.result";
        LLVMSetValueName2(ai, sret_var, strlen(sret_var));
        //TODO add noalias attribute to the var
    }
    unsigned param_count = array_size(&fi->args);
    for (unsigned i = 0; i < param_count; i++) {
        LLVMValueRef param = LLVMGetParam(fun, i);
        struct var_node *fun_param = (struct var_node *)array_get(&proto->fun_params, i);
        LLVMSetValueName2(param, string_get(fun_param->var_name), string_size(fun_param->var_name));
        struct ast_abi_arg *aa = (struct ast_abi_arg *)array_get(&fi->args, i);
        if (aa->type->type == TYPE_EXT)
            hashtable_set(&cg->varname_2_typename, string_get(fun_param->var_name), aa->type->name);
    }
    return fun;
}

LLVMValueRef emit_function_node(struct code_generator *cg, struct exp_node *node)
{
    struct function_node *fun_node = (struct function_node *)node;
    if (is_generic(node->type)) {
        return 0;
    }
    assert(fun_node->base.type->kind == KIND_OPER);
    hashtable_clear(&cg->named_values);
    struct fun_info *fi = 0;
    LLVMValueRef fun = emit_prototype_node(cg, (struct exp_node *)fun_node->prototype, &fi);
    assert(fun && fi);

    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(cg->context, fun, "entry");
    LLVMPositionBuilderAtEnd(cg->builder, bb);
    _emit_argument_allocas(cg, fun_node->prototype, fi, fun);
    LLVMValueRef ret_val = 0;
    for (size_t i = 0; i < array_size(&fun_node->body->nodes); i++) {
        struct exp_node *stmt = *(struct exp_node **)array_get(&fun_node->body->nodes, i);
        ret_val = emit_ir_code(cg, stmt);
    }
    if (!ret_val) {
        struct type_exp *ret_type = get_ret_type(fun_node);
        enum type type = get_type(ret_type);
        ret_val = cg->ops[type].get_zero(cg->context, cg->builder);
    }
    assert(ret_val);
    LLVMBuildRet(cg->builder, ret_val);
    return fun;
}

LLVMValueRef get_llvm_function(struct code_generator *cg, const char *name)
{
    LLVMValueRef f = LLVMGetNamedFunction(cg->module, name);
    if (f)
        return f;
    struct exp_node *fp = (struct exp_node *)hashtable_get(&cg->protos, name);
    if (fp)
        return emit_prototype_node(cg, fp, 0);
    return 0;
}
