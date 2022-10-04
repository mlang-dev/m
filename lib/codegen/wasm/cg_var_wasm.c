/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen emit variables
 * 
 */
#include "codegen/wasm/cg_wasm.h"
#include "codegen/wasm/wasm_abi.h"
#include "codegen/wasm/wasm_api.h"
#include "clib/array.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include "sema/analyzer.h"
#include "sema/type.h"
#include "sema/frontend.h"
#include "codegen/type_size_info.h"
#include <assert.h>
#include <stdint.h>
#include <float.h>

void wasm_emit_var(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == VAR_NODE);
    // TODO: var_index zero better is not matched
    if (node->var->init_value){
        wasm_emit_code(cg, ba, node->var->init_value);
        ba_add(ba, OPCODE_LOCALSET); // local.set
        u32 var_index = func_context_get_var_index(cg, node->var->var_name);
        wasm_emit_uint(ba, var_index);
    }
}

void wasm_emit_struct(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{

}

void _wasm_store_struct_member_values(struct cg_wasm *cg, u32 alloca, struct byte_array *ba, struct ast_node *values)
{
    for (size_t i = 0; i < array_size(&values->struct_init->body->block->nodes); i++) {
        struct ast_node *member = *(struct ast_node **)array_get(&values->struct_init->body->block->nodes, i);
        wasm_emit_code(cg, ba, member);

        //LLVMValueRef member = LLVMBuildStructGEP(cg->builder, alloca, (unsigned)i, "");
        //LLVMBuildStore(cg->builder, exp, member);
    }
}

void wasm_emit_struct_init(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    /*
    struct fun_context *fc = get_top_fun_context(cg);
    struct ast_node *ft_node = fc->fun->func->func_type;
    struct type_expr *te = node->type;
    struct type_size_info tsi = get_type_size_info(te);
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, ft_node);
    bool is_rvo = check_rvo(fi);
    //is_ret = is_ret || member_values->is_ret;
    //LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); // builder->GetInsertBlock()->getParent();
    //LLVMValueRef alloca = 0;
    if (is_rvo && node->is_ret) {
        assert(fi->tai.sret_arg_no != InvalidIndex);
        //function parameter with sret: just directly used the pointer passed
        alloca = LLVMGetParam(fun, fi->tai.sret_arg_no);
        _store_struct_member_values(cg, alloca, member_values);
    } else {
        LLVMTypeRef type = (LLVMTypeRef)hashtable_get_p(&cg->typename_2_irtypes, te->name);
        alloca = create_alloca(type, tsi.align_bits / 8, fun, name);
        _store_struct_member_values(cg, alloca, member_values);
    }
    return alloca;
    */
}
