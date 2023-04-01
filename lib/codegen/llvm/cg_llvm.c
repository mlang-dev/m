/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation Functions
 */
#include <assert.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "codegen/llvm/cg_call_llvm.h"
#include "codegen/llvm/cg_fun_llvm.h"
#include "codegen/llvm/cg_var_llvm.h"
#include "codegen/llvm/cg_llvm.h"
#include "codegen/fun_info.h"
#include "codegen/llvm/llvm_api.h"
#include "codegen/llvm/x86_64_abi.h"
#include "codegen/llvm/winx86_64_abi.h"
#include "sema/type_size_info.h"
#include "sema/type.h"
#include "sema/eval.h"
#include <llvm-c/Support.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Core.h>
#include "parser/astdump.h"


struct cg_llvm *g_cg = 0;

LLVMContextRef get_llvm_context()
{
    if (!g_cg)
        return 0;
    return g_cg->context;
}

LLVMTypeRef get_int_type(LLVMContextRef context, struct type_item *type)
{
    (void)type;
    return LLVMInt32TypeInContext(context);
}

LLVMTypeRef get_char_type(LLVMContextRef context, struct type_item *type)
{
    (void)type;
    return LLVMInt8TypeInContext(context);
}

LLVMTypeRef get_bool_type(LLVMContextRef context, struct type_item *type)
{
    (void)type;
    return LLVMInt1TypeInContext(context);
}

LLVMTypeRef get_double_type(LLVMContextRef context, struct type_item *type)
{
    (void)type;
    return LLVMDoubleTypeInContext(context);
}

LLVMTypeRef _create_struct_backend_type(LLVMContextRef context, struct type_item *type_exp)
{
    assert(is_aggregate_type(type_exp));
    assert(g_cg);
    LLVMTypeRef struct_type = hashtable_get_p(&g_cg->typename_2_irtypes, type_exp->name);
    if (struct_type)
        return struct_type;
    struct_type = LLVMStructCreateNamed(context, string_get(type_exp->name));
    unsigned member_count = (unsigned)array_size(&type_exp->args);
    LLVMTypeRef *members;
    MALLOC(members, member_count * sizeof(LLVMTypeRef));
    for (unsigned i = 0; i < member_count; i++) {
        struct type_item *field_type = array_get_ptr(&type_exp->args, i);
        members[i] = get_backend_type(field_type);
    }
    LLVMStructSetBody(struct_type, members, member_count, false);
    hashtable_set_p(&g_cg->typename_2_irtypes, type_exp->name, struct_type);
    FREE(members);
    return struct_type;
}

LLVMTypeRef get_str_type(LLVMContextRef context, struct type_item *type)
{
    (void)type;
    return LLVMPointerType(LLVMInt8TypeInContext(context), 0);
}

const char *buiiltin_funs[] = {
    "llvm.sin",
    "llvm.cos",
    "llvm.sqrt",
};

LLVMValueRef get_int_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void)builder;
    return LLVMConstInt(get_int_type(context, 0), *(int *)value, true);
}

LLVMValueRef get_bool_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void)builder;
    return LLVMConstInt(get_bool_type(context, 0), *(int *)value, true);
}

LLVMValueRef get_char_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void)builder;
    return LLVMConstInt(get_char_type(context, 0), *(char *)value, true);
}

LLVMValueRef get_double_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void)builder;
    return LLVMConstReal(get_double_type(context, 0), *(f64 *)value);
}

LLVMValueRef f_cmp(LLVMBuilderRef builder, int op,
                           LLVMValueRef lhs, LLVMValueRef rhs,
                           const char *name)
{
    return LLVMBuildFCmp(builder, op, lhs, rhs, name);
}

LLVMValueRef i_cmp(LLVMBuilderRef builder, int op,
                           LLVMValueRef lhs, LLVMValueRef rhs,
                           const char *name)
{
    return LLVMBuildICmp(builder, op, lhs, rhs, name);
}

LLVMValueRef get_str_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    const char *str = (const char *)value;

    // implementation of LLVMBuildGlobalString, except of way of getting module
    unsigned size = (unsigned)strlen(str);
    LLVMValueRef str_const = LLVMConstStringInContext(context, str, size, 0);
    LLVMTypeRef str_type = LLVMTypeOf(str_const);
    LLVMValueRef str_value = LLVMAddGlobal(get_llvm_module(), str_type, "");
    LLVMSetInitializer(str_value, str_const);
    LLVMSetGlobalConstant(str_value, true);
    LLVMSetLinkage(str_value, LLVMPrivateLinkage);
    LLVMSetUnnamedAddr(str_value, true);
    LLVMSetAlignment(str_value, 1);

    // converting GlobalVariable to a pointer
    LLVMValueRef zero = LLVMConstInt(LLVMInt32TypeInContext(context), 0, false);
    LLVMValueRef indexes[2] = { zero, zero };
    return LLVMBuildInBoundsGEP2(builder, str_type, str_value, indexes, 2, "");
}

