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
#include "codegen/type_size_info.h"
#include "sema/type.h"
#include <llvm-c/Support.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Core.h>


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

LLVMTypeRef get_ext_type(LLVMContextRef context, struct type_item *type_exp)
{
    assert(type_exp->type == TYPE_STRUCT);
    assert(g_cg);
    LLVMTypeRef struct_type = hashtable_get_p(&g_cg->typename_2_irtypes, type_exp->name);
    if (struct_type)
        return struct_type;
    struct_type = LLVMStructCreateNamed(context, string_get(type_exp->name));
    unsigned member_count = (unsigned)array_size(&type_exp->args);
    LLVMTypeRef *members;
    MALLOC(members, member_count * sizeof(LLVMTypeRef));
    for (unsigned i = 0; i < member_count; i++) {
        struct type_item *field_type = *(struct type_item **)array_get(&type_exp->args, i);
        members[i] = get_llvm_type(field_type);
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
    LLVMValueRef str_value = LLVMAddGlobal(get_llvm_module(), LLVMTypeOf(str_const), "");
    LLVMSetInitializer(str_value, str_const);
    LLVMSetGlobalConstant(str_value, true);
    LLVMSetLinkage(str_value, LLVMPrivateLinkage);
    LLVMSetUnnamedAddr(str_value, true);
    LLVMSetAlignment(str_value, 1);

    // converting GlobalVariable to a pointer
    LLVMValueRef zero = LLVMConstInt(LLVMInt32TypeInContext(context), 0, false);
    LLVMValueRef indexes[2] = { zero, zero };
    return LLVMBuildInBoundsGEP2(builder, LLVMGlobalGetValueType(str_value), str_value, indexes, 2, "");
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
    cg->ops[TYPE_STRUCT] = ext_ops;
}

unsigned _get_count_struct_element_types(TargetType type)
{
    return LLVMCountStructElementTypes(type);
}

void _fill_struct_fields(struct array *fields, TargetType struct_type)
{
    for (unsigned j = 0; j < LLVMCountStructElementTypes(struct_type); ++j) {
        LLVMTypeRef field_type = LLVMStructGetTypeAtIndex(struct_type, j);
        array_push(fields, &field_type);
    }
}

TargetType _get_function_type(TargetType ret_type, TargetType *param_types, unsigned param_count, bool is_variadic)
{
    return LLVMFunctionType(ret_type, (LLVMTypeRef*)param_types, param_count, is_variadic);
}

TargetType _get_target_type(struct type_item *type)
{
    return get_llvm_type(type);
}

TargetType _get_pointer_type(TargetType type)
{
    return LLVMPointerType(type, 0);
}

TargetType _get_size_int_type(unsigned width)
{
    return LLVMIntTypeInContext(get_llvm_context(), width);
}


void _init_target_info(struct target_info *ti)
{
    ti->extend_type = LLVMInt8TypeInContext(get_llvm_context()); //would use 32 bits
    ti->get_size_int_type = _get_size_int_type;//LLVMIntTypeInContext(get_llvm_context(), width)
    ti->get_pointer_type = _get_pointer_type; //LLVMPointerType(get_llvm_type(fi->ret.type), 0)
    ti->get_target_type = _get_target_type; //get_llvm_type(fi->ret.type)
    ti->get_function_type = _get_function_type;
    ti->fill_struct_fields = _fill_struct_fields;//
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
    _set_bin_ops(cg);
    hashtable_init(&cg->cg_gvar_name_2_asts);
    hashtable_init(&cg->varname_2_irvalues);
    hashtable_init(&cg->typename_2_irtypes);
    hashtable_init(&cg->varname_2_typename);
    cg->base.target_info = ti_new(LLVMGetDefaultTargetTriple());
    g_cg = cg;
    _init_target_info(cg->base.target_info);
    if (get_os() == OS_WIN32){
        cg->base.compute_fun_info = winx86_64_compute_fun_info;
    }else{
        cg->base.compute_fun_info = x86_64_compute_fun_info;
    }
    return cg;
}

void llvm_cg_free(struct cg_llvm *cg)
{
    LLVMDisposeBuilder(cg->builder);
    if (cg->module)
        LLVMDisposeModule(cg->module);
    LLVMContextDispose(cg->context);
    ti_free(cg->base.target_info);
    hashtable_deinit(&cg->cg_gvar_name_2_asts);
    hashtable_deinit(&cg->varname_2_irvalues);
    hashtable_deinit(&cg->typename_2_irtypes);
    hashtable_deinit(&cg->varname_2_typename);
    FREE(cg);
    g_cg = 0;
}

LLVMTypeRef _get_llvm_type(struct cg_llvm *cg, struct type_item *type)
{
    enum type en_type = get_type(type);
    return cg->ops[en_type].get_type(cg->context, type);
}

LLVMValueRef _emit_block_node(struct cg_llvm *cg, struct ast_node *node)
{
    LLVMValueRef codegen = 0;
    for (size_t i = 0; i < array_size(&node->block->nodes); i++) {
        struct ast_node *exp = *(struct ast_node **)array_get(&node->block->nodes, i);
        codegen = emit_ir_code(cg, exp);
    }
    return codegen;
}

LLVMValueRef _emit_literal_node(struct cg_llvm *cg, struct ast_node *node)
{
    assert(node->type);
    assert(node->node_type == LITERAL_NODE);
    enum type type = get_type(node->type);
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
    if (node->type->type < TYPE_STRUCT){
        return LLVMBuildLoad(cg->builder, v, string_get(node->ident->name));
    }
    else{
        return v;
    }
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
    return LLVMBuildCall(cg->builder, fun, &operand_v, 1, "unop");
}

LLVMValueRef _emit_accessor_node(struct cg_llvm *cg, struct ast_node *node)
{
    assert(node->node_type == MEMBER_INDEX_NODE);
    assert(node->index->object->node_type == IDENT_NODE);
    assert(node->index->index->node_type == IDENT_NODE);
    symbol id = node->index->object->ident->name;
    LLVMValueRef v = (LLVMValueRef)hashtable_get_p(&cg->varname_2_irvalues, id);
    if (!v) {
        v = get_global_variable(cg, id);
        assert(v);
    }
    string *type_name = hashtable_get_p(&cg->varname_2_typename, id);
    struct ast_node *type_item_node = hashtable_get_p(&cg->base.sema_context->struct_typename_2_asts, type_name);
    symbol attr = node->index->index->ident->name;
    int index = find_member_index(type_item_node, attr);
    v = LLVMBuildStructGEP(cg->builder, v, index, string_get(attr));
    if (node->type->type < TYPE_STRUCT){
        string dot_id ;
        string_init(&dot_id);
        string_add(&dot_id, id);
        string_add_chars(&dot_id, ".");
        string_add(&dot_id, attr);
        return LLVMBuildLoad(cg->builder, v, string_get(&dot_id));
    }
    else{
        return v;
    }
}

LLVMValueRef _emit_binary_node(struct cg_llvm *cg, struct ast_node *node)
{

    LLVMValueRef lv = emit_ir_code(cg, node->binop->lhs);
    LLVMValueRef rv = emit_ir_code(cg, node->binop->rhs);
    // assert(LLVMGetValueKind(lv) == LLVMGetValueKind(rv));
    assert(node->binop->lhs->type && prune(node->binop->lhs->type)->type == prune(node->binop->rhs->type)->type);
    assert(lv && rv);
    assert(LLVMTypeOf(lv) == LLVMTypeOf(rv));
    struct ops *ops = &cg->ops[prune(node->binop->lhs->type)->type];
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
            return LLVMBuildCall(cg->builder, fun, lrv, 2, "binop");
    }
}

