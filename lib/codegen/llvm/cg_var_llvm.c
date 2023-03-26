/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation for variable declarations/definitions
 */
#include <assert.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "codegen/llvm/cg_fun_llvm.h"
#include "codegen/llvm/cg_var_llvm.h"
#include "codegen/llvm/cg_llvm.h"
#include "codegen/fun_info.h"
#include "codegen/llvm/llvm_api.h"
#include "codegen/type_size_info.h"
#include "sema/type.h"
#include <llvm-c/Support.h>

void _store_struct_member_values(struct cg_llvm *cg, LLVMTypeRef alloca_type, LLVMValueRef alloca, struct ast_node *values)
{
    for (size_t i = 0; i < array_size(&values->adt_init->body->block->nodes); i++) {
        struct ast_node *arg = array_get_ptr(&values->adt_init->body->block->nodes, i);
        LLVMValueRef exp = emit_ir_code(cg, arg);
        LLVMValueRef member = LLVMBuildStructGEP2(cg->builder, alloca_type, alloca, (unsigned)i, "");
        LLVMBuildStore(cg->builder, exp, member);
    }
}

void _store_array_values(struct cg_llvm *cg, LLVMTypeRef alloca_type, LLVMValueRef alloca, struct ast_node *array_values)
{
    size_t element_count = array_size(&array_values->array_init->block->nodes);
    LLVMValueRef elm_value;
    LLVMValueRef zero = LLVMConstInt(LLVMInt32TypeInContext(cg->context), 0, false);
    LLVMValueRef indexes[2] = { zero, zero };
    for (size_t i = 0; i < element_count; i++) {
        struct ast_node *arg = array_get_ptr(&array_values->array_init->block->nodes, i);
        elm_value = emit_ir_code(cg, arg);
        indexes[1] = LLVMConstInt(LLVMInt32TypeInContext(cg->context), i, false);
        LLVMValueRef field = LLVMBuildInBoundsGEP2(cg->builder, alloca_type, alloca, indexes, 2, "");
        LLVMBuildStore(cg->builder, elm_value, field);
    }
}

LLVMValueRef emit_struct_init_node(struct cg_llvm *cg, struct ast_node *node, bool is_ret, const char *name)
{
    struct ast_node *parent_func = *(struct ast_node**)stack_top(&cg->base.sema_context->func_stack);
    struct ast_node *ft_node = parent_func->func->func_type;
    struct type_item *te = node->type;
    struct type_size_info tsi = get_type_size_info(te);
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, ft_node);
    bool is_rvo = check_rvo(fi);
    is_ret = is_ret || node->is_ret;
    LLVMValueRef alloca = 0;
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); // builder->GetInsertBlock()->getParent();
    if (is_rvo && is_ret) {
        assert(fi->tai.sret_arg_no != InvalidIndex);
        //function parameter with sret: just directly used the pointer passed
        alloca = LLVMGetParam(fun, fi->tai.sret_arg_no);
        LLVMTypeRef ret_type = get_llvm_type(fi->ret.type);
        assert(ret_type);
        _store_struct_member_values(cg, ret_type, alloca, node);
    } else {
        LLVMTypeRef type = (LLVMTypeRef)hashtable_get_p(&cg->typename_2_irtypes, te->name);
        assert(type);
        alloca = create_alloca(type, tsi.align_bits / 8, fun, name);
        _store_struct_member_values(cg, type, alloca, node);
    }
    return alloca;
}

LLVMValueRef emit_array_init_node(struct cg_llvm *cg, struct ast_node *node, bool is_ret, const char *name)
{
    struct ast_node *parent_func = *(struct ast_node**)stack_top(&cg->base.sema_context->func_stack);
    struct ast_node *ft_node = parent_func->func->func_type;
    struct type_item *te = node->type;
    struct type_size_info tsi = get_type_size_info(te);
    struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, ft_node);
    bool is_rvo = check_rvo(fi);
    is_ret = is_ret || node->is_ret;
    LLVMValueRef alloca = 0;
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); // builder->GetInsertBlock()->getParent();
    if (is_rvo && is_ret) {
        assert(fi->tai.sret_arg_no != InvalidIndex);
        //function parameter with sret: just directly used the pointer passed
        alloca = LLVMGetParam(fun, fi->tai.sret_arg_no);
        LLVMTypeRef ret_type = get_llvm_type(fi->ret.type);
        assert(ret_type);
        _store_array_values(cg, ret_type, alloca, node);
    } else {
        //local stack allocation
        LLVMTypeRef elm_type = get_llvm_type(te->val_type);
        LLVMTypeRef type = LLVMArrayType(elm_type, get_array_size(te));
        node->type->backend_type = type;
        // LLVMTypeRef type = (LLVMTypeRef)hashtable_get_p(&cg->typename_2_irtypes, te->name);
        // assert(type);
        alloca = create_alloca(type, tsi.align_bits / 8, fun, name);
        _store_array_values(cg, type, alloca, node);
    }
    return alloca;
}