LLVMValueRef get_int_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstInt(get_int_type(context, 0), 0, true);
}

LLVMValueRef get_bool_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstInt(get_bool_type(context, 0), 0, true);
}

LLVMValueRef get_char_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstInt(get_char_type(context, 0), 0, true);
}

LLVMValueRef get_double_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstReal(get_double_type(context, 0), 0.0);
}

LLVMValueRef get_str_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return get_str_const(context, builder, "");
}

LLVMValueRef get_int_one(LLVMContextRef context)
{
    return LLVMConstInt(get_int_type(context, 0), 1, true);
}

LLVMValueRef get_bool_one(LLVMContextRef context)
{
    return LLVMConstInt(get_bool_type(context, 0), 1, true);
}

LLVMValueRef get_char_one(LLVMContextRef context)
{
    return LLVMConstInt(get_char_type(context, 0), 1, true);
}

LLVMValueRef get_double_one(LLVMContextRef context)
{
    return LLVMConstReal(get_double_type(context, 0), 1.0);
}

struct ops null_ops = { 0 };

struct ops bool_ops = {
    get_bool_type,
    get_bool_const,
    get_bool_zero,
    get_bool_one,
    LLVMBuildAdd,
    LLVMBuildSub,
    LLVMBuildMul,
    LLVMBuildSDiv,
    LLVMBuildSRem,
    i_cmp,
    LLVMIntULT,
    LLVMIntUGT,
    LLVMIntEQ,
    LLVMIntNE,
    LLVMIntULE,
    LLVMIntUGE,
    LLVMBuildOr,
    LLVMBuildAnd,
    LLVMBuildNot,
    LLVMBuildNeg,
};

struct ops char_ops = {
    get_char_type,
    get_char_const,
    get_char_zero,
    get_char_one,
    LLVMBuildAdd,
    LLVMBuildSub,
    LLVMBuildMul,
    LLVMBuildSDiv,
    LLVMBuildSRem,
    i_cmp,
    LLVMIntSLT,
    LLVMIntSGT,
    LLVMIntEQ,
    LLVMIntNE,
    LLVMIntSLE,
    LLVMIntSGE,
    LLVMBuildOr,
    LLVMBuildAnd,
    LLVMBuildNot,
    LLVMBuildNeg,
};

struct ops int_ops = {
    get_int_type,
    get_int_const,
    get_int_zero,
    get_int_one,
    LLVMBuildAdd,
    LLVMBuildSub,
    LLVMBuildMul,
    LLVMBuildSDiv,
    LLVMBuildSRem,
    i_cmp,
    LLVMIntSLT,
    LLVMIntSGT,
    LLVMIntEQ,
    LLVMIntNE,
    LLVMIntSLE,
    LLVMIntSGE,
    LLVMBuildOr,
    LLVMBuildAnd,
    LLVMBuildNot,
    LLVMBuildNeg,
};

struct ops str_ops = {
    get_str_type,
    get_str_const,
    get_str_zero,
    get_int_one,
    LLVMBuildAdd,
    LLVMBuildSub,
    LLVMBuildMul,
    LLVMBuildSDiv,
    LLVMBuildSRem,
    i_cmp,
    LLVMIntSLT,
    LLVMIntSGT,
    LLVMIntEQ,
    LLVMIntNE,
    LLVMIntSLE,
    LLVMIntSGE,
    LLVMBuildOr,
    LLVMBuildAnd,
    LLVMBuildNot,
    LLVMBuildNeg,
};

struct ops double_ops = {
    get_double_type,
    get_double_const,
    get_double_zero,
    get_double_one,
    LLVMBuildFAdd,
    LLVMBuildFSub,
    LLVMBuildFMul,
    LLVMBuildFDiv,
    LLVMBuildFRem,
    f_cmp,
    LLVMRealULT,
    LLVMRealUGT,
    LLVMRealUEQ,
    LLVMRealUNE,
    LLVMRealULE,
    LLVMRealUGE,
    LLVMBuildOr,
    LLVMBuildAnd,
    LLVMBuildNot,
    LLVMBuildNeg,
};

struct ops aggr_ops = {
    _create_struct_backend_type,
    get_double_const,
    get_int_zero,
    get_int_one,
    LLVMBuildFAdd,
    LLVMBuildFSub,
    LLVMBuildFMul,
    LLVMBuildFDiv,
    LLVMBuildFRem,
    i_cmp,
    LLVMRealULT,
    LLVMRealUGT,
    LLVMRealUEQ,
    LLVMRealUNE,
    LLVMRealULE,
    LLVMRealUGE,
    LLVMBuildOr,
    LLVMBuildAnd,
    LLVMBuildNot,
    LLVMBuildNeg,
};