LLVMValueRef _emit_condition_node(struct cg_llvm *cg, struct ast_node *node)
{
    // KSDbgInfo.emitLocation(this);
    LLVMValueRef cond_v = emit_ir_code(cg, node->cond->if_node);
    assert(cond_v);

    cond_v = LLVMBuildICmp(cg->builder, LLVMIntNE, cond_v, cg->ops[TYPE_INT].get_zero(cg->context, cg->builder), "ifcond");

    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder));

    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "then");
    LLVMBasicBlockRef else_bb = LLVMCreateBasicBlockInContext(cg->context, "else");
    LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(cg->context, "ifcont");

    LLVMBuildCondBr(cg->builder, cond_v, then_bb, else_bb);
    LLVMPositionBuilderAtEnd(cg->builder, then_bb);

    LLVMValueRef then_v = emit_ir_code(cg, node->cond->then_node);
    assert(then_v);
    LLVMBuildBr(cg->builder, merge_bb);
    then_bb = LLVMGetInsertBlock(cg->builder);

    LLVMAppendExistingBasicBlock(fun, else_bb);
    LLVMPositionBuilderAtEnd(cg->builder, else_bb);

    LLVMValueRef else_v = emit_ir_code(cg, node->cond->else_node);
    assert(else_v);
    LLVMBuildBr(cg->builder, merge_bb);
    else_bb = LLVMGetInsertBlock(cg->builder);
    LLVMAppendExistingBasicBlock(fun, merge_bb);
    LLVMPositionBuilderAtEnd(cg->builder, merge_bb);
    enum type type = get_type(node->cond->then_node->type);
    LLVMValueRef phi_node = LLVMBuildPhi(cg->builder, cg->ops[type].get_type(cg->context, node->cond->then_node->type), "iftmp");
    LLVMAddIncoming(phi_node, &then_v, &then_bb, 1);
    LLVMAddIncoming(phi_node, &else_v, &else_bb, 1);
    return phi_node;
}