LLVMValueRef _emit_local_var_struct_node(struct cg_llvm *cg, struct ast_node *node)
{
    symbol var_name = node->var->var->ident->name;
    LLVMValueRef alloca = 0;

    if (node->var->init_value->node_type == ADT_INIT_NODE) {
        assert(node->type->name == node->var->init_value->type->name);
        alloca = emit_struct_init_node(cg, node->var->init_value, node->is_ret, string_get(var_name));
    } else {
        alloca = emit_ir_code(cg, node->var->init_value);
        LLVMSetValueName2(alloca, string_get(var_name), string_size(var_name));
    }
    hashtable_set_p(&cg->varname_2_irvalues, var_name, alloca);
    /*TODO: local & global sharing the same hashtable now*/
    hashtable_set_p(&cg->varname_2_typename, var_name, node->type->name);
    return 0;
    // KSDbgInfo.emitLocation(this);
}


LLVMValueRef _emit_local_var_array_node(struct cg_llvm *cg, struct ast_node *node)
{
    symbol var_name = node->var->var->ident->name;
    LLVMValueRef alloca = 0;

    if (node->var->init_value->node_type == ARRAY_INIT_NODE) {
        assert(node->type->name == node->var->init_value->type->name);
        alloca = emit_array_init_node(cg, node->var->init_value, node->is_ret, string_get(var_name));
    } else {
        alloca = emit_ir_code(cg, node->var->init_value);
        LLVMSetValueName2(alloca, string_get(var_name), string_size(var_name));
    }
    if(node->var->init_value)
        node->type->backend_type = node->var->init_value->type->backend_type;
    hashtable_set_p(&cg->varname_2_irvalues, var_name, alloca);
    /*TODO: local & global sharing the same hashtable now*/
    hashtable_set_p(&cg->varname_2_typename, var_name, node->type->name);
    return 0;
    // KSDbgInfo.emitLocation(this);
}

LLVMValueRef _emit_local_var_node(struct cg_llvm *cg, struct ast_node *node)
{
    if (node->type->type == TYPE_STRUCT)
        return _emit_local_var_struct_node(cg, node);
    else if (node->type->type == TYPE_ARRAY)
        return _emit_local_var_array_node(cg, node);
    // fprintf(stderr, "_emit_var_node:1 %lu!, %lu\n", node->var_names.size(),
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); // builder->GetInsertBlock()->getParent();
    // fprintf(stderr, "_emit_var_node:2 %lu!\n", node->var_names.size());
    symbol var_name = node->var->var->ident->name;
    // log_info(DEBUG, "local var cg: %s", var_name.c_str());
    assert(node->var->init_value);
    LLVMValueRef init_val = emit_ir_code(cg, node->var->init_value);
    assert(init_val);
    enum type type = get_type(node->type);
    struct type_size_info tsi = get_type_size_info(node->type);
    LLVMValueRef alloca = create_alloca(cg->ops[type].get_type(cg->context, node->type), tsi.align_bits / 8, fun, string_get(var_name));
    LLVMBuildStore(cg->builder, init_val, alloca);
    hashtable_set_p(&cg->varname_2_irvalues, var_name, alloca);
    return 0;
    // KSDbgInfo.emitLocation(this);
}

LLVMValueRef _get_const_value_struct_init(struct cg_llvm *cg, LLVMTypeRef type, struct ast_node *struct_values)
{
    size_t element_count = array_size(&struct_values->adt_init->body->block->nodes);
    LLVMValueRef *values;
    MALLOC(values, element_count * sizeof(LLVMValueRef));
    for (size_t i = 0; i < element_count; i++) {
        struct ast_node *arg = array_get_ptr(&struct_values->adt_init->body->block->nodes, i);
        values[i] = emit_ir_code(cg, arg);
    }
    LLVMValueRef value = LLVMConstNamedStruct(type, values, (unsigned int)element_count);
    FREE(values);
    return value;
}