void _set_bin_ops(struct cg_llvm *cg)
{
    cg->ops[TYPE_NULL] = double_ops;
    cg->ops[TYPE_GENERIC] = double_ops;
    cg->ops[TYPE_UNIT] = double_ops;
    cg->ops[TYPE_BOOL] = bool_ops;
    cg->ops[TYPE_CHAR] = char_ops;
    cg->ops[TYPE_INT] = int_ops;
    cg->ops[TYPE_F64] = double_ops;
    cg->ops[TYPE_STRING] = str_ops;
    cg->ops[TYPE_FUNCTION] = double_ops;
    cg->ops[TYPE_STRUCT] = aggr_ops;
    cg->ops[TYPE_TUPLE] = aggr_ops;
    cg->ops[TYPE_VARIANT] = aggr_ops;
    cg->ops[TYPE_ARRAY] = aggr_ops;
}

unsigned _get_count_struct_element_types(TargetType type)
{
    return LLVMCountStructElementTypes(type);
}

void _fill_struct_fields_llvm(struct array *fields, TargetType struct_type)
{
    for (unsigned j = 0; j < LLVMCountStructElementTypes(struct_type); ++j) {
        LLVMTypeRef field_type = LLVMStructGetTypeAtIndex(struct_type, j);
        array_push(fields, &field_type);
    }
}

TargetType _get_function_type_llvm(TargetType ret_type, TargetType *param_types, unsigned param_count, bool is_variadic)
{
    return LLVMFunctionType(ret_type, (LLVMTypeRef*)param_types, param_count, is_variadic);
}

TargetType _get_target_type_llvm(struct type_item *type)
{
    return get_backend_type(type);
}

TargetType _get_pointer_type_llvm(TargetType type)
{
    return LLVMPointerType(type, 0);
}

TargetType _get_size_int_type_llvm(unsigned width)
{
    return LLVMIntTypeInContext(get_llvm_context(), width);
}


void _init_target_info_llvm(struct target_info *ti)
{
    ti->extend_type = LLVMInt8TypeInContext(get_llvm_context()); //would use 32 bits
    ti->get_size_int_type = _get_size_int_type_llvm;//LLVMIntTypeInContext(get_llvm_context(), width)
    ti->get_pointer_type = _get_pointer_type_llvm; //LLVMPointerType(get_backend_type(fi->ret.type), 0)
    ti->get_target_type = _get_target_type_llvm; //get_backend_type(fi->ret.type)
    ti->get_function_type = _get_function_type_llvm;
    ti->fill_struct_fields = _fill_struct_fields_llvm;//
    ti->get_count_struct_element_types = _get_count_struct_element_types; //LLVMCountStructElementTypes
    ti->void_type = LLVMVoidTypeInContext(get_llvm_context());
}

struct cg_llvm *llvm_cg_new(struct sema_context *sema_context)
{
    LLVMContextRef context = LLVMContextCreate();
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    struct cg_llvm *cg;
    MALLOC(cg, sizeof(*cg));
    cg->base.sema_context = sema_context;
    cg->context = context;
    cg->builder = LLVMCreateBuilderInContext(context);
    cg->module = 0;
    cg->target_machine = 0;
    cg->target_data = 0;
    cg->current_loop_block = -1;
    _set_bin_ops(cg);
    hashtable_init(&cg->cg_gvar_name_2_asts);
    hashtable_init(&cg->varname_2_irvalues);
    hashtable_init(&cg->typename_2_irtypes);
    hashtable_init(&cg->varname_2_typename);
    const char *target_triple = LLVMGetDefaultTargetTriple();
    cg->base.target_info = ti_new(sema_context->tc, target_triple);
    free((void*)target_triple);
    g_cg = cg;
    _init_target_info_llvm(cg->base.target_info);
    if (get_os() == OS_WIN32){
        cg->base.compute_fun_info = winx86_64_compute_fun_info;
    }else{
        cg->base.compute_fun_info = x86_64_compute_fun_info;
    }
    return cg;
}

void llvm_cg_free(struct cg_llvm *cg)
{
    if (cg->module){
        LLVMDisposeModule(cg->module);
    }
    if (cg->target_machine){
        LLVMDisposeTargetMachine(cg->target_machine);
    }
    if (cg->target_data){
        LLVMDisposeTargetData(cg->target_data);
    }
    LLVMDisposeBuilder(cg->builder);
    LLVMContextDispose(cg->context);
    ti_free(cg->base.target_info);
    hashtable_deinit(&cg->cg_gvar_name_2_asts);
    hashtable_deinit(&cg->varname_2_irvalues);
    hashtable_deinit(&cg->typename_2_irtypes);
    hashtable_deinit(&cg->varname_2_typename);
    FREE(cg);
    g_cg = 0;
    LLVMShutdown();
}