LLVMValueRef _emit_struct_node(struct cg_llvm *cg, struct ast_node *node)
{
    assert(node->type);
    // TODO: Notes: get_ext_type having set side effects, so can't be removed
    get_ext_type(cg->context, node->type);
    return 0;
}

LLVMValueRef _emit_for_node(struct cg_llvm *cg, struct ast_node *node)
{
    symbol var_name = node->forloop->var->var->var->ident->name;
    LLVMBasicBlockRef bb = LLVMGetInsertBlock(cg->builder);
    LLVMValueRef fun = LLVMGetBasicBlockParent(bb);

    // TODO: fixme with correct type_exp passed down
    LLVMValueRef alloca = create_alloca(cg->ops[TYPE_INT].get_type(cg->context, 0), 4, fun, string_get(var_name));

    // KSDbgInfo.emitLocation(this);
    LLVMValueRef start_v = emit_ir_code(cg, node->forloop->range->range->start);
    assert(start_v);

    LLVMBuildStore(cg->builder, start_v, alloca);
    LLVMBasicBlockRef loop_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "loop");
    LLVMBuildBr(cg->builder, loop_bb);
    LLVMPositionBuilderAtEnd(cg->builder, loop_bb);

    LLVMValueRef old_alloca = (LLVMValueRef)hashtable_get_p(&cg->varname_2_irvalues, var_name);
    hashtable_set_p(&cg->varname_2_irvalues, var_name, alloca);
    emit_ir_code(cg, node->forloop->body);
    LLVMValueRef step_v;
    if (node->forloop->range->range->step) {
        step_v = emit_ir_code(cg, node->forloop->range->range->step);
        assert(step_v);
    } else {
        step_v = get_int_one(cg->context);
    }
    LLVMValueRef end_cond = emit_ir_code(cg, node->forloop->range->range->end);
    assert(end_cond);

    LLVMValueRef cur_var = LLVMBuildLoad(cg->builder, alloca, string_get(var_name));
    LLVMValueRef next_var = LLVMBuildAdd(cg->builder, cur_var, step_v, "nextvar");
    LLVMBuildStore(cg->builder, next_var, alloca);
    end_cond = LLVMBuildICmp(cg->builder, LLVMIntNE, end_cond, get_int_zero(cg->context, cg->builder), "loopcond");

    LLVMBasicBlockRef after_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "afterloop");

    LLVMBuildCondBr(cg->builder, end_cond, loop_bb, after_bb);
    LLVMPositionBuilderAtEnd(cg->builder, after_bb);

    if (old_alloca)
        hashtable_set_p(&cg->varname_2_irvalues, var_name, old_alloca);
    else
        hashtable_remove_p(&cg->varname_2_irvalues, var_name);

    return LLVMConstNull(cg->ops[TYPE_INT].get_type(cg->context, 0));
}

