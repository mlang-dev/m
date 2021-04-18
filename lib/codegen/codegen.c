/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation Functions
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "codegen/cg_call.h"
#include "codegen/cg_fun.h"
#include "codegen/cg_var.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"
#include "sema/type.h"
#include <llvm-c/Support.h>

struct code_generator *g_cg = 0;

LLVMContextRef get_llvm_context()
{
    if (!g_cg)
        return 0;
    return g_cg->context;
}

LLVMTypeRef get_int_type(LLVMContextRef context, struct type_ext *type)
{
    (void *)type;
    return LLVMInt32TypeInContext(context);
}

LLVMTypeRef get_char_type(LLVMContextRef context, struct type_ext *type)
{
    (void *)type;
    return LLVMInt8TypeInContext(context);
}

LLVMTypeRef get_bool_type(LLVMContextRef context, struct type_ext *type)
{
    (void *)type;
    return LLVMInt1TypeInContext(context);
}

LLVMTypeRef get_double_type(LLVMContextRef context, struct type_ext *type)
{
    (void *)type;
    return LLVMDoubleTypeInContext(context);
}

LLVMTypeRef get_ext_type(LLVMContextRef context, struct type_exp *type_exp)
{
    assert(type_exp->type == TYPE_EXT);
    assert(g_cg);
    LLVMTypeRef struct_type = hashtable_get(&g_cg->ext_types, string_get(type_exp->name));
    if (struct_type)
        return struct_type;
    struct type_oper *type = (struct type_oper *)type_exp;
    struct_type = LLVMStructCreateNamed(context, string_get(type->base.name));
    unsigned int member_count = array_size(&type->args);
    LLVMTypeRef *members = malloc(member_count * sizeof(LLVMTypeRef));
    for (size_t i = 0; i < member_count; i++) {
        struct type_exp *field_type = *(struct type_exp **)array_get(&type->args, i);
        members[i] = get_llvm_type(field_type);
    }
    LLVMStructSetBody(struct_type, members, member_count, false);
    hashtable_set(&g_cg->ext_types, string_get(type->base.name), struct_type);
    free(members);
    return struct_type;
}

LLVMTypeRef get_str_type(LLVMContextRef context, struct type_ext *type)
{
    (void *)type;
    return LLVMPointerType(LLVMInt8TypeInContext(context), 0);
}

const char *buiiltin_funs[] = {
    "llvm.sin",
    "llvm.cos",
    "llvm.sqrt",
};

LLVMValueRef get_int_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void *)builder;
    return LLVMConstInt(get_int_type(context, 0), *(int *)value, true);
}

LLVMValueRef get_bool_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void *)builder;
    return LLVMConstInt(get_bool_type(context, 0), *(int *)value, true);
}

LLVMValueRef get_char_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void *)builder;
    return LLVMConstInt(get_char_type(context, 0), *(char *)value, true);
}

LLVMValueRef get_double_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    (void *)builder;
    return LLVMConstReal(get_double_type(context, 0), *(double *)value);
}

LLVMValueRef get_str_const(LLVMContextRef context, LLVMBuilderRef builder, void *value)
{
    const char *str = (const char *)value;

    //implementation of LLVMBuildGlobalString, except of way of getting module
    uint64_t size = strlen(str);
    LLVMValueRef str_const = LLVMConstStringInContext(context, str, size, 0);
    LLVMValueRef str_value = LLVMAddGlobal(get_llvm_module(), LLVMTypeOf(str_const), "");
    LLVMSetInitializer(str_value, str_const);
    LLVMSetGlobalConstant(str_value, true);
    LLVMSetLinkage(str_value, LLVMPrivateLinkage);
    LLVMSetUnnamedAddr(str_value, true);
    LLVMSetAlignment(str_value, 1);

    //converting GlobalVariable to a pointer
    LLVMValueRef zero = LLVMConstInt(LLVMInt32TypeInContext(context), 0, false);
    LLVMValueRef indexes[2] = { zero, zero };
    return LLVMBuildInBoundsGEP2(builder, LLVMGlobalGetValueType(str_value), str_value, indexes, 2, "");
}

LLVMValueRef get_int_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void *)builder;
    return LLVMConstInt(get_int_type(context, 0), 0, true);
}

LLVMValueRef get_bool_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void *)builder;
    return LLVMConstInt(get_bool_type(context, 0), 0, true);
}

LLVMValueRef get_char_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void *)builder;
    return LLVMConstInt(get_char_type(context, 0), 0, true);
}