void delete_current_module(struct cg_llvm *cg)
{
    if (cg->module){
        LLVMDisposeModule(cg->module);
        cg->module = 0;
    }
    if (cg->target_machine){
        LLVMDisposeTargetMachine(cg->target_machine);
        cg->target_machine = 0;
    }
    if (cg->target_data){
        LLVMDisposeTargetData(cg->target_data);
        cg->target_data = 0;
    }
}

LLVMTypeRef _get_llvm_type(struct cg_llvm *cg, struct type_item *type)
{
    struct type_context *tc = cg->base.sema_context->tc;
    enum type en_type = get_type(tc, type);
    if(is_prime_type(type->type))
        return cg->ops[en_type].get_type(cg->context, type);
    else if(type->type == TYPE_ARRAY){
        LLVMTypeRef elm_type = get_backend_type(type->val_type);
        return LLVMArrayType(elm_type, get_array_size(type));
    } else if(is_struct_like_type(type)){
        return _create_struct_backend_type(cg->context, type);
    } else if(type->type == TYPE_FUNCTION){
        struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, type);
        assert(fi);
        return create_target_fun_type(cg->base.target_info, fi);
    }
    assert(false);
    return 0;
}

LLVMValueRef _emit_block_node(struct cg_llvm *cg, struct ast_node *node)
{
    LLVMValueRef codegen = 0;
    for (size_t i = 0; i < array_size(&node->block->nodes); i++) {
        struct ast_node *exp = array_get_ptr(&node->block->nodes, i);
        codegen = emit_ir_code(cg, exp);
    }
    return codegen;
}

LLVMValueRef _emit_literal_node(struct cg_llvm *cg, struct ast_node *node)
{
    assert(node->type);
    assert(node->node_type == LITERAL_NODE);
    struct type_context *tc = cg->base.sema_context->tc;
    enum type type = get_type(tc, node->type);
    void *value = 0;
    if (type == TYPE_CHAR)
        value = &node->liter->int_val;
    else if (is_int_type(type))
        value = &node->liter->int_val;
    else if (type == TYPE_F64)
        value = &node->liter->double_val;
    else if (type == TYPE_STRING) {
        value = (void *)node->liter->str_val;
    }
    return cg->ops[type].get_const(cg->context, cg->builder, value);
}

/*xy->TypeNode*/
LLVMValueRef _emit_ident_node(struct cg_llvm *cg, struct ast_node *node)
{
    // const char *idname = string_get(id);
    LLVMValueRef v = (LLVMValueRef)hashtable_get_p(&cg->varname_2_irvalues, node->ident->name);
    if (!v) {
        v = get_global_variable(cg, node->ident->name);
        assert(v);
    }
    LLVMTypeRef type = get_backend_type(node->type);
    if (node->is_lvalue || is_aggregate_type(node->type)){
        return v;
    }
    return LLVMBuildLoad2(cg->builder, type, v, string_get(node->ident->name));
}

LLVMValueRef _emit_assign_node(struct cg_llvm *cg, struct ast_node *node)
{
    LLVMValueRef assignee = emit_ir_code(cg, node->binop->lhs);
    LLVMValueRef expr = emit_ir_code(cg, node->binop->rhs);
    return LLVMBuildStore(cg->builder, expr, assignee);
}

