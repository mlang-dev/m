/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen emit call functions
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
#include "parser/amodule.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include "sema/type.h"
#include "sema/frontend.h"
#include "sema/type_size_info.h"
#include <assert.h>
#include <stdint.h>
#include <float.h>

#define STACK_POINTER_VAR_INDEX 0

bool _is_indirect(struct type_item *type)
{
    return is_aggregate_type(type) && !is_empty_struct(type) && !is_single_element_struct(type);
}

void wasm_emit_call(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == CALL_NODE);
    struct type_context *tc = cg->base.sema_context->tc;
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct ast_node *arg;
    symbol callee = node->call->specialized_callee ? node->call->specialized_callee : node->call->callee;
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, node->call->callee_func_type->type);
    struct ast_node *fun_type = hashtable_get_p(&cg->func_name_2_ast, callee);
    u32 param_num = array_size(&fun_type->ft->params->block->nodes);
    u32 func_index = hashtable_get_int(&cg->func_name_2_idx, callee);
    bool has_sret = fi->tai.sret_arg_no != InvalidIndex;
    struct var_info *vi = 0;
    if(has_sret){
        //emit return value space
        vi = fc_get_var_info(fc, node);
        //alloc = fc_get_alloc(fc, node);
        i32 stack_offset = fc_get_stack_offset(fc, node);

        wasm_emit_assign_var(ba, vi->var_index, false, WasmInstrNumI32ADD, stack_offset, fc->local_sp->var_index, false);
        wasm_emit_get_var(ba, vi->var_index, false); 
        //send the sret address to the calling stack
    }
    struct ast_node *block = block_node_new_empty();
    for(u32 i = 0; i < array_size(&node->call->arg_block->block->nodes); i++){
        arg = array_get_ptr(&node->call->arg_block->block->nodes, i);
        if (!fun_type->ft->is_variadic||i < param_num - 1) {
            //normal argument
            wasm_emit_code(cg, ba, arg);
            //for value type, the value is on the stack
            //for aggregate type, put the reference(address) on the stack
            if(is_aggregate_type(arg->type)){                
                vi = fc_get_var_info(fc, arg);
                if(!is_refered_later(arg) || vi->var_index < fc->local_params || arg->type->type == TYPE_ARRAY){
                    /*rvalue or is parameter, we don't need to make a copy*/
                    wasm_emit_get_var(ba, vi->var_index, false);
                }else{
                    u32 temp_var_index = vi->var_index + 1; //TODO: we should do it explicity in collect_local_variables
                    u32 field_offset = *(u64*)array_get(&fc->stack_size_info.sl->field_offsets, temp_var_index) / 8;
                    wasm_emit_assign_var(ba, temp_var_index, false, WasmInstrNumI32ADD, field_offset, fc->local_sp->var_index, false);
                    wasm_emit_copy_struct_value(tc, ba, temp_var_index, 0, arg->type, vi->var_index, 0);
                    wasm_emit_get_var(ba, temp_var_index, false);
                }
            }
        }else{
            //optional arguments
            block_node_add(block, arg);
        }
    }
    if (fun_type->ft->is_variadic){ 
        if (array_size(&node->call->arg_block->block->nodes) < array_size(&fun_type->ft->params->block->nodes)){
            /*there is no extra arguments, the extra pointer is zero*/
            wasm_emit_const_i32(ba, 0);
        }else{
            //global variable 0 as stack pointer
            //global sp -> stack
            vi = fc_get_var_info(fc, node);
            i32 stack_offset = fc_get_stack_offset(fc, node);
            struct struct_layout *sl = fc_get_stack_sl(fc, node);
            wasm_emit_assign_var(ba, vi->var_index, false, WasmInstrNumI32ADD, stack_offset, fc->local_sp->var_index, false);
            wasm_emit_store_struct_value(cg, ba, vi->var_index, 0, sl, block);
            //lastly, sending start address as optional arguments as the rest call parameter
            wasm_emit_get_var(ba, vi->var_index, false);
        }
    }
    wasm_emit_call_fun(ba, func_index);
    if(has_sret){
        //for reference type, return as a reference to the result
        //wasm_emit_get_var(ba, vi->var_index, false);
    }
    free_block_node(block, false);
}
