/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation for function calls
 */
#include "codegen/cg_call.h"
#include "codegen/cg_fun.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/ir_arg_info.h"
#include <assert.h>
// void emit_call(struct call_node *call)
// {
//     struct fun_info *fi = get_fun_info(call->callee_decl);
//     LLVMValueRef ir_fun_type = get_fun_type(fi);
// }

LLVMValueRef emit_call_node(struct code_generator *cg, struct exp_node *node)
{
    struct call_node *call = (struct call_node *)node;
    assert(call->callee_decl);
    symbol callee_name = get_callee(call);
    struct fun_info *fi = get_fun_info(call->callee_decl);
    assert(fi);
    LLVMValueRef callee = get_llvm_function(cg, callee_name);
    assert(callee);
    LLVMValueRef *arg_values = malloc(array_size(&call->args) * sizeof(LLVMValueRef));
    for (size_t i = 0, e = array_size(&call->args); i != e; ++i) {
        struct exp_node *arg = *(struct exp_node **)array_get(&call->args, i);
        arg_values[i] = emit_ir_code(cg, arg);
    }
    LLVMValueRef value = LLVMBuildCall(cg->builder, callee, arg_values, array_size(&call->args), "");
    free(arg_values);
    return value;
}