LLVMValueRef _emit_unary_node(struct cg_llvm *cg, struct ast_node *node)
{
    LLVMValueRef operand_v = emit_ir_code(cg, node->unop->operand);
    assert(operand_v);
    if (node->unop->opcode == OP_PLUS)
        return operand_v;
    else if (node->unop->opcode == OP_MINUS) {
        return cg->ops->neg_op(cg->builder, operand_v, "negtmp");
    } else if (node->unop->opcode == OP_NOT) {
        LLVMValueRef ret = cg->ops->not_op(cg->builder, operand_v, "nottmp");
        return LLVMBuildZExt(cg->builder, ret, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
    }
    string fname;
    string_init_chars(&fname, "unary");
    string_add_chars(&fname, get_opcode(node->unop->opcode));
    symbol op = to_symbol(string_get(&fname));
    LLVMValueRef fun = get_llvm_function(cg, op);
    if (fun == 0)
        return log_info(ERROR, "Unknown unary operator");

    // KSDbgInfo.emitLocation(this);
    return LLVMBuildCall2(cg->builder, LLVMGetElementType(LLVMTypeOf(fun)), fun, &operand_v, 1, "unop");
}

LLVMValueRef _emit_array_index(struct cg_llvm *cg, struct ast_node *node)
{
    int index = eval(node->index->index);
    LLVMValueRef obj = emit_ir_code(cg, node->index->object);
    LLVMValueRef zero = LLVMConstInt(LLVMInt32TypeInContext(cg->context), 0, false);
    LLVMValueRef index_value = LLVMConstInt(LLVMInt32TypeInContext(cg->context), index, false);
    LLVMValueRef indexes[2] = { zero, index_value };
    LLVMValueRef v = LLVMBuildInBoundsGEP2(cg->builder, get_backend_type(node->index->object->type), obj, indexes, 2, "");
    if(node->is_lvalue){
        return v;
    }
    return LLVMBuildLoad2(cg->builder, get_backend_type(node->type), v, "");
}

int _get_member_index(struct cg_llvm *cg, symbol struct_name, symbol field_name)
{
    struct ast_node *type_item_node = hashtable_get_p(&cg->base.sema_context->struct_typename_2_asts, struct_name);
    return find_member_index(type_item_node, field_name);
}

LLVMValueRef _emit_field_access_node(struct cg_llvm *cg, struct ast_node *node)
{
    LLVMValueRef v = emit_ir_code(cg, node->index->object);
    LLVMTypeRef struct_type = get_backend_type(node->index->object->type);
    int index = 0;
    if(node->index->object->type->type == TYPE_STRUCT){
        index = _get_member_index(cg, node->index->object->type->name, node->index->index->ident->name);
    } else {
        index = eval(node->index->index);
    }
    v = LLVMBuildStructGEP2(cg->builder, struct_type, v, index, "");
    if (node->is_lvalue || is_aggregate_type(node->type)) {
        return v;
    } 
    LLVMTypeRef vt = get_backend_type(node->type);
    assert(vt);
    return LLVMBuildLoad2(cg->builder, vt, v, "");
}

LLVMValueRef _emit_binary_node(struct cg_llvm *cg, struct ast_node *node)
{
    struct type_context *tc = cg->base.sema_context->tc;
    LLVMValueRef lv = emit_ir_code(cg, node->binop->lhs);
    LLVMValueRef rv = emit_ir_code(cg, node->binop->rhs);
    // assert(LLVMGetValueKind(lv) == LLVMGetValueKind(rv));
    assert(node->binop->lhs->type && prune(tc, node->binop->lhs->type)->type == prune(tc, node->binop->rhs->type)->type);
    assert(lv && rv);
    assert(LLVMTypeOf(lv) == LLVMTypeOf(rv));
    struct ops *ops = &cg->ops[prune(tc, node->binop->lhs->type)->type];
    string f_name;
    switch(node->binop->opcode){
        case OP_PLUS:
            return ops->add(cg->builder, lv, rv, "");
        case OP_MINUS:
            return ops->sub(cg->builder, lv, rv, "");
        case OP_STAR:
            return ops->mul(cg->builder, lv, rv, "");
        case OP_DIVISION:
            return ops->div(cg->builder, lv, rv, "");
        case OP_MODULUS:
            return ops->rem(cg->builder, lv, rv, "");
        case OP_LT:
            lv = ops->cmp(cg->builder, ops->cmp_lt, lv, rv, "cmplttmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        case OP_GT:
            lv = ops->cmp(cg->builder, ops->cmp_gt, lv, rv, "cmpgttmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        case OP_EQ:
            lv = ops->cmp(cg->builder, ops->cmp_eq, lv, rv, "cmpeqtmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        case OP_NE:
            lv = ops->cmp(cg->builder, ops->cmp_neq, lv, rv, "cmpneqtmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        case OP_LE:
            lv = ops->cmp(cg->builder, ops->cmp_le, lv, rv, "cmpletmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        case OP_GE:
            lv = ops->cmp(cg->builder, ops->cmp_ge, lv, rv, "cmpgetmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        case OP_OR:
            lv = ops->or_op(cg->builder, lv, rv, "ortmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        case OP_AND:
            lv = ops->and_op(cg->builder, lv, rv, "andtmp");
            lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
            return lv;
        default:
            string_init_chars(&f_name, "binary");
            string_add_chars(&f_name, get_opcode(node->binop->opcode));
            symbol op = to_symbol(string_get(&f_name));
            LLVMValueRef fun = get_llvm_function(cg, op);
            assert(fun && "binary operator not found!");
            LLVMValueRef lrv[2] = { lv, rv };
            return LLVMBuildCall2(cg->builder, LLVMGetElementType(LLVMTypeOf(fun)), fun, lrv, 2, "binop");
    }
}

LLVMValueRef _emit_condition_node(struct cg_llvm *cg, struct ast_node *node)
{
    struct type_context *tc = cg->base.sema_context->tc;
    // KSDbgInfo.emitLocation(this);
    LLVMValueRef cond_v = emit_ir_code(cg, node->cond->if_node);
    assert(cond_v);

    cond_v = LLVMBuildICmp(cg->builder, LLVMIntNE, cond_v, cg->ops[TYPE_INT].get_zero(cg->context, cg->builder), "ifcond");

    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder));
    bool has_else = node->cond->else_node != 0;
    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "then");
    LLVMBasicBlockRef else_bb = 0;
    if(has_else) else_bb = LLVMCreateBasicBlockInContext(cg->context, "else");
    LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(cg->context, "ifcont");

    LLVMBuildCondBr(cg->builder, cond_v, then_bb, else_bb ? else_bb : merge_bb);
    //set insert point of the block, move the insertion point to be at the end of the block
    //to build then block
    LLVMPositionBuilderAtEnd(cg->builder, then_bb); 

    LLVMValueRef then_v = emit_ir_code(cg, node->cond->then_node);

    assert(then_v);
    //after then block is done, emit unconditional jump to merge BB, if then_v is not a control flow instruction
    struct ast_node *then_last = node->cond->then_node->node_type == BLOCK_NODE ? array_back_ptr(&node->cond->then_node->block->nodes) : node->cond->then_node;
    if(then_last->node_type != JUMP_NODE)
        LLVMBuildBr(cg->builder, merge_bb);
    //we might have changed then-block by then blocking emitting, let's regain/refresh then block
    then_bb = LLVMGetInsertBlock(cg->builder);

    LLVMValueRef else_v = 0;
    if(has_else){
        //add else block and proceeds similarly like then block 
        LLVMAppendExistingBasicBlock(fun, else_bb);
        LLVMPositionBuilderAtEnd(cg->builder, else_bb);
        else_v = emit_ir_code(cg, node->cond->else_node);
        assert(else_v);
        struct ast_node *else_last = node->cond->else_node->node_type == BLOCK_NODE ? array_back_ptr(&node->cond->else_node->block->nodes) : node->cond->else_node;
        if(else_last->node_type != JUMP_NODE)
            LLVMBuildBr(cg->builder, merge_bb);
        else_bb = LLVMGetInsertBlock(cg->builder);
    }
    LLVMAppendExistingBasicBlock(fun, merge_bb);
    LLVMPositionBuilderAtEnd(cg->builder, merge_bb);
    if(has_else){
        enum type type = get_type(tc, node->cond->then_node->type);
        LLVMValueRef phi_node = LLVMBuildPhi(cg->builder, cg->ops[type].get_type(cg->context, node->cond->then_node->type), "iftmp");
        LLVMAddIncoming(phi_node, &then_v, &then_bb, 1);
        LLVMAddIncoming(phi_node, &else_v, &else_bb, 1);
        return phi_node;
    }
    return cond_v;
}

LLVMValueRef _emit_jump_node(struct cg_llvm *cg, struct ast_node *node)
{
    if(node->jump->token_type == TOKEN_BREAK){
        return LLVMBuildBr(cg->builder, cg->loop_blocks[cg->current_loop_block].end_bb);
    }
    else if(node->jump->token_type == TOKEN_CONTINUE){
        return LLVMBuildBr(cg->builder, cg->loop_blocks[cg->current_loop_block].cont_bb);
    }
    else if(node->jump->token_type == TOKEN_RETURN){
        return LLVMBuildRet(cg->builder, emit_ir_code(cg, node->jump->expr));
    }
    return 0;
}

LLVMValueRef _emit_while_node(struct cg_llvm *cg, struct ast_node *node)
{
    LLVMBasicBlockRef bb = LLVMGetInsertBlock(cg->builder);
    LLVMValueRef fun = LLVMGetBasicBlockParent(bb);
    // KSDbgInfo.emitLocation(this);
    cg->current_loop_block++;
    LLVMBasicBlockRef start_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "loop");
    LLVMBasicBlockRef cont_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "contloop");
    LLVMBasicBlockRef end_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "afterloop");
    cg->loop_blocks[cg->current_loop_block].cont_bb = start_bb;
    cg->loop_blocks[cg->current_loop_block].end_bb = end_bb;
    LLVMBuildBr(cg->builder, start_bb);
    LLVMPositionBuilderAtEnd(cg->builder, start_bb);

    LLVMValueRef end_cond = emit_ir_code(cg, node->whileloop->expr);
    end_cond = LLVMBuildICmp(cg->builder, LLVMIntNE, end_cond, get_int_zero(cg->context, cg->builder), "loopcond");
    //if end_cond (id < end != 0) then start_bb else end_bb
    LLVMBuildCondBr(cg->builder, end_cond, cont_bb, end_bb);
    LLVMPositionBuilderAtEnd(cg->builder, cont_bb);

    emit_ir_code(cg, node->whileloop->body);

    LLVMBuildBr(cg->builder, start_bb);
    LLVMPositionBuilderAtEnd(cg->builder, end_bb);
    cg->current_loop_block--;
    return 0;
}

LLVMValueRef _emit_for_node(struct cg_llvm *cg, struct ast_node *node)
{
    struct type_context *tc = cg->base.sema_context->tc;
    symbol var_name = node->forloop->var->var->var->ident->name;
    LLVMBasicBlockRef bb = LLVMGetInsertBlock(cg->builder);
    LLVMValueRef fun = LLVMGetBasicBlockParent(bb);

    // TODO: fixme with correct type_exp passed down
    LLVMTypeRef at = cg->ops[TYPE_INT].get_type(cg->context, 0);
    assert(at);
    LLVMValueRef alloca = create_alloca(at, 4, fun, string_get(var_name));

    // KSDbgInfo.emitLocation(this);
    LLVMValueRef start_v = emit_ir_code(cg, node->forloop->range->range->start);
    assert(start_v);

    LLVMBuildStore(cg->builder, start_v, alloca);
    cg->current_loop_block++;
    LLVMBasicBlockRef start_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "loop");
    LLVMBasicBlockRef cont_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "contloop");
    LLVMBasicBlockRef end_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "afterloop");
    cg->loop_blocks[cg->current_loop_block].cont_bb = cont_bb;
    cg->loop_blocks[cg->current_loop_block].end_bb = end_bb;
    LLVMBuildBr(cg->builder, start_bb);
    LLVMPositionBuilderAtEnd(cg->builder, start_bb);

    LLVMValueRef old_alloca = (LLVMValueRef)hashtable_get_p(&cg->varname_2_irvalues, var_name);
    hashtable_set_p(&cg->varname_2_irvalues, var_name, alloca);
    emit_ir_code(cg, node->forloop->body);

    LLVMBuildBr(cg->builder, cont_bb);
    LLVMPositionBuilderAtEnd(cg->builder, cont_bb);

    LLVMValueRef step_v;
    if (node->forloop->range->range->step) {
        step_v = emit_ir_code(cg, node->forloop->range->range->step);
        assert(step_v);
    } else {
        step_v = get_int_one(cg->context);
    }
    LLVMValueRef cur_var = LLVMBuildLoad2(cg->builder, at, alloca, string_get(var_name));
    LLVMValueRef next_var = LLVMBuildAdd(cg->builder, cur_var, step_v, "nextvar");
    LLVMBuildStore(cg->builder, next_var, alloca);

    struct ast_node *id = ident_node_new(var_name, node->forloop->var->loc);
    id->type = node->forloop->var->type;
    struct ast_node *end_cond_node = binary_node_new(OP_LT, 
        id, 
        node_copy(tc, node->forloop->range->range->end), 
        node->forloop->range->range->end->loc);
    //TODO: need to free new created binary and id node
    LLVMValueRef end_cond = emit_ir_code(cg, end_cond_node);
    assert(end_cond);
    node_free(end_cond_node);
    //IF Not Equals: end_cond, 0
    end_cond = LLVMBuildICmp(cg->builder, LLVMIntNE, end_cond, get_int_zero(cg->context, cg->builder), "loopcond");

    //if end_cond (id < end != 0) then start_bb else end_bb
    LLVMBuildCondBr(cg->builder, end_cond, start_bb, end_bb);
    LLVMPositionBuilderAtEnd(cg->builder, end_bb);

    if (!old_alloca)
        hashtable_remove_p(&cg->varname_2_irvalues, var_name);

    cg->current_loop_block--;
    return LLVMConstNull(cg->ops[TYPE_INT].get_type(cg->context, 0));
}

