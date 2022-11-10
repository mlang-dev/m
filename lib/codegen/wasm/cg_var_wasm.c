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
                if(node->type->type == TYPE_STRUCT){
                    if(!node->is_ret){
                        //for return value optimization, var_index is parameter
                        wasm_emit_assign_var(ba, var_index, false, OPCODE_I32ADD, stack_offset, fc->local_sp->var_index, false);
                    }
                    wasm_emit_copy_struct_value(ba, var_index, 0, node->type, init_vi->var_index, 0);
                }else{//array type is reference type
                    wasm_emit_assign_var(ba, var_index, false, OPCODE_I32ADD, 0, init_vi->var_index, false);
                }
            }
        }else{//scalar value
            if(node->is_addressed){
                //store to stack memory
                u32 align = get_type_align(node->type);
                wasm_emit_store_scalar_value_at(cg, ba, var_index, align, stack_offset, node->var->init_value);
            }else{
                wasm_emit_code(cg, ba, node->var->init_value);
                wasm_emit_set_var(ba, var_index, false);
            }
        }
    }else if(node->type->type==TYPE_ARRAY){
        //assign varable index to the right address of stack
        wasm_emit_assign_var(ba, var_index, false, OPCODE_I32ADD, stack_offset, fc->local_sp->var_index, false);
    }
}