LLVMValueRef _get_zero_value_struct_type(struct cg_llvm *cg, LLVMTypeRef type, struct type_item *struct_type)
{
    size_t element_count = array_size(&struct_type->args);
    LLVMValueRef *values;
    MALLOC(values, element_count * sizeof(LLVMValueRef));
    for (size_t i = 0; i < element_count; i++) {
        enum type element_type = get_type(array_get_ptr(&struct_type->args, i));
        values[i] = cg->ops[element_type].get_zero(cg->context, cg->builder);
    }
    LLVMValueRef value = LLVMConstNamedStruct(type, values, (unsigned int)element_count);
    FREE(values);
    return value;
}

LLVMValueRef _get_const_array_value(struct cg_llvm *cg, LLVMTypeRef elm_type, struct ast_node *array_values)
{
    size_t element_count = array_size(&array_values->array_init->block->nodes);
    LLVMValueRef *values;
    MALLOC(values, element_count * sizeof(LLVMValueRef));
    for (size_t i = 0; i < element_count; i++) {
        struct ast_node *arg = array_get_ptr(&array_values->array_init->block->nodes, i);
        values[i] = emit_ir_code(cg, arg);
    }
    LLVMValueRef value = LLVMConstArray(elm_type, values, (unsigned int)element_count);
    FREE(values);
    return value;
}

LLVMValueRef _get_zero_value_array_type(struct cg_llvm *cg, LLVMTypeRef elm_type, struct type_item *array_type)
{
    size_t element_count = get_array_size(array_type);
    LLVMValueRef *values;
    MALLOC(values, element_count * sizeof(LLVMValueRef));
    enum type element_type = get_type(array_type->val_type);
    for (size_t i = 0; i < element_count; i++) {
        values[i] = cg->ops[element_type].get_zero(cg->context, cg->builder);
    }
    LLVMValueRef value = LLVMConstArray(elm_type, values, (unsigned int)element_count);
    FREE(values);
    return value;
}

LLVMValueRef _emit_global_var_struct_node(struct cg_llvm *cg, struct ast_node *node,
    bool is_external)
{
    const char *var_name = string_get(node->var->var->ident->name);
    LLVMValueRef gVar = LLVMGetNamedGlobal(cg->module, var_name);
    assert(node->type);
    LLVMTypeRef type = (LLVMTypeRef)hashtable_get_p(&cg->typename_2_irtypes, node->type->name);
    node->type->backend_type = type;
    assert(type);
    if (hashtable_in_p(&cg->cg_gvar_name_2_asts, node->var->var->ident->name) && !gVar && !is_external)
        is_external = true;
    if (!gVar) {
        if (is_external) {
            //global variable declare without initialization, initialized elsewhere
            gVar = LLVMAddGlobal(cg->module, type, var_name);
        } else {
            //global variable with initialization
            hashtable_set_p(&cg->cg_gvar_name_2_asts, node->var->var->ident->name, node);
            gVar = LLVMAddGlobal(cg->module, type, var_name);
            LLVMValueRef init_value;
            if (node->var->init_value)
                init_value = _get_const_value_struct_init(cg, type, node->var->init_value);
            else
                init_value = _get_zero_value_struct_type(cg, type, node->type);
            LLVMSetInitializer(gVar, init_value);
        }
    }
    hashtable_set_p(&cg->varname_2_typename, node->var->var->ident->name, node->type->name);
    if (!cg->base.sema_context->is_repl)
        return 0;
    //printf("node->init_value node type: %s\n", node_type_strings[node->init_value->node_type]);
    struct ast_node *values = node->var->init_value;
    char tempname[64];
    for (size_t i = 0; i < array_size(&values->adt_init->body->block->nodes); i++) {
        struct ast_node *arg = array_get_ptr(&values->adt_init->body->block->nodes, i);
        LLVMValueRef exp = emit_ir_code(cg, arg);
        sprintf_s(tempname, sizeof(tempname), "temp%zu", i);
        LLVMValueRef member = LLVMBuildStructGEP2(cg->builder, type, gVar, (unsigned int)i, tempname);
        LLVMBuildStore(cg->builder, exp, member);
    }
    return 0;
}

