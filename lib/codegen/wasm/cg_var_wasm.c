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
    u32 var_index = func_context_get_var_index(cg, node->var->var_name);
    if (node->var->init_value){
        if (node->type->type==TYPE_STRUCT){
            wasm_emit_struct_init(cg, ba, node->var->init_value);
        }else{
            wasm_emit_code(cg, ba, node->var->init_value);
            wasm_emit_set_var(ba, var_index, false);
        }
    }
}

void wasm_emit_struct(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{

}

void _wasm_store_struct_init_members(struct cg_wasm *cg, u32 local_var_index, struct byte_array *ba, struct ast_node *node)
{
    u32 offset = 0;
    for (size_t i = 0; i < array_size(&node->struct_init->body->block->nodes); i++) {
        struct ast_node *member = *(struct ast_node **)array_get(&node->struct_init->body->block->nodes, i);
        offset = wasm_emit_store_value(cg, ba, local_var_index, offset, member);
    }
}

void wasm_emit_struct_init(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    struct fun_context *fc = get_top_fun_context(cg);
    struct ast_node *ft_node = fc->fun->func->func_type;
    //struct type_expr *te = node->type;
    //struct type_size_info tsi = get_type_size_info(te);
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, ft_node);
    bool is_rvo = check_rvo(fi);
    //is_ret = is_ret || member_values->is_ret;
    if (is_rvo && node->is_ret) {
        assert(fi->tai.sret_arg_no != InvalidIndex);
        //function parameter with sret: just directly used the pointer passed
        _wasm_store_struct_init_members(cg, fi->tai.sret_arg_no, ba, node);
    } else {
        struct var_info *vi = hashtable_get_p(&fc->ast_2_index, node);
        assert(vi);
        int alloc_index = hashtable_get_int(&fc->ast_2_alloc_index, node);
        assert(alloc_index>=0);
        struct mem_alloc *alloc = array_get(&fc->allocs, alloc_index);
        wasm_emit_assign_var(ba, vi->index, false, OPCODE_I32ADD, alloc->address, fc->local_sp->index, false);
        _wasm_store_struct_init_members(cg, vi->index, ba, node);
    }
}