void create_ir_module(struct cg_llvm *cg,
    const char *module_name)
{
    delete_current_module(cg);
    cg->module = LLVMModuleCreateWithNameInContext(module_name, cg->context);
    cg->target_machine = create_target_machine(cg->module, &cg->target_data);
}

LLVMValueRef emit_ir_code(struct cg_llvm *cg, struct ast_node *node)
{
    if(node->transformed) 
        node = node->transformed;
    LLVMValueRef value = 0;
    switch(node->node_type){
        case LITERAL_NODE:
            value = _emit_literal_node(cg, node);
            break;
        case IDENT_NODE:
            value = _emit_ident_node(cg, node);
            break;
        case VAR_NODE:
            value = emit_var_node(cg, node);
            break;
        case ADT_INIT_NODE:
            value = emit_struct_init_node(cg, node, false, "");
            break;
        case ARRAY_INIT_NODE:
            value = emit_array_init_node(cg, node, false, "");
            break;        
        case UNARY_NODE:
            value = _emit_unary_node(cg, node);
            break;
        case MEMBER_INDEX_NODE:
            if(node->index->object->type->type == TYPE_ARRAY)
                value = _emit_array_index(cg, node);
            else
                value = _emit_field_access_node(cg, node);
            break;
        case ASSIGN_NODE:
            value = _emit_assign_node(cg, node);
            break;
        case BINARY_NODE:
            value = _emit_binary_node(cg, node);
            break;
        case IF_NODE:
            value = _emit_condition_node(cg, node);
            break;
        case WHILE_NODE:
            value = _emit_while_node(cg, node);
            break;
        case FOR_NODE:
            value = _emit_for_node(cg, node);
            break;
        case JUMP_NODE:
            value = _emit_jump_node(cg, node);
            break;
        case CALL_NODE:
            value = emit_call_node(cg, node);
            break;
        case FUNC_TYPE_NODE:
            value = emit_func_type_node(cg, node);
            break;
        case FUNC_NODE:
            value = emit_function_node(cg, node);
            break;
        case BLOCK_NODE:
            value = _emit_block_node(cg, node);
            break;
        case CAST_NODE:
        case MATCH_NODE:
            break;

        case TYPE_EXPR_ITEM_NODE:
        case MATCH_CASE_NODE:
        case WILDCARD_NODE:
        case VARIANT_NODE:
        case VARIANT_TYPE_ITEM_NODE:
        case ARRAY_TYPE_NODE:
        case TYPE_ITEM_NODE:
        case TYPE_NODE:
        case STRUCT_NODE:
        case NULL_NODE:
        case IMPORT_NODE:
        case MEMORY_NODE:
        case RANGE_NODE:
        case TOTAL_NODE:
        case TOKEN_NODE:
            break;
    }
    return value;
}