LLVMValueRef _emit_global_var_array_node(struct cg_llvm *cg, struct ast_node *node,
    bool is_external)
{
    const char *var_name = string_get(node->var->var->ident->name);
    LLVMValueRef gVar = LLVMGetNamedGlobal(cg->module, var_name);
    assert(node->type);
    LLVMTypeRef elm_type = get_llvm_type(node->type->val_type);
    LLVMTypeRef type = LLVMArrayType(elm_type, get_array_size(node->type));
    node->type->backend_type = type;
    assert(type);
    if (hashtable_in_p(&cg->cg_gvar_name_2_asts, node->var->var->ident->name) && !gVar && !is_external)
        is_external = true;
    if (!gVar) {
        if (is_external) {
            //global variable declare without initialization, initialized elsewhere
            gVar = LLVMAddGlobal(cg->module, type, var_name);
        } else {
            //global variable with initialization
            hashtable_set_p(&cg->cg_gvar_name_2_asts, node->var->var->ident->name, node);
            gVar = LLVMAddGlobal(cg->module, type, var_name);
            LLVMValueRef init_value;
            if (node->var->init_value)
                init_value = _get_const_array_value(cg, elm_type, node->var->init_value);
            else
                init_value = _get_zero_value_array_type(cg, elm_type, node->type);
            LLVMSetInitializer(gVar, init_value);
            // LLVMValueRef var = LLVMGetNamedGlobal(cg->module, var_name);
            // printf("hello: %p\n", (void*)var);
        }
    }
    hashtable_set_p(&cg->varname_2_typename, node->var->var->ident->name, node->type->name);
    if (!cg->base.sema_context->is_repl)
        return gVar;
    //printf("node->init_value node type: %s\n", node_type_strings[node->init_value->node_type]);
    struct ast_node *values = node->var->init_value;
    char tempname[64];
    LLVMValueRef zero = LLVMConstInt(LLVMInt32TypeInContext(cg->context), 0, false);
    LLVMValueRef indexes[2] = { zero, zero };
    for (size_t i = 0; i < array_size(&values->array_init->block->nodes); i++) {
        struct ast_node *arg = array_get_ptr(&values->array_init->block->nodes, i);
        LLVMValueRef expr = emit_ir_code(cg, arg);
        sprintf_s(tempname, sizeof(tempname), "temp%zu", i);
        indexes[1] = LLVMConstInt(LLVMInt32TypeInContext(cg->context), i, false);
        LLVMValueRef to_member = LLVMBuildInBoundsGEP2(cg->builder, type, gVar, indexes, 2, tempname);
        LLVMBuildStore(cg->builder, expr, to_member);
    }
    return 0;
}

LLVMValueRef _emit_global_var_node(struct cg_llvm *cg, struct ast_node *node,
    bool is_external)
{
    if (node->type->type == TYPE_STRUCT) {
        return _emit_global_var_struct_node(cg, node, is_external);
    } else if (node->type->type == TYPE_ARRAY){
        return _emit_global_var_array_node(cg, node, is_external);
    }
    const char *var_name = string_get(node->var->var->ident->name);
    LLVMValueRef gVar = LLVMGetNamedGlobal(cg->module, var_name);
    LLVMValueRef exp = emit_ir_code(cg, node->var->init_value);
    assert(node->type && cg->module);
    enum type type = get_type(node->type);
    if (hashtable_in_p(&cg->cg_gvar_name_2_asts, node->var->var->ident->name) && !gVar && !is_external)
        is_external = true;
    if (!gVar) {
        if (is_external) {
            gVar = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context, node->type), var_name);
            LLVMSetExternallyInitialized(gVar, true);
        } else {
            hashtable_set_p(&cg->cg_gvar_name_2_asts, node->var->var->ident->name, node);
            gVar = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context, node->type), var_name);
            LLVMSetExternallyInitialized(gVar, false);
            if (cg->base.sema_context->is_repl)
                // REPL treated as the global variable initialized as zero and
                // then updated with any expression
                LLVMSetInitializer(gVar, cg->ops[type].get_zero(cg->context, cg->builder));
            else {
                //TODO: We need to assert exp has to be a constant value
                LLVMSetInitializer(gVar, exp);
            }
        }
    }
    if (cg->base.sema_context->is_repl)
        LLVMBuildStore(cg->builder, exp, gVar);
    return 0;
}

LLVMValueRef emit_var_node(struct cg_llvm *cg, struct ast_node *node)
{
    if (node->var->is_global){
        return _emit_global_var_node(cg, node, false);
    }
    else{
        return _emit_local_var_node(cg, node);
    }
}

LLVMValueRef get_global_variable(struct cg_llvm *cg, symbol gv_name)
{
    const char *name = string_get(gv_name);
    LLVMValueRef gv = LLVMGetNamedGlobal(cg->module, name);
    if (gv)
        return gv;
    struct ast_node *var = hashtable_get_p(&cg->cg_gvar_name_2_asts, gv_name);
    if (var) {
        LLVMTypeRef type = var->type->backend_type? var->type->backend_type : get_llvm_type(var->type);
        gv = LLVMAddGlobal(cg->module, type, name);
        LLVMSetExternallyInitialized(gv, true);
        return gv;
    }
    return 0;
}
