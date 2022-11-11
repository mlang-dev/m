/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen emit array type
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

void wasm_emit_store_struct_value(struct cg_wasm *cg, struct byte_array *ba, u32 local_address_var_index, u32 offset, struct struct_layout *sl, struct ast_node *block)
{
    struct ast_node *field;
    u32 field_offset;
    for (u32 i = 0; i < array_size(&block->block->nodes); i++) {
        field = *(struct ast_node **)array_get(&block->block->nodes, i);
        field_offset = *(u64*)array_get(&sl->field_offsets, i) / 8;
        u32 align = get_type_align(field->type);
        if(field->type->type == TYPE_STRUCT){
            assert(field->node_type == STRUCT_INIT_NODE);
            struct struct_layout *field_sl = *(struct struct_layout**)array_get(&sl->field_layouts, i);
            wasm_emit_store_struct_value(cg, ba, local_address_var_index, offset + field_offset, field_sl, field->struct_init->body);
        }else{
            wasm_emit_store_scalar_value_at(cg, ba, local_address_var_index, align, offset + field_offset, field);
        }
    }
}

void wasm_emit_store_array_value(struct cg_wasm *cg, struct byte_array *ba, u32 local_address_var_index, u32 offset, u32 elm_align, u32 elm_type_size, struct ast_node *array_init)
{
    struct ast_node *field;
    u32 field_offset = 0;
    if(array_init->node_type == BLOCK_NODE){
        for (u32 i = 0; i < array_size(&array_init->block->nodes); i++) {
            field = *(struct ast_node **)array_get(&array_init->block->nodes, i);
            wasm_emit_store_scalar_value_at(cg, ba, local_address_var_index, elm_align, offset + field_offset, field);
            field_offset += elm_type_size;
        }
    }
}

void wasm_emit_struct_init(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct ast_node *ft_node = fc->fun->func->func_type;
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, ft_node);
    bool is_rvo = check_rvo(fi);
    struct type_size_info tsi = get_type_size_info(node->type);
    if (is_rvo && node->is_ret) {
        assert(fi->tai.sret_arg_no != InvalidIndex);
        //function parameter with sret: just directly used the pointer passed
        wasm_emit_store_struct_value(cg, ba, fi->tai.sret_arg_no, 0, tsi.sl, node->struct_init->body);
        //no return
    } else {
        struct var_info *vi = fc_get_var_info(fc, node);
        i32 stack_offset = fc_get_stack_offset(fc, node);
        wasm_emit_assign_var(ba, vi->var_index, false, OPCODE_I32ADD, stack_offset, fc->local_sp->var_index, false);
        wasm_emit_store_struct_value(cg, ba, vi->var_index, 0, tsi.sl, node->struct_init->body);
    }
}

void wasm_emit_array_init(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    if (!node->array_init) return; //empty list does nothing
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct ast_node *ft_node = fc->fun->func->func_type;
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, ft_node);
    bool is_rvo = check_rvo(fi);
    u32 addr_var_index;
    if (is_rvo && node->is_ret) {
        assert(fi->tai.sret_arg_no != InvalidIndex);
        //function parameter with sret: returned array is allocated by caller, and 
        //its address is passed down to callee, we only need to fill the memory
        addr_var_index = fi->tai.sret_arg_no;
        //no return
    } else {
        struct var_info *vi = fc_get_var_info(fc, node);
        i32 stack_offset = fc_get_stack_offset(fc, node);
        wasm_emit_assign_var(ba, vi->var_index, false, OPCODE_I32ADD, stack_offset, fc->local_sp->var_index, false);
        addr_var_index = vi->var_index;
    }
    struct type_size_info tsi = get_type_size_info(node->type->val_type);
    wasm_emit_store_array_value(cg, ba, addr_var_index, 0, tsi.align_bits/8, tsi.width_bits / 8, node->array_init);
}
