/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation for variable declarations/definitions
 */
#include <assert.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "codegen/cg_fun.h"
#include "codegen/cg_var.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/ir_api.h"
#include "codegen/type_size_info.h"
#include "sema/type.h"
#include <llvm-c/Support.h>

void _store_type_values(struct code_generator *cg, LLVMValueRef alloca, struct ast_node *values)
{
    for (size_t i = 0; i < array_size(&values->type_value->body->block->nodes); i++) {
        struct ast_node *arg = *(struct ast_node **)array_get(&values->type_value->body->block->nodes, i);
        LLVMValueRef exp = emit_ir_code(cg, arg);
        LLVMValueRef member = LLVMBuildStructGEP(cg->builder, alloca, (unsigned)i, "");
        LLVMBuildStore(cg->builder, exp, member);
    }
}

LLVMValueRef emit_type_value_node(struct code_generator *cg, struct ast_node *type_values, bool is_ret, const char *name)
{
    struct ast_node *ft_node = (struct ast_node *)find_parent_proto(type_values);
    struct type_exp *te = type_values->type;
    LLVMTypeRef type = (LLVMTypeRef)hashtable_get_p(&cg->typename_2_irtypes, te->name);
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); // builder->GetInsertBlock()->getParent();
    struct type_size_info tsi = get_type_size_info(te);
    struct fun_info *fi = get_fun_info(ft_node);
    bool is_rvo = check_rvo(fi);
    is_ret = is_ret || type_values->is_ret;
    LLVMValueRef alloca = 0;
    if (is_rvo && is_ret) {
        assert(fi->iai.sret_arg_no != InvalidIndex);
        //function parameter with sret: just directly used the pointer passed
        alloca = LLVMGetParam(fun, fi->iai.sret_arg_no);
        _store_type_values(cg, alloca, type_values);
    } else {
        alloca = create_alloca(type, tsi.align_bits / 8, fun, name);
        _store_type_values(cg, alloca, type_values);
    }
    return alloca;
}