LLVMTargetMachineRef create_target_machine(LLVMModuleRef module, LLVMTargetDataRef* target_data_out)
{
    char *target_triple = LLVMGetDefaultTargetTriple();
    LLVMSetTarget(module, target_triple);
    char *error;
    LLVMTargetRef target;
    if (LLVMGetTargetFromTriple(target_triple, &target, &error)) {
        log_info(ERROR, "error in creating target machine: %s", error);
        free(target_triple);
        return 0;
    }
    const char *cpu = "generic";
    const char *features = "";
    LLVMCodeGenOptLevel opt = LLVMCodeGenLevelDefault;
    LLVMRelocMode rm = LLVMRelocPIC; // LLVMRelocDefault;
    LLVMCodeModel cm = LLVMCodeModelDefault;
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, target_triple, cpu, features, opt, rm, cm);
    LLVMTargetDataRef target_data = LLVMCreateTargetDataLayout(target_machine);
    LLVMSetModuleDataLayout(module, target_data);
    *target_data_out = target_data;
    free(target_triple);
    return target_machine;
}

LLVMTypeRef get_backend_type(struct type_item *type)
{
    if(type->backend_type)
        return type->backend_type;
    assert(g_cg);
    type->backend_type = _get_llvm_type(g_cg, type);
    return type->backend_type;
}

