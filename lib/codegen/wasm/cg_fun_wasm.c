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


struct var_info *_req_new_local_var(struct cg_wasm *cg, struct type_expr *type, bool is_local_var, bool is_ret, bool is_addressed)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    if (is_aggregate_type(type) && is_local_var && is_ret){
        return &cg->local_vars[0];//return first sret
    }
    u32 index = fc->local_vars++;
    if (!is_local_var) {
        fc->local_params++;
    }
    struct var_info *vi = &cg->local_vars[cg->var_top];
    vi->var_index = index;
    ASSERT_TYPE(type->type);
    vi->target_type = type_2_wtype[type->type];
    if ((is_aggregate_type(type) && is_local_var) || is_addressed){
        vi->alloc_index = fc_register_alloc(fc, type);
    }else{
        vi->alloc_index = -1;
    }
    cg->var_top++;
    return vi;
}

void collect_local_variables(struct cg_wasm *cg, struct ast_node *node)
{
    struct ast_node *arg_node;
    if(node->transformed) node = node->transformed;
    switch(node->node_type)
    {
        default:
            break;
        case IF_NODE:
            collect_local_variables(cg, node->cond->if_node);
            collect_local_variables(cg, node->cond->then_node);
            if(node->cond->else_node){
                collect_local_variables(cg, node->cond->else_node);
            }
            break;
        case ARRAY_INIT_NODE:
        case STRUCT_INIT_NODE:
            //only the parent node is needed
            func_register_local_variable(cg, node, true);
            break;
        case UNARY_NODE:
            collect_local_variables(cg, node->unop->operand);
            break;
        case ASSIGN_NODE:
        case BINARY_NODE:
            func_register_local_variable(cg, node, true);
            break;
        case MEMBER_INDEX_NODE:
            /*get the root ast_node*/
            node = get_root_object(node);
            collect_local_variables(cg, node);
            break;
        case FOR_NODE:
            func_register_local_variable(cg, node, true);
            collect_local_variables(cg, node->forloop->body);
            break;
        case VAR_NODE:
            func_register_local_variable(cg, node, true);
            if(node->var->init_value){
                collect_local_variables(cg, node->var->init_value);
            }
            break;
        case CALL_NODE:
            func_register_local_variable(cg, node, true);
            for(u32 i = 0; i < array_size(&node->call->arg_block->block->nodes); i++){
                arg_node = *(struct ast_node **)array_get(&node->call->arg_block->block->nodes, i);
                collect_local_variables(cg, arg_node);
                if(arg_node->type->type == TYPE_STRUCT && is_refered_later(arg_node)){
                    /*not for array type, array type is always reference type*/
                    /*struct type is value type, we need to make copy of it to prevent being changed by 
                     *callee
                    */
                    struct fun_context *fc = cg_get_top_fun_context(cg);
                    struct var_info *vi = fc_get_var_info(fc, arg_node);
                    if(vi->var_index>=fc->local_params){
                    //this is local variable
                    //only for lvalue, and local variable (not parameter)
                    //request a temp variable for copy-by-value struct pass style 
                    //to prevent callee from changing the argument
                        _req_new_local_var(cg, arg_node->type, true, arg_node->is_ret, true);
                    }
                }
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

struct type_expr *_create_type_for_call_with_optional_parameters(struct cg_wasm*cg, struct ast_node *node)
{
    symbol callee = node->call->specialized_callee ? node->call->specialized_callee : node->call->callee;
    struct ast_node *fun_type = hashtable_get_p(&cg->func_name_2_ast, callee);
    u32 param_num = array_size(&fun_type->ft->params->block->nodes);
    struct array arg_types;
    array_init(&arg_types, sizeof(struct type_expr *));
    for(u32 i = 0; i < array_size(&node->call->arg_block->block->nodes); i++){
        struct ast_node *arg = *(struct ast_node **)array_get(&node->call->arg_block->block->nodes, i);
        if (!fun_type->ft->is_variadic||i < param_num - 1) {
            continue;
        }
        array_push(&arg_types, &arg->type);
    }
    return create_type_oper_struct(0, Immutable, &arg_types);
}
/*
 * register local variable & stack space
 */ 
void func_register_local_variable(struct cg_wasm *cg, struct ast_node *node, bool is_local_var)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct var_info *vi;
    struct fun_info *fi;
    switch (node->node_type){
    default:
        break;
    case ARRAY_INIT_NODE:
    case STRUCT_INIT_NODE:
        vi = _req_new_local_var(cg, node->type, is_local_var, node->is_ret, node->is_addressed);
        hashtable_set_p(&fc->ast_2_index, node, vi);
        break;
    case  VAR_NODE:
        if(symboltable_get(&fc->varname_2_index, node->var->var->ident->name)){
            break;
        }
        vi = _req_new_local_var(cg, node->type, is_local_var, node->is_ret, node->is_addressed);
        hashtable_set_p(&fc->ast_2_index, node, vi);
        symboltable_push(&fc->varname_2_index, node->var->var->ident->name, vi);
        break;
    case ASSIGN_NODE:
    case BINARY_NODE:
        if(node->binop->lhs->type->type == TYPE_STRUCT){
            if(node->binop->lhs->node_type != IDENT_NODE){
                vi = _req_new_local_var(cg, node->binop->lhs->type, is_local_var, node->binop->lhs->is_ret, node->binop->lhs->is_addressed);
                hashtable_set_p(&fc->ast_2_index, node->binop->lhs, vi);
            }
            if(node->binop->rhs->node_type != IDENT_NODE){
                vi = _req_new_local_var(cg, node->binop->rhs->type, is_local_var, node->binop->rhs->is_ret, node->binop->rhs->is_addressed);
                hashtable_set_p(&fc->ast_2_index, node->binop->rhs, vi);
            }
        }
        break;
    case FOR_NODE:
        vi = _req_new_local_var(cg, node->forloop->range->range->start->type, is_local_var, node->forloop->range->range->start->is_ret, node->forloop->range->range->start->is_addressed);
        symboltable_push(&fc->varname_2_index, node->forloop->var->var->var->ident->name, vi);
        hashtable_set_p(&fc->ast_2_index, node->forloop->var, vi);
        vi = _req_new_local_var(cg, node->forloop->range->range->step->type, true, node->forloop->range->range->step->is_ret, node->forloop->range->range->step->is_addressed);
        hashtable_set_p(&fc->ast_2_index, node->forloop->range->range->step, vi);
        vi = _req_new_local_var(cg, node->forloop->range->range->end->binop->rhs->type, true, node->forloop->range->range->end->binop->rhs->is_ret, node->forloop->range->range->end->binop->rhs->is_addressed);
        hashtable_set_p(&fc->ast_2_index, node->forloop->range->range->end->binop->rhs, vi);
        break;
    case CALL_NODE:
        /*TODO: call node doesn't support both sret and varidic parameter*/
        /*for variadic function call, we might need one local variable*/
        fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, node->call->callee_func_type);
        bool has_optional_args = is_variadic_call_with_optional_arguments(cg, node);
        struct type_expr *te = 0;
        if(has_optional_args || fi_has_sret(fi)){
            te = has_optional_args ? _create_type_for_call_with_optional_parameters(cg, node) : fi->ret.type;
            vi = _req_new_local_var(cg, te, is_local_var, has_optional_args ? false : node->is_ret, false);
            hashtable_set_p(&fc->ast_2_index, node, vi);
        }
        if(has_optional_args){
            //TODO: fix memory leak, but we can't free here type_expr_free(te);
            //
        }
        break;
    }
}

void wasm_emit_func(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == FUNC_NODE);
    assert(node->type->kind == KIND_OPER);
    struct type_expr *to = node->type;
    assert(!is_generic(node->type));
    struct fun_context *fc = _func_enter(cg, node);
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, node->func->func_type);
    bool has_sret = fi_has_sret(fi);
    struct ast_node *p0 = 0;
    if(has_sret){
        struct ast_node *is_of_type = ident_node_new(fi->ret.type->name, node->loc);
        p0 = var_node_new(ident_node_new(to_symbol("__p0"), node->loc), is_of_type, 0, false, true, node->loc);
        p0->type = fi->ret.type;
        func_register_local_variable(cg, p0, false); //register one parameter
    }
    for(u32 i=0; i < array_size(&node->func->func_type->ft->params->block->nodes); i++){
        struct ast_node *param = *(struct ast_node **)array_get(&node->func->func_type->ft->params->block->nodes, i);
        param->type = (*(struct type_expr**)array_get(&to->args,i));
        func_register_local_variable(cg, param, false);
    }
    collect_local_variables(cg, node->func->body);
    u32 stack_size = fc_get_stack_size(fc);
    if(stack_size){
        //TODO: make builtin type as constant
        struct type_expr *to_sp = create_nullary_type(TYPE_INT);
        fc->local_sp = _req_new_local_var(cg, to_sp, true, false, false);
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
    //function body
    wasm_emit_code(cg, &func, node->func->body);
    if(stack_size){
        //adjustment back to original sp
        wasm_emit_assign_var(&func, STACK_POINTER_VAR_INDEX, true, OPCODE_I32ADD, stack_size, fc->local_sp->var_index, false);
    }
    //end of function
    ba_add(&func, OPCODE_END);
    wasm_emit_uint(ba, func.size); //function body size
    ba_add2(ba, &func);
    ba_deinit(&func);

    _func_leave(cg, node);
    node_free(p0);
}
