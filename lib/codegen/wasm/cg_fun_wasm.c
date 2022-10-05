/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen emit functions
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
#include <assert.h>
#include <stdint.h>
#include <float.h>

#define STACK_POINTER_VAR_INDEX 0

struct fun_context *_func_enter(struct cg_wasm *cg, struct ast_node *fun)
{
    struct fun_context *fc = &cg->fun_contexts[cg->fun_top];
    fc_init(fc);
    fc->fun = fun;
    cg->fun_top ++;
    return fc;
}

void _func_leave(struct cg_wasm *cg, struct ast_node *fun)
{
    cg->fun_top--;
    struct fun_context *fc = &cg->fun_contexts[cg->fun_top];
    cg->var_top -= fc->local_vars;
    fc_deinit(fc);
    assert(cg->fun_contexts[cg->fun_top].fun == fun);
}


void collect_local_variables(struct cg_wasm *cg, struct ast_node *node)
{
    switch(node->node_type)
    {
        default:
            break;
        case FOR_NODE:
            func_register_local_variable(cg, node, node->forloop->start->type->type, true);
            collect_local_variables(cg, node->forloop->body);
            break;
        case VAR_NODE:
            func_register_local_variable(cg, node, node->type->type, true);
            break;
        case CALL_NODE:
            /*for variadic function call, we might need one local variable*/
            if(is_variadic_call_with_optional_arguments(cg, node)){
                func_register_local_variable(cg, node, TYPE_INT, true);
            }
            break;
        case BLOCK_NODE:
            for(u32 i = 0; i < array_size(&node->block->nodes); i++){
                collect_local_variables(cg, *(struct ast_node **)array_get(&node->block->nodes, i));
            }
            break;
    }
}

u32 _func_get_local_var_nums(struct cg_wasm *cg)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    return fc->local_vars - fc->local_params;
}

struct var_info *_req_new_local_var(struct cg_wasm *cg, enum type type, bool is_local_var)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    u32 index = fc->local_vars++;
    if (!is_local_var) {
        fc->local_params++;
    }
    struct var_info *vi = &cg->local_vars[cg->var_top];
    vi->var_index = index;
    ASSERT_TYPE(type);
    vi->target_type = type_2_wtype[type];
    vi->alloc_index = -1;
    cg->var_top++;
    return vi;
}

/*
 * register local variable & stack space
 */ 
void func_register_local_variable(struct cg_wasm *cg, struct ast_node *node, enum type type, bool is_local_var)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct var_info *vi = _req_new_local_var(cg, type, is_local_var);
    if (node->node_type == VAR_NODE) {
        if (type == TYPE_STRUCT){
            struct ast_node * struct_def = hashtable_get_p(&cg->base.sema_context->struct_typename_2_asts, node->type->name);
            vi->alloc_index = fc_register_alloc(fc, struct_def->type);
        }
        hashtable_set_p(&fc->ast_2_index, node, vi);
        if(node->var->init_value){
            hashtable_set_p(&fc->ast_2_index, node->var->init_value, vi);
        }
        symboltable_push(&fc->varname_2_index, node->var->var_name, vi);

    } else if (node->node_type == FOR_NODE) {
        symboltable_push(&fc->varname_2_index, node->forloop->var_name, vi);
        vi = _req_new_local_var(cg, node->forloop->step->type->type, true);
        hashtable_set_p(&fc->ast_2_index, node->forloop->step, vi);
        vi = _req_new_local_var(cg, node->forloop->end->type->type, true);
        hashtable_set_p(&fc->ast_2_index, node->forloop->end, vi);
    } else if (node->node_type == CALL_NODE) {
        hashtable_set_p(&fc->ast_2_index, node, vi);
        symbol callee = node->call->specialized_callee ? node->call->specialized_callee : node->call->callee;
        struct ast_node *fun_type = hashtable_get_p(&cg->func_name_2_ast, callee);
        u32 param_num = array_size(&fun_type->ft->params->block->nodes);
        struct ast_node *block = block_node_new_empty();

        struct array arg_types;
        array_init(&arg_types, sizeof(struct type_expr *));
        for(u32 i = 0; i < array_size(&node->call->arg_block->block->nodes); i++){
            struct ast_node *arg = *(struct ast_node **)array_get(&node->call->arg_block->block->nodes, i);
            if (!fun_type->ft->is_variadic||i < param_num - 1) {
                continue;
            }
            block_node_add(block, arg);
            array_push(&arg_types, &arg->type);
        }
        struct type_oper *to = create_type_oper_struct(0, &arg_types);
        vi->alloc_index = fc_register_alloc(fc, &to->base);
        type_exp_free(&to->base);
        array_deinit(&arg_types);        
        free_block_node(block, false);
    }
}

void wasm_emit_func(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == FUNC_NODE);
    struct fun_context *fc = _func_enter(cg, node);
    assert(node->type->kind == KIND_OPER);
    struct type_oper *to = (struct type_oper *)node->type;
    for(u32 i=0; i < array_size(&node->func->func_type->ft->params->block->nodes); i++){
        struct ast_node *param = *(struct ast_node **)array_get(&node->func->func_type->ft->params->block->nodes, i);
        func_register_local_variable(cg, param, (*(struct type_expr**)array_get(&to->args,i))->type, false);
    }
    collect_local_variables(cg, node->func->body);
    u32 stack_size = fc_get_stack_size(fc);
    if(stack_size){
        fc->local_sp = _req_new_local_var(cg, TYPE_INT, true);
    }
    
    struct byte_array func;
    ba_init(&func, 17);
    u32 local_vars = _func_get_local_var_nums(cg);
    wasm_emit_uint(&func, local_vars); // num local variables
    u32 start_pos = cg->var_top - local_vars;
    for(u32 i = 0; i < local_vars; i++){
        wasm_emit_uint(&func, 1); // num local following types
        ba_add(&func, cg->local_vars[start_pos + i].target_type);
    }

    if(stack_size){
        //adjust sp
        wasm_emit_assign_var(&func, fc->local_sp->var_index, false, OPCODE_I32SUB, stack_size, STACK_POINTER_VAR_INDEX, true);
        
        //set global sp to the new address
        wasm_emit_assign_var(&func, STACK_POINTER_VAR_INDEX, true, 0, 0, fc->local_sp->var_index, false);
    }
    wasm_emit_code(cg, &func, node->func->body);
    if(stack_size){
        //adjustment back to original sp
        wasm_emit_assign_var(&func, STACK_POINTER_VAR_INDEX, true, OPCODE_I32ADD, stack_size, fc->local_sp->var_index, false);
    }

    ba_add(&func, OPCODE_END);
    //function body
    wasm_emit_uint(ba, func.size); //function body size
    ba_add2(ba, &func);
    ba_deinit(&func);

    _func_leave(cg, node);
}

