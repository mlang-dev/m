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
#include "codegen/type_size_info.h"
#include <assert.h>
#include <stdint.h>
#include <float.h>

#define STACK_POINTER_VAR_INDEX 0

bool _is_indirect(struct type_expr *type)
{
    return is_aggregate_type(type->type) && !is_empty_struct(type) && !is_single_element_struct(type);
}

void wasm_emit_call(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == CALL_NODE);
    struct ast_node *arg;
    symbol callee = node->call->specialized_callee ? node->call->specialized_callee : node->call->callee;
    struct ast_node *fun_type = hashtable_get_p(&cg->func_name_2_ast, callee);
    u32 param_num = array_size(&fun_type->ft->params->block->nodes);
    u32 func_index = hashtable_get_int(&cg->func_name_2_idx, callee);
    u32 stack_size = 0;
    u32 size;
    for(u32 i = 0; i < array_size(&node->call->arg_block->block->nodes); i++){
        arg = *(struct ast_node **)array_get(&node->call->arg_block->block->nodes, i);
        if (!fun_type->ft->is_variadic||i < param_num - 1) {
            wasm_emit_code(cg, ba, arg);
        }else{//optional arguments
            size = get_type_size(arg->type);
            if (size < 4) size = 4;
            stack_size = align_to(stack_size, size);
            stack_size += size;  //clang-wasm ABI always uses 16 bytes alignment
        }
    }
    //make a copy to prevent the callee from changing it
    stack_size = align_to(stack_size * 2, 16);

    u32 local_var_index = 0;
    u32 arg_type_size = 0;
    if (fun_type->ft->is_variadic){ 
        if (array_size(&node->call->arg_block->block->nodes) < array_size(&fun_type->ft->params->block->nodes)){
            wasm_emit_const_i32(ba, 0);
        }else{
            //global variable 0 as stack pointer
            //global sp -> stack
            local_var_index = func_get_local_var_index(cg, node);
            ba_add(ba, OPCODE_GLOBALGET);
            wasm_emit_uint(ba, STACK_POINTER_VAR_INDEX);
            wasm_emit_const_i32(ba, stack_size);
            ba_add(ba, OPCODE_I32SUB); // saved sp-stack_size address to stack
            ba_add(ba, OPCODE_LOCALSET);
            wasm_emit_uint(ba, local_var_index); //saved stack value to local var

            //set global sp to the new address
            ba_add(ba, OPCODE_LOCALGET);
            wasm_emit_uint(ba, local_var_index);
            ba_add(ba, OPCODE_GLOBALSET);
            wasm_emit_uint(ba, STACK_POINTER_VAR_INDEX); // global set variable

            u32 offset = 0;
            for (u32 i = array_size(&fun_type->ft->params->block->nodes) - 1; i < array_size(&node->call->arg_block->block->nodes); i++) {
                arg = *(struct ast_node **)array_get(&node->call->arg_block->block->nodes, i);
                //get local variable containing the start address to stack
                ba_add(ba, OPCODE_LOCALGET);
                wasm_emit_uint(ba, local_var_index);

                // content of the arg to stack
                wasm_emit_code(cg, ba, arg);  
                ba_add(ba, type_2_store_op[arg->type->type]);
                //align(u32), and offset(u32)
                arg_type_size = type_size(arg->type->type); 
                wasm_emit_uint(ba, arg_type_size == 8? ALIGN_EIGHT_BYTES : ALIGN_FOUR_BYTES);
                //we need to adjust offset for better alignment
                if (offset % arg_type_size != 0){
                    offset = (offset / arg_type_size + 1) * arg_type_size;
                }
                wasm_emit_uint(ba, offset);
                offset += arg_type_size;
            }
            //lastly, sending start address as optional arguments as the rest call parameter
            ba_add(ba, OPCODE_LOCALGET);
            wasm_emit_uint(ba, local_var_index);
        }
    }
    ba_add(ba, OPCODE_CALL); // num local variables
    wasm_emit_uint(ba, func_index);

    if(is_variadic_call_with_optional_arguments(cg, node)){
        // reset back to stack size
        ba_add(ba, OPCODE_LOCALGET);
        wasm_emit_uint(ba, local_var_index);
        wasm_emit_const_i32(ba, stack_size);
        ba_add(ba, OPCODE_I32ADD); // add back to original sp
        ba_add(ba, OPCODE_GLOBALSET);
        wasm_emit_uint(ba, 0); //global sp always is zero
    }
}