LLVMValueRef get_double_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void *)builder;
    return LLVMConstReal(get_double_type(context, 0), 0.0);
}

LLVMValueRef get_str_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
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
    LLVMBuildICmp,
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
    LLVMBuildICmp,
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
    LLVMBuildICmp,
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
    LLVMBuildICmp,
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
    LLVMBuildFCmp,
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

struct ops ext_ops = {
    get_ext_type,
    get_double_const,
    get_int_zero,
    get_int_one,
    LLVMBuildFAdd,
    LLVMBuildFSub,
    LLVMBuildFMul,
    LLVMBuildFDiv,
    LLVMBuildFRem,
    LLVMBuildICmp,
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

void _set_bin_ops(struct code_generator *cg)
{
    cg->ops[TYPE_UNK] = double_ops;
    cg->ops[TYPE_GENERIC] = double_ops;
    cg->ops[TYPE_UNIT] = double_ops;
    cg->ops[TYPE_BOOL] = bool_ops;
    cg->ops[TYPE_CHAR] = char_ops;
    cg->ops[TYPE_INT] = int_ops;
    cg->ops[TYPE_DOUBLE] = double_ops;
    cg->ops[TYPE_STRING] = str_ops;
    cg->ops[TYPE_FUNCTION] = double_ops;
    cg->ops[TYPE_EXT] = ext_ops;
}

struct code_generator *cg_new(struct sema_context *sema_context)
{
    LLVMContextRef context = LLVMContextCreate();
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    struct code_generator *cg = malloc(sizeof(*cg));
    cg->sema_context = sema_context;
    cg->context = context;
    cg->builder = LLVMCreateBuilderInContext(context);
    hashset_init(&cg->builtins);
    cg->module = 0;
    _set_bin_ops(cg);
    hashtable_init(&cg->gvs);
    hashtable_init(&cg->protos);
    hashtable_init(&cg->named_values);
    hashtable_init(&cg->ext_types);
    hashtable_init(&cg->ext_nodes);
    hashtable_init(&cg->ext_vars);
    hashtable_init_with_value_size(&cg->type_size_infos, sizeof(struct type_size_info), 0);
    hashtable_init_with_value_size(&cg->fun_infos, sizeof(struct fun_info), (free_fun)fun_info_deinit);
    cg->target_info = ti_new();
    g_cg = cg;
    return cg;
}

void cg_free(struct code_generator *cg)
{
    LLVMDisposeBuilder(cg->builder);
    if (cg->module)
        LLVMDisposeModule(cg->module);
    LLVMContextDispose(cg->context);
    ti_free(cg->target_info);
    hashtable_deinit(&cg->fun_infos);
    hashtable_deinit(&cg->type_size_infos);
    hashtable_deinit(&cg->gvs);
    hashtable_deinit(&cg->protos);
    hashtable_deinit(&cg->named_values);
    hashset_deinit(&cg->builtins);
    hashtable_deinit(&cg->ext_types);
    hashtable_deinit(&cg->ext_nodes);
    hashtable_deinit(&cg->ext_vars);
    free(cg);
    g_cg = cg;
}

LLVMTypeRef _get_llvm_type(struct code_generator *cg, struct type_exp *type)
{
    enum type en_type = get_type(type);
    return cg->ops[en_type].get_type(cg->context, type);
}

LLVMValueRef _emit_block_node(struct code_generator *cg, struct exp_node *node)
{
    struct block_node *block = (struct block_node *)node;
    LLVMValueRef codegen = 0;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node *exp = *(struct exp_node **)array_get(&block->nodes, i);
        codegen = emit_ir_code(cg, exp);
    }
    return codegen;
}

void _emit_param(struct type_oper *to)
{
    struct aligned_pointer ap = { 0, 0 };
}

LLVMValueRef _emit_literal_node(struct code_generator *cg, struct exp_node *node)
{
    assert(node->type);
    enum type type = get_type(node->type);
    void *value = 0;
    if (type == TYPE_CHAR)
        value = &((struct literal_node *)node)->char_val;
    else if (is_int_type(type))
        value = &((struct literal_node *)node)->int_val;
    else if (type == TYPE_DOUBLE)
        value = &((struct literal_node *)node)->double_val;
    else if (type == TYPE_STRING) {
        value = (void *)((struct literal_node *)node)->str_val;
    }
    return cg->ops[type].get_const(cg->context, cg->builder, value);
}

