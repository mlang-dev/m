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


void fun_context_init(struct fun_context *fc)
{
    fc->fun_name = 0;
    fc->local_vars = 0;
    fc->local_params = 0;
    symboltable_init(&fc->varname_2_index);
    hashtable_init(&fc->ast_2_index);
}

void fun_context_deinit(struct fun_context *fc)
{
    hashtable_deinit(&fc->ast_2_index);
    symboltable_deinit(&fc->varname_2_index);
}

void _func_enter(struct cg_wasm *cg, symbol fun_name)
{
    struct fun_context *fc = &cg->fun_contexts[cg->fun_top];
    fun_context_init(fc);
    fc->fun_name = fun_name;
    cg->fun_top ++;
}

void _func_leave(struct cg_wasm *cg, symbol fun_name)
{
    cg->fun_top--;
    struct fun_context *fc = &cg->fun_contexts[cg->fun_top];
    cg->var_top -= fc->local_vars;
    fun_context_deinit(fc);
    assert(cg->fun_contexts[cg->fun_top].fun_name == fun_name);
}


struct fun_context *_fun_context_top(struct cg_wasm *cg)
{
    return cg->fun_top >= 1 ? &cg->fun_contexts[cg->fun_top - 1] : 0;
}

u32 func_get_local_var_index(struct cg_wasm *cg, struct ast_node *node)
{
    struct fun_context *fc = _fun_context_top(cg);
    struct var_info *vi = (struct var_info *)hashtable_get_p(&fc->ast_2_index, node);
    assert(vi);
    return vi->index;
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
    struct fun_context *fc = _fun_context_top(cg);
    return fc->local_vars - fc->local_params;
}

struct var_info *_req_new_local_var(struct cg_wasm *cg, enum type type, bool is_local_var)
{
    struct fun_context *fc = _fun_context_top(cg);
    u32 index = fc->local_vars++;
    if (!is_local_var) {
        fc->local_params++;
    }
    struct var_info *vi = &cg->local_vars[cg->var_top];
    vi->index = index;
    ASSERT_TYPE(type);
    vi->type = type_2_wtype[type];
    cg->var_top++;
    return vi;
}

void func_register_local_variable(struct cg_wasm *cg, struct ast_node *node, enum type type, bool is_local_var)
{
    struct fun_context *fc = _fun_context_top(cg);
    struct var_info *vi = _req_new_local_var(cg, type, is_local_var);
    if (node->node_type == VAR_NODE) {
        symboltable_push(&fc->varname_2_index, node->var->var_name, vi);
    } else if (node->node_type == FOR_NODE) {
        symboltable_push(&fc->varname_2_index, node->forloop->var_name, vi);
        vi = _req_new_local_var(cg, node->forloop->step->type->type, true);
        hashtable_set_p(&fc->ast_2_index, node->forloop->step, vi);
        vi = _req_new_local_var(cg, node->forloop->end->type->type, true);
        hashtable_set_p(&fc->ast_2_index, node->forloop->end, vi);
    } else if (node->node_type == CALL_NODE) {
        hashtable_set_p(&fc->ast_2_index, node, vi);
    }
}

u32 func_context_get_var_index(struct cg_wasm *cg, symbol var_name)
{
    struct fun_context *fc = _fun_context_top(cg);
    struct var_info *vi = symboltable_get(&fc->varname_2_index, var_name);
    assert(vi);
    return vi->index;
}

void wasm_emit_func(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == FUNC_NODE);
    _func_enter(cg, node->func->func_type->ft->name);
    assert(node->type->kind == KIND_OPER);
    struct type_oper *to = (struct type_oper *)node->type;
    for(u32 i=0; i < array_size(&node->func->func_type->ft->params->block->nodes); i++){
        struct ast_node *param = *(struct ast_node **)array_get(&node->func->func_type->ft->params->block->nodes, i);
        func_register_local_variable(cg, param, (*(struct type_expr**)array_get(&to->args,i))->type, false);
    }
    collect_local_variables(cg, node->func->body);
    struct byte_array func;
    ba_init(&func, 17);
    u32 local_vars = _func_get_local_var_nums(cg);
    wasm_emit_uint(&func, local_vars); // num local variables
    u32 start_pos = cg->var_top - local_vars;
    for(u32 i = 0; i < local_vars; i++){
        wasm_emit_uint(&func, 1); // num local following types
        ba_add(&func, cg->local_vars[start_pos + i].type);
    }
    wasm_emit_code(cg, &func, node->func->body);
    ba_add(&func, OPCODE_END);
    //function body
    wasm_emit_uint(ba, func.size); //function body size
    ba_add2(ba, &func);
    ba_deinit(&func);
    _func_leave(cg, node->func->func_type->ft->name);
}
