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
    struct fun_context *fc = cg_get_top_fun_context(cg);
    u32 var_index = fc_get_var_info(fc, node)->var_index;
    i32 stack_offset = fc_get_stack_offset(fc, node);
    if (node->var->init_value){
        if(is_aggregate_type(node->type)){
            struct var_info *init_vi = fc_get_var_info(fc, node->var->init_value);
            wasm_emit_code(cg, ba, node->var->init_value);
            if(init_vi->var_index != var_index){
                //set var index address
                if(!node->is_ret){
                    //for return value optimization, var_index is parameter
                    wasm_emit_assign_var(ba, var_index, false, OPCODE_I32ADD, stack_offset, fc->local_sp->var_index, false);
                }
                wasm_emit_copy_struct_value(ba, var_index, 0, node->type, init_vi->var_index, 0);
            }
        }else{//scalar value
            if(node->is_addressed){
                //store to stack memory
                u32 align = get_type_align(node->type);
                wasm_emit_store_scalar_value(cg, ba, var_index, align, stack_offset, node->var->init_value);
            }else{
                wasm_emit_code(cg, ba, node->var->init_value);
                wasm_emit_set_var(ba, var_index, false);
            }
        }
    }
}

void wasm_emit_struct(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
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