/*xy->TypeNode*/
LLVMValueRef _emit_ident_node(struct code_generator *cg, struct exp_node *node)
{
    struct ident_node *ident = (struct ident_node *)node;
    symbol id = *((symbol *)array_get(&ident->member_accessors, 0));
    const char *idname = string_get(id);
    LLVMValueRef v = (LLVMValueRef)hashtable_get(&cg->named_values, idname);
    if (!v) {
        v = get_global_variable(cg, idname);
        assert(v);
    }
    if (array_size(&ident->member_accessors) > 1) {
        char tempname[64];
        string *type_name = hashtable_get(&cg->ext_vars, idname);
        struct type_node *type_node = (struct type_node *)hashtable_get(&cg->ext_nodes, string_get(type_name));
        symbol attr = *((symbol *)array_get(&ident->member_accessors, 1));
        int index = find_member_index(type_node, attr);
        sprintf(tempname, "temp%d", index);
        v = LLVMBuildStructGEP(cg->builder, v, index, tempname);
    }
    return LLVMBuildLoad(cg->builder, v, string_get(ident->name));
}

LLVMValueRef _emit_unary_node(struct code_generator *cg, struct exp_node *node)
{
    struct unary_node *unary = (struct unary_node *)node;
    LLVMValueRef operand_v = emit_ir_code(cg, unary->operand);
    assert(operand_v);
    if (string_eq_chars(unary->op, "+"))
        return operand_v;
    else if (string_eq_chars(unary->op, "-")) {
        return cg->ops->neg_op(cg->builder, operand_v, "negtmp");
    } else if (string_eq_chars(unary->op, "!")) {
        LLVMValueRef ret = cg->ops->not_op(cg->builder, operand_v, "nottmp");
        return LLVMBuildZExt(cg->builder, ret, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
    }
    string fname;
    string_init_chars(&fname, "unary");
    string_add(&fname, unary->op);
    LLVMValueRef fun = get_llvm_function(cg, string_get(&fname));
    if (fun == 0)
        return log_info(ERROR, "Unknown unary operator");

    // KSDbgInfo.emitLocation(this);
    return LLVMBuildCall(cg->builder, fun, &operand_v, 1, "unop");
}

LLVMValueRef _emit_binary_node(struct code_generator *cg, struct exp_node *node)
{
    struct binary_node *bin = (struct binary_node *)node;
    LLVMValueRef lv = emit_ir_code(cg, bin->lhs);
    LLVMValueRef rv = emit_ir_code(cg, bin->rhs);
    //assert(LLVMGetValueKind(lv) == LLVMGetValueKind(rv));
    assert(bin->lhs->type && prune(bin->lhs->type)->type == prune(bin->rhs->type)->type);
    assert(lv && rv);
    assert(LLVMTypeOf(lv) == LLVMTypeOf(rv));
    struct ops *ops = &cg->ops[prune(bin->lhs->type)->type];
    if (string_eq_chars(bin->op, "+"))
        return ops->add(cg->builder, lv, rv, "addtmp");
    else if (string_eq_chars(bin->op, "-"))
        return ops->sub(cg->builder, lv, rv, "subtmp");
    else if (string_eq_chars(bin->op, "*"))
        return ops->mul(cg->builder, lv, rv, "multmp");
    else if (string_eq_chars(bin->op, "/"))
        return ops->div(cg->builder, lv, rv, "divtmp");
    else if (string_eq_chars(bin->op, "%"))
        return ops->rem(cg->builder, lv, rv, "remtmp");
    else if (string_eq_chars(bin->op, "<")) {
        lv = ops->cmp(cg->builder, ops->cmp_lt, lv, rv, "cmplttmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else if (string_eq_chars(bin->op, ">")) {
        lv = ops->cmp(cg->builder, ops->cmp_gt, lv, rv, "cmpgttmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else if (string_eq_chars(bin->op, "==")) {
        lv = ops->cmp(cg->builder, ops->cmp_eq, lv, rv, "cmpeqtmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else if (string_eq_chars(bin->op, "!=")) {
        lv = ops->cmp(cg->builder, ops->cmp_neq, lv, rv, "cmpneqtmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else if (string_eq_chars(bin->op, "<=")) {
        lv = ops->cmp(cg->builder, ops->cmp_le, lv, rv, "cmpletmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else if (string_eq_chars(bin->op, ">=")) {
        lv = ops->cmp(cg->builder, ops->cmp_ge, lv, rv, "cmpgetmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else if (string_eq_chars(bin->op, "||")) {
        lv = ops->or_op(cg->builder, lv, rv, "ortmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else if (string_eq_chars(bin->op, "&&")) {
        lv = ops->and_op(cg->builder, lv, rv, "andtmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context, 0), "ret_val_int");
        return lv;
    } else {
        string f_name;
        string_init_chars(&f_name, "binary");
        string_add(&f_name, bin->op);
        LLVMValueRef fun = get_llvm_function(cg, string_get(&f_name));
        assert(fun && "binary operator not found!");
        LLVMValueRef ops[2] = { lv, rv };
        return LLVMBuildCall(cg->builder, fun, ops, 2, "binop");
    }
}

LLVMValueRef _emit_condition_node(struct code_generator *cg, struct exp_node *node)
{
    // KSDbgInfo.emitLocation(this);
    struct condition_node *cond = (struct condition_node *)node;
    LLVMValueRef cond_v = emit_ir_code(cg, cond->condition_node);
    assert(cond_v);

    cond_v = LLVMBuildICmp(cg->builder, LLVMIntNE, cond_v, cg->ops[TYPE_INT].get_zero(cg->context, cg->builder), "ifcond");

    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder));

    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "then");
    LLVMBasicBlockRef else_bb = LLVMCreateBasicBlockInContext(cg->context, "else");
    LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(cg->context, "ifcont");

    LLVMBuildCondBr(cg->builder, cond_v, then_bb, else_bb);
    LLVMPositionBuilderAtEnd(cg->builder, then_bb);

    LLVMValueRef then_v = emit_ir_code(cg, cond->then_node);
    assert(then_v);
    LLVMBuildBr(cg->builder, merge_bb);
    then_bb = LLVMGetInsertBlock(cg->builder);

    LLVMAppendExistingBasicBlock(fun, else_bb);
    LLVMPositionBuilderAtEnd(cg->builder, else_bb);

    LLVMValueRef else_v = emit_ir_code(cg, cond->else_node);
    assert(else_v);
    LLVMBuildBr(cg->builder, merge_bb);
    else_bb = LLVMGetInsertBlock(cg->builder);
    LLVMAppendExistingBasicBlock(fun, merge_bb);
    LLVMPositionBuilderAtEnd(cg->builder, merge_bb);
    enum type type = get_type(cond->then_node->type);
    LLVMValueRef phi_node = LLVMBuildPhi(cg->builder, cg->ops[type].get_type(cg->context, cond->then_node->type), "iftmp");
    LLVMAddIncoming(phi_node, &then_v, &then_bb, 1);
    LLVMAddIncoming(phi_node, &else_v, &else_bb, 1);
    return phi_node;
}

LLVMValueRef _emit_type_node(struct code_generator *cg, struct exp_node *node)
{
    struct type_oper *type = (struct type_oper *)node->type;
    assert(node->type);
    LLVMTypeRef struct_type = get_ext_type(cg->context, node->type);
    hashtable_set(&cg->ext_nodes, string_get(type->base.name), node);
    return 0;
}

LLVMValueRef _emit_type_value_node(struct code_generator *cg, struct exp_node *node)
{
    //struct type_value_node* type_values = (struct type_value_node*)node;
    assert(false);
    return 0;
}

LLVMValueRef _emit_for_node(struct code_generator *cg, struct exp_node *node)
{
    struct for_node *forn = (struct for_node *)node;
    const char *var_name = string_get(forn->var_name);
    LLVMBasicBlockRef bb = LLVMGetInsertBlock(cg->builder);
    LLVMValueRef fun = LLVMGetBasicBlockParent(bb);

    //TODO: fixme with correct type_exp passed down
    LLVMValueRef alloca = emit_entry_block_alloca(cg->ops[TYPE_INT].get_type(cg->context, 0), fun, var_name);

    // KSDbgInfo.emitLocation(this);
    LLVMValueRef start_v = emit_ir_code(cg, forn->start);
    assert(start_v);

    LLVMBuildStore(cg->builder, start_v, alloca);
    LLVMBasicBlockRef loop_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "loop");
    LLVMBuildBr(cg->builder, loop_bb);
    LLVMPositionBuilderAtEnd(cg->builder, loop_bb);

    LLVMValueRef old_alloca = (LLVMValueRef)hashtable_get(&cg->named_values, var_name);
    hashtable_set(&cg->named_values, var_name, alloca);
    emit_ir_code(cg, forn->body);
    LLVMValueRef step_v;
    if (forn->step) {
        step_v = emit_ir_code(cg, forn->step);
        assert(step_v);
    } else {
        step_v = get_int_one(cg->context);
    }
    LLVMValueRef end_cond = emit_ir_code(cg, forn->end);
    assert(end_cond);

    LLVMValueRef cur_var = LLVMBuildLoad(cg->builder, alloca, var_name);
    LLVMValueRef next_var = LLVMBuildAdd(cg->builder, cur_var, step_v, "nextvar");
    LLVMBuildStore(cg->builder, next_var, alloca);
    end_cond = LLVMBuildICmp(cg->builder, LLVMIntNE, end_cond, get_int_zero(cg->context, cg->builder), "loopcond");

    LLVMBasicBlockRef after_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "afterloop");

    LLVMBuildCondBr(cg->builder, end_cond, loop_bb, after_bb);
    LLVMPositionBuilderAtEnd(cg->builder, after_bb);

    if (old_alloca)
        hashtable_set(&cg->named_values, var_name, old_alloca);
    else
        hashtable_remove(&cg->named_values, var_name);

    return LLVMConstNull(cg->ops[TYPE_INT].get_type(cg->context, 0));
}

void create_ir_module(struct code_generator *cg,
    const char *module_name)
{
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext(module_name, cg->context);
    LLVMTargetMachineRef target_marchine = create_target_machine(module);
    LLVMTargetDataRef data_layout = LLVMCreateTargetDataLayout(target_marchine);
    LLVMSetModuleDataLayout(module, data_layout);
    cg->module = module;
}

LLVMValueRef _emit_unk_node(struct code_generator *cg, struct exp_node *node)
{
    if (!cg || !node)
        return 0;

    return 0;
}

LLVMValueRef (*cg_fp[])(struct code_generator *, struct exp_node *) = {
    _emit_unk_node,
    _emit_literal_node,
    _emit_ident_node,
    emit_var_node,
    _emit_type_node,
    _emit_type_value_node,
    _emit_unary_node,
    _emit_binary_node,
    _emit_condition_node,
    _emit_for_node,
    emit_call_node,
    emit_prototype_node,
    emit_function_node,
    _emit_block_node
};

LLVMValueRef emit_ir_code(struct code_generator *cg, struct exp_node *node)
{
    return cg_fp[node->node_type](cg, node);
}

LLVMTargetMachineRef create_target_machine(LLVMModuleRef module)
{
    char *target_triple = LLVMGetDefaultTargetTriple();
    LLVMSetTarget(module, target_triple);
    char *error;
    LLVMTargetRef target;
    if (LLVMGetTargetFromTriple(target_triple, &target, &error)) {
        log_info(ERROR, "error in creating target machine: %s", error);
        return 0;
    }
    const char *cpu = "generic";
    const char *features = "";
    LLVMCodeGenOptLevel opt = LLVMCodeGenLevelDefault;
    LLVMRelocMode rm = LLVMRelocPIC; // LLVMRelocDefault;
    LLVMCodeModel cm = LLVMCodeModelDefault;
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, target_triple, cpu, features, opt, rm, cm);
    LLVMSetModuleDataLayout(module, LLVMCreateTargetDataLayout(target_machine));
    return target_machine;
}

LLVMTypeRef get_llvm_type(struct type_exp *type)
{
    assert(g_cg);
    return _get_llvm_type(g_cg, type);
}

LLVMTypeRef get_llvm_type_for_abi(struct type_exp *type)
{
    assert(g_cg);
    struct type_size_info tsi = get_type_size_info(type);
    if (type->type == TYPE_BOOL) //bool type is 1 bit size in llvm but we need to comply with abi size
        return LLVMIntTypeInContext(get_llvm_context(), tsi.width_bits);
    return _get_llvm_type(g_cg, type);
}

LLVMTargetDataRef get_llvm_data_layout()
{
    assert(g_cg && g_cg->module);
    return LLVMGetModuleDataLayout(g_cg->module);
}

enum OS get_os()
{
    assert(g_cg);
    return g_cg->target_info->os;
}

struct hashtable *get_type_size_infos()
{
    assert(g_cg);
    return &g_cg->type_size_infos;
}

LLVMModuleRef get_llvm_module()
{
    assert(g_cg);
    return g_cg->module;
}

struct hashtable *get_fun_infos()
{
    assert(g_cg);
    return &g_cg->fun_infos;
}