LLVMValueRef _emit_local_var_type_node(struct code_generator *cg, struct ast_node *node)
{
    symbol var_name = node->var->var_name;
    LLVMValueRef alloca = 0;

    if (node->var->init_value->node_type == TYPE_VALUE_NODE) {
        assert(node->type->name == node->var->init_value->type->name);
        //bool is_ret = node->base.is_ret;
        alloca = emit_type_value_node(cg, (struct ast_node *)node->var->init_value, node->is_ret, string_get(var_name));
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

LLVMValueRef _emit_local_var_node(struct code_generator *cg, struct ast_node *node)
{
    if (node->type->type == TYPE_EXT)
        return _emit_local_var_type_node(cg, node);
    // fprintf(stderr, "_emit_var_node:1 %lu!, %lu\n", node->var_names.size(),
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); // builder->GetInsertBlock()->getParent();
    // fprintf(stderr, "_emit_var_node:2 %lu!\n", node->var_names.size());
    symbol var_name = node->var->var_name;
    // log_info(DEBUG, "local var cg: %s", var_name.c_str());
    assert(node->var->init_value);
    LLVMValueRef init_val = emit_ir_code(cg, node->var->init_value);
    assert(init_val);
    enum type type = get_type(node->type);
    struct type_size_info tsi = get_type_size_info(node->type);
    LLVMValueRef alloca = create_alloca(cg->ops[type].get_type(cg->context, node->type), tsi.align_bits / 8, fun, string_get(var_name));
    LLVMBuildStore(cg->builder, init_val, alloca);
    hashtable_set_p(&cg->varname_2_irvalues, var_name, alloca);
    if (type == TYPE_EXT)
        assert(!node->is_ret);
    return 0;
    // KSDbgInfo.emitLocation(this);
}

LLVMValueRef _get_const_value_ext_type(struct code_generator *cg, LLVMTypeRef type, struct ast_node *struct_values)
{
    size_t element_count = array_size(&struct_values->type_value->body->block->nodes);
    LLVMValueRef *values;
    MALLOC(values, element_count * sizeof(LLVMValueRef));
    for (size_t i = 0; i < element_count; i++) {
        struct ast_node *arg = *(struct ast_node **)array_get(&struct_values->type_value->body->block->nodes, i);
        values[i] = emit_ir_code(cg, arg);
    }
    LLVMValueRef value = LLVMConstNamedStruct(type, values, (unsigned int)element_count);
    FREE(values);
    return value;
}

LLVMValueRef _get_zero_value_ext_type(struct code_generator *cg, LLVMTypeRef type, struct type_oper *type_ext)
{
    size_t element_count = array_size(&type_ext->args);
    LLVMValueRef *values;
    MALLOC(values, element_count * sizeof(LLVMValueRef));
    for (size_t i = 0; i < element_count; i++) {
        enum type element_type = get_type(*(struct type_exp **)array_get(&type_ext->args, i));
        //values[i] = LLVMConstReal(LLVMDoubleTypeInContext(cg->context), 10.0 * (i+1));
        values[i] = cg->ops[element_type].get_zero(cg->context, cg->builder);
    }
    LLVMValueRef value = LLVMConstNamedStruct(type, values, (unsigned int)element_count);
    FREE(values);
    return value;
}

LLVMValueRef _emit_global_var_type_node(struct code_generator *cg, struct ast_node *node,
    bool is_external)
{
    const char *var_name = string_get(node->var->var_name);
    LLVMValueRef gVar = LLVMGetNamedGlobal(cg->module, var_name);
    assert(node->type);
    LLVMTypeRef type = (LLVMTypeRef)hashtable_get_p(&cg->typename_2_irtypes, node->type->name);
    assert(type);
    if (hashtable_in_p(&cg->gvs, node->var->var_name) && !gVar && !is_external)
        is_external = true;
    if (!gVar) {
        if (is_external) {
            gVar = LLVMAddGlobal(cg->module, type, var_name);
        } else {
            hashtable_set_p(&cg->gvs, node->var->var_name, node);
            gVar = LLVMAddGlobal(cg->module, type, var_name);
            LLVMValueRef init_value;
            if (node->var->init_value)
                init_value = _get_const_value_ext_type(cg, type, (struct ast_node *)node->var->init_value);
            else
                init_value = _get_zero_value_ext_type(cg, type, (struct type_oper *)node->type);
            LLVMSetInitializer(gVar, init_value);
        }
    }
    hashtable_set_p(&cg->varname_2_typename, node->var->var_name, node->type->name);
    if (!cg->sema_context->parser->is_repl)
        return 0;
    //printf("node->init_value node type: %s\n", node_type_strings[node->init_value->node_type]);
    struct ast_node *values = (struct ast_node *)node->var->init_value;
    char tempname[64];
    for (size_t i = 0; i < array_size(&values->type_value->body->block->nodes); i++) {
        struct ast_node *arg = *(struct ast_node **)array_get(&values->type_value->body->block->nodes, i);
        LLVMValueRef exp = emit_ir_code(cg, arg);
        sprintf_s(tempname, sizeof(tempname), "temp%zu", i);
        LLVMValueRef member = LLVMBuildStructGEP(cg->builder, gVar, (unsigned int)i, tempname);
        LLVMBuildStore(cg->builder, exp, member);
    }
    return 0;
}

LLVMValueRef _emit_global_var_node(struct code_generator *cg, struct ast_node *node,
    bool is_external)
{
    if (node->type->type == TYPE_EXT) {
        return _emit_global_var_type_node(cg, node, is_external);
    }
    const char *var_name = string_get(node->var->var_name);
    LLVMValueRef gVar = LLVMGetNamedGlobal(cg->module, var_name);
    LLVMValueRef exp = emit_ir_code(cg, node->var->init_value);
    assert(node->type && cg->module);
    enum type type = get_type(node->type);
    if (hashtable_in_p(&cg->gvs, node->var->var_name) && !gVar && !is_external)
        is_external = true;
    if (!gVar) {
        if (is_external) {
            gVar = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context, node->type), var_name);
            LLVMSetExternallyInitialized(gVar, true);
        } else {
            hashtable_set_p(&cg->gvs, node->var->var_name, node);
            gVar = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context, node->type), var_name);
            LLVMSetExternallyInitialized(gVar, false);
            if (cg->sema_context->parser->is_repl)
                // REPL treated as the global variable initialized as zero and
                // then updated with any expression
                LLVMSetInitializer(gVar, cg->ops[type].get_zero(cg->context, cg->builder));
            else {
                //TODO: We need to assert exp has to be a constant value
                LLVMSetInitializer(gVar, exp);
            }
        }
    }
    if (cg->sema_context->parser->is_repl)
        LLVMBuildStore(cg->builder, exp, gVar);
    return 0;
}

LLVMValueRef emit_var_node(struct code_generator *cg, struct ast_node *node)
{
    struct ast_node *var = (struct ast_node *)node;
    if (!var->parent)
        return _emit_global_var_node(cg, var, false);
    else
        return _emit_local_var_node(cg, var);
}

LLVMValueRef get_global_variable(struct code_generator *cg, symbol gv_name)
{
    const char *name = string_get(gv_name);
    LLVMValueRef gv = LLVMGetNamedGlobal(cg->module, name);
    if (gv)
        return gv;
    struct ast_node *var = (struct ast_node *)hashtable_get_p(&cg->gvs, gv_name);
    if (var) {
        LLVMTypeRef type = get_llvm_type(var->type);
        gv = LLVMAddGlobal(cg->module, type, name);
        return gv;
    }
    return 0;
}