void create_ir_module(struct cg_llvm *cg,
    const char *module_name)
{
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext(module_name, cg->context);
    LLVMTargetMachineRef target_marchine = create_target_machine(module);
    LLVMTargetDataRef data_layout = LLVMCreateTargetDataLayout(target_marchine);
    LLVMSetModuleDataLayout(module, data_layout);
    cg->module = module;
}

LLVMValueRef _emit_unk_node(struct cg_llvm *cg, struct ast_node *node)
{
    if (!cg || !node)
        return 0;
    return 0;
}

LLVMValueRef emit_ir_code(struct cg_llvm *cg, struct ast_node *node)
{
    if(node->transformed) 
        node = node->transformed;
    LLVMValueRef value = 0;
    switch(node->node_type){
        case TOTAL_NODE:
        case NULL_NODE:
            value = _emit_unk_node(cg, node);
            break;
        case IMPORT_NODE:
        case MEMORY_NODE:
        case RANGE_NODE:
        case ARRAY_INIT_NODE:
        case ARRAY_TYPE_NODE:
        case TYPE_ITEM_NODE:
        case WHILE_NODE:
        case JUMP_NODE:
        case CAST_NODE:
        case VARIANT_TYPE_ITEM_NODE:
        case MATCH_NODE:
        case MATCH_CASE_NODE:
        case WILDCARD_NODE:
        case TYPE_EXPR_ITEM_NODE:
            break;
        case LITERAL_NODE:
            value = _emit_literal_node(cg, node);
            break;
        case IDENT_NODE:
            value = _emit_ident_node(cg, node);
            break;
        case VAR_NODE:
            value = emit_var_node(cg, node);
            break;
        case VARIANT_NODE:
        case RECORD_NODE:
            value = _emit_struct_node(cg, node);
            break;
        case ADT_INIT_NODE:
            value = emit_record_init_node(cg, node, false, "tmp");
            break;
        case UNARY_NODE:
            value = _emit_unary_node(cg, node);
            break;
        case MEMBER_INDEX_NODE:
            value = _emit_accessor_node(cg, node);
            break;
        case ASSIGN_NODE:
        case BINARY_NODE:
            value = _emit_binary_node(cg, node);
            break;
        case IF_NODE:
            value = _emit_condition_node(cg, node);
            break;
        case FOR_NODE:
            value = _emit_for_node(cg, node);
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
    }
    return value;
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

LLVMTypeRef get_llvm_type(struct type_item *type)
{
    assert(g_cg);
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
        struct ast_node *new_sp = *(struct ast_node **)array_get(&cg->base.sema_context->new_specialized_asts, i);
        emit_ir_code(cg, new_sp);
    }
    array_reset(&cg->base.sema_context->new_specialized_asts);
}

void emit_code(struct cg_llvm *cg, struct ast_node *node)
{
    emit_sp_code(cg);
    if (array_size(&cg->base.sema_context->used_builtin_names)) {
        for (size_t i = 0; i < array_size(&cg->base.sema_context->used_builtin_names); i++) {
            symbol built_name = *((symbol *)array_get(&cg->base.sema_context->used_builtin_names, i));
            struct ast_node *n = hashtable_get_p(&cg->base.sema_context->builtin_ast, built_name);
            emit_ir_code(cg, n);
        }
        array_clear(&cg->base.sema_context->used_builtin_names);
    }
}