LLVMTargetDataRef get_llvm_data_layout()
{
    assert(g_cg && g_cg->module);
    return LLVMGetModuleDataLayout(g_cg->module);
}

enum OS get_os()
{
    assert(g_cg);
    return g_cg->base.target_info->os;
}

LLVMModuleRef get_llvm_module()
{
    assert(g_cg);
    return g_cg->module;
}

struct cg_llvm *get_cg()
{
    assert(g_cg);
    return g_cg;
}

void emit_sp_code(struct cg_llvm *cg)
{
    for(size_t i = 0; i < array_size(&cg->base.sema_context->new_specialized_asts); i++){
        struct ast_node *new_sp = array_get_ptr(&cg->base.sema_context->new_specialized_asts, i);
        emit_ir_code(cg, new_sp);
    }
    array_reset(&cg->base.sema_context->new_specialized_asts);
}

void emit_code(struct cg_llvm *cg, struct ast_node *node)
{
    emit_sp_code(cg);
    if (array_size(&cg->base.sema_context->used_builtin_names)) {
        for (size_t i = 0; i < array_size(&cg->base.sema_context->used_builtin_names); i++) {
            symbol built_name = array_get_ptr(&cg->base.sema_context->used_builtin_names, i);
            struct ast_node *n = hashtable_get_p(&cg->base.sema_context->builtin_ast, built_name);
            emit_ir_code(cg, n);
        }
        array_clear(&cg->base.sema_context->used_builtin_names);
    }
}
