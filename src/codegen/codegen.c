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
#include "codegen.h"
#include "type.h"
#include "llvm-c/Support.h"

LLVMValueRef _generate_global_var_node(struct code_generator* cg, struct var_node* node,
    bool is_external);
LLVMValueRef _generate_local_var_node(struct code_generator* cg, struct var_node* node);
LLVMValueRef _generate_prototype_node(struct code_generator* cg, struct exp_node* node);
LLVMValueRef _generate_block_node(struct code_generator* cg, struct exp_node* block);

LLVMTypeRef get_int_type(LLVMContextRef context)
{
    return LLVMInt32TypeInContext(context);
}

LLVMTypeRef get_bool_type(LLVMContextRef context)
{
    return LLVMInt1TypeInContext(context);
}

LLVMTypeRef get_double_type(LLVMContextRef context)
{
    return LLVMDoubleTypeInContext(context);
}

LLVMTypeRef get_str_type(LLVMContextRef context)
{
    return LLVMPointerType(LLVMInt8TypeInContext(context), 0);
}

const char* buiiltin_funs[] = {
    "llvm.sin",
    "llvm.cos",
    "llvm.sqrt",
};

LLVMValueRef get_int_const(LLVMContextRef context, LLVMBuilderRef builder, void* value)
{
    (void)builder;
    return LLVMConstInt(get_int_type(context), *(int*)value, true);
}

LLVMValueRef get_bool_const(LLVMContextRef context, LLVMBuilderRef builder, void* value)
{
    (void)builder;
    return LLVMConstInt(get_bool_type(context), *(int*)value, true);
}

LLVMValueRef get_double_const(LLVMContextRef context, LLVMBuilderRef builder, void* value)
{
    (void)builder;
    return LLVMConstReal(get_double_type(context), *(double*)value);
}

LLVMValueRef get_str_const(LLVMContextRef context, LLVMBuilderRef builder, void* value)
{
    (void)context;
    const char* str = (const char*)value;
    return LLVMBuildGlobalStringPtr(builder, str, "str.temp");
}

LLVMValueRef get_int_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstInt(get_int_type(context), 0, true);
}

LLVMValueRef get_bool_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstInt(get_bool_type(context), 0, true);
}

LLVMValueRef get_double_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstReal(get_double_type(context), 0.0);
}

LLVMValueRef get_str_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)context;
    return get_str_const(context, builder, "");
}

LLVMValueRef get_int_one(LLVMContextRef context)
{
    return LLVMConstInt(get_int_type(context), 1, true);
}

LLVMValueRef get_bool_one(LLVMContextRef context)
{
    return LLVMConstInt(get_bool_type(context), 1, true);
}

LLVMValueRef get_double_one(LLVMContextRef context)
{
    return LLVMConstReal(get_double_type(context), 1.0);
}

struct ops null_ops = { 0 };

LLVMValueRef int_cmp_op(LLVMBuilderRef builder, unsigned short op, LLVMValueRef lhs, LLVMValueRef rhs,
    const char* name)
{
    return LLVMBuildICmp(builder, op, lhs, rhs, name);
}


LLVMValueRef double_cmp_op(LLVMBuilderRef builder, unsigned short op, LLVMValueRef lhs, LLVMValueRef rhs,
    const char* name)
{
    return LLVMBuildFCmp(builder, op, lhs, rhs, name);
}

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
    int_cmp_op,
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
    int_cmp_op,
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
    int_cmp_op,
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
    double_cmp_op,
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

void _set_bin_ops(struct code_generator* cg)
{
    cg->ops[TYPE_UNK] = double_ops;
    cg->ops[TYPE_GENERIC] = double_ops;
    cg->ops[TYPE_UNIT] = double_ops;
    cg->ops[TYPE_BOOL] = bool_ops;
    cg->ops[TYPE_CHAR] = int_ops;
    cg->ops[TYPE_INT] = int_ops;
    cg->ops[TYPE_DOUBLE] = double_ops;
    cg->ops[TYPE_STRING] = str_ops;
    cg->ops[TYPE_FUNCTION] = double_ops;
    cg->ops[TYPE_EXT] = double_ops;

}
struct code_generator* cg_new(struct parser* parser)
{
    LLVMContextRef context = LLVMContextCreate();
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    struct code_generator* cg = malloc(sizeof(*cg));
    cg->parser = parser;
    cg->context = context;
    cg->builder = LLVMCreateBuilderInContext(context);
    hashset_init(&cg->builtins);
    cg->module = 0;
    _set_bin_ops(cg);
    hashtable_init(&cg->gvs);
    hashtable_init(&cg->protos);
    hashtable_init(&cg->named_values);
    hashtable_init(&cg->specialized_nodes);
    return cg;
}


void cg_free(struct code_generator* cg)
{
    LLVMDisposeBuilder(cg->builder);
    //delete (llvm::legacy::FunctionPassManager*)cg->fpm;
    if (cg->module)
        LLVMDisposeModule(cg->module);
    LLVMContextDispose(cg->context);
    hashtable_deinit(&cg->specialized_nodes);
    hashtable_deinit(&cg->gvs);
    hashtable_deinit(&cg->protos);
    hashtable_deinit(&cg->named_values);
    hashset_deinit(&cg->builtins);
    free(cg);
    //LLVMShutdown();
}

LLVMValueRef _get_function(struct code_generator* cg, const char* name)
{
    LLVMValueRef f = LLVMGetNamedFunction(cg->module, name);
    if (f)
        return f;

    struct exp_node* fp = (struct exp_node*)hashtable_get(&cg->protos, name);
    if (fp)
        return _generate_prototype_node(cg, fp);

    return 0;
}

LLVMValueRef _get_named_global(struct code_generator* cg, const char* name)
{
    return LLVMGetNamedGlobal(cg->module, name);
}

LLVMValueRef _get_global_variable(struct code_generator* cg, const char* name)
{
    LLVMValueRef gv = _get_named_global(cg, name);
    if (gv)
        return gv;
    struct var_node* var = (struct var_node*)hashtable_get(&cg->gvs, name); //.find(name);
    if (var) {
        enum type type = get_type(var->base.type);
        gv = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context), name);
        LLVMSetExternallyInitialized(gv, true);
        return gv;
    }

    return 0;
}

LLVMValueRef _create_entry_block_alloca(
    LLVMTypeRef type,
    LLVMValueRef fun,
    const char* var_name)
{
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMBasicBlockRef bb = LLVMGetEntryBasicBlock(fun);
    LLVMPositionBuilder(builder, bb, LLVMGetFirstInstruction(bb));
    LLVMValueRef alloca = LLVMBuildAlloca(builder, type, var_name);
    LLVMDisposeBuilder(builder);
    return alloca;
}

void _create_argument_allocas(struct code_generator* cg, struct prototype_node* node,
    LLVMValueRef fun)
{
    struct type_oper* proto_type = (struct type_oper*)node->base.type;
    //assert (LLVMCountParams(fun) == array_size(&proto_type->args) - 1);
    for (unsigned i = 0; i < LLVMCountParams(fun); i++) {
        struct var_node* param = (struct var_node*)array_get(&node->fun_params, i);
        struct type_exp* type_exp = *(struct type_exp**)array_get(&proto_type->args, i);
        enum type type = get_type(type_exp);
        //assert(type_exp && type_exp->type >= 0 && type_exp->type < TYPE_TYPES);
        //TODO: fix the inconsistency enum type type = get_type(param->base.type);        
        LLVMValueRef alloca = _create_entry_block_alloca(
            cg->ops[type].get_type(cg->context), fun, string_get(&param->var_name));

        // Create a debug descriptor for the variable.
        /*DIScope *Scope = KSDbgInfo.LexicalBlocks.back();
    DIFile *Unit = DBuilder->createFile(KSDbgInfo.TheCU->getFilename(),
                                        KSDbgInfo.TheCU->getDirectory());
    DILocalVariable *D = DBuilder->createLocalVariable(
                                                       dwarf::DW_TAG_arg_variable,
    Scope, Args[Idx], Unit, Line, KSDbgInfo.getDoubleTy(), Idx);

    DBuilder->insertDeclare(Alloca, D, DBuilder->createExpression(),
                            DebugLoc::get(Line, 0, Scope),
                            Builder.GetInsertBlock());
    */

        LLVMBuildStore(cg->builder, LLVMGetParam(fun, i), alloca);
        hashtable_set(&cg->named_values, string_get(&param->var_name), alloca);
    }
}

LLVMValueRef _generate_literal_node(struct code_generator* cg, struct exp_node* node)
{
    assert(node->type);
    enum type type = get_type(node->type);
    void *value = 0;
    if (is_int_type(type))
        value = &((struct literal_node*)node)->int_val;
    else if(type==TYPE_DOUBLE)
        value = &((struct literal_node*)node)->double_val;
    else if(type==TYPE_STRING){
        value = (void*)((struct literal_node*)node)->str_val;
    }
    return cg->ops[type].get_const(cg->context, cg->builder, value);
}

LLVMValueRef _generate_ident_node(struct code_generator* cg, struct exp_node* node)
{
    struct ident_node* ident = (struct ident_node*)node;
    const char* idname = string_get(&ident->name);
    LLVMValueRef v = (LLVMValueRef)hashtable_get(&cg->named_values, idname);
    if (!v) {
        LLVMValueRef gVar = _get_global_variable(cg, idname);
        assert(gVar);
        return LLVMBuildLoad(cg->builder, gVar, idname);
    }
    enum type id_type = get_type(ident->base.type);
    return LLVMBuildLoad2(cg->builder, cg->ops[id_type].get_type(cg->context),  v, idname);
}

LLVMValueRef _generate_unary_node(struct code_generator* cg, struct exp_node* node)
{
    struct unary_node* unary = (struct unary_node*)node;
    LLVMValueRef operand_v = generate_code(cg, unary->operand);
    assert(operand_v);
    if (string_eq_chars(&unary->op, "+"))
        return operand_v;
    else if (string_eq_chars(&unary->op, "-")){
        return cg->ops->neg_op(cg->builder, operand_v, "negtmp");
    } else if (string_eq_chars(&unary->op, "!")){
        LLVMValueRef ret = cg->ops->not_op(cg->builder, operand_v, "nottmp");
        return LLVMBuildZExt(cg->builder, ret, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
    }
    string fname;
    string_init_chars(&fname, "unary");
    string_add(&fname, &unary->op);
    LLVMValueRef fun = _get_function(cg, string_get(&fname));
    if (fun == 0)
        return log_info(ERROR, "Unknown unary operator");

    // KSDbgInfo.emitLocation(this);
    return LLVMBuildCall(cg->builder, fun, &operand_v, 1, "unop");
}

LLVMValueRef _generate_binary_node(struct code_generator* cg, struct exp_node* node)
{
    struct binary_node* bin = (struct binary_node*)node;
    LLVMValueRef lv = generate_code(cg, bin->lhs);
    LLVMValueRef rv = generate_code(cg, bin->rhs);
    //assert(LLVMGetValueKind(lv) == LLVMGetValueKind(rv));
    assert(bin->lhs->type && prune(bin->lhs->type)->type == prune(bin->rhs->type)->type);
    assert(lv && rv);
    assert(LLVMTypeOf(lv) == LLVMTypeOf(rv));
    struct ops* ops = &cg->ops[prune(bin->lhs->type)->type];
    if (string_eq_chars(&bin->op, "+"))
        return ops->add(cg->builder, lv, rv, "addtmp");
    else if (string_eq_chars(&bin->op, "-"))
        return ops->sub(cg->builder, lv, rv, "subtmp");
    else if (string_eq_chars(&bin->op, "*"))
        return ops->mul(cg->builder, lv, rv, "multmp");
    else if (string_eq_chars(&bin->op, "/"))
        return ops->div(cg->builder, lv, rv, "divtmp");
    else if (string_eq_chars(&bin->op, "%"))
        return ops->rem(cg->builder, lv, rv, "remtmp");
    else if (string_eq_chars(&bin->op, "<")) {
        lv = ops->cmp(cg->builder, ops->cmp_lt, lv, rv, "cmplttmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else if (string_eq_chars(&bin->op, ">")) {
        lv = ops->cmp(cg->builder, ops->cmp_gt, lv, rv, "cmpgttmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else if (string_eq_chars(&bin->op, "==")) {
        lv = ops->cmp(cg->builder, ops->cmp_eq, lv, rv, "cmpeqtmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else if (string_eq_chars(&bin->op, "!=")) {
        lv = ops->cmp(cg->builder, ops->cmp_neq, lv, rv, "cmpneqtmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else if (string_eq_chars(&bin->op, "<=")) {
        lv = ops->cmp(cg->builder, ops->cmp_le, lv, rv, "cmpletmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else if (string_eq_chars(&bin->op, ">=")) {
        lv = ops->cmp(cg->builder, ops->cmp_ge, lv, rv, "cmpgetmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else if (string_eq_chars(&bin->op, "||")) {
        lv = ops->or_op(cg->builder, lv, rv, "ortmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else if (string_eq_chars(&bin->op, "&&")) {
        lv = ops->and_op(cg->builder, lv, rv, "andtmp");
        lv = LLVMBuildZExt(cg->builder, lv, cg->ops[TYPE_INT].get_type(cg->context), "ret_val_int");
        return lv;
    } else {
        string f_name;
        string_init_chars(&f_name, "binary");
        string_add(&f_name, &bin->op);
        LLVMValueRef fun = _get_function(cg, string_get(&f_name));
        assert(fun && "binary operator not found!");
        LLVMValueRef ops[2] = { lv, rv };
        return LLVMBuildCall(cg->builder, fun, ops, 2, "binop");
    }
}

LLVMValueRef _generate_prototype_node(struct code_generator* cg, struct exp_node* node)
{
    struct prototype_node* proto = (struct prototype_node*)node;
    hashtable_set(&cg->protos, string_get(&proto->name), proto);
    size_t param_count = array_size(&proto->fun_params);
    if(proto->is_variadic)
        param_count --;
    LLVMTypeRef arg_types[param_count];
    struct type_oper* proto_type = (struct type_oper*)proto->base.type;
    assert(proto_type->base.kind == KIND_OPER);
    struct type_exp* ret = *(struct type_exp**)array_back(&proto_type->args);
    enum type rettype = get_type(ret);
    for (size_t i = 0; i < param_count; i++) {
        struct type_exp* type_exp = *(struct type_exp**)array_get(&proto_type->args, i);
        arg_types[i] = cg->ops[get_type(type_exp)].get_type(cg->context);
    }
    LLVMTypeRef ret_type = cg->ops[rettype].get_type(cg->context);
    LLVMTypeRef ft = LLVMFunctionType(ret_type, arg_types, param_count, proto->is_variadic);
    LLVMValueRef fun = LLVMAddFunction(cg->module, string_get(&proto->name), ft);
    for (unsigned i = 0; i < LLVMCountParams(fun); i++) {
        LLVMValueRef param = LLVMGetParam(fun, i);
        struct var_node* fun_param = (struct var_node*)array_get(&proto->fun_params, i);
        LLVMSetValueName2(param, string_get(&fun_param->var_name), string_size(&fun_param->var_name));
    }
    return fun;
}

LLVMValueRef _generate_function_node(struct code_generator* cg, struct exp_node* node)
{
    struct function_node* fun_node = (struct function_node*)node;
    if(is_generic(node->type)){
        return 0;
    }
    hashtable_clear(&cg->named_values);
    LLVMValueRef fun = _generate_prototype_node(cg, (struct exp_node*)fun_node->prototype);
    assert (fun);
    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(cg->context, fun, "entry");
    LLVMPositionBuilderAtEnd(cg->builder, bb);
    _create_argument_allocas(cg, fun_node->prototype, fun);
    LLVMValueRef ret_val = 0;
    for (size_t i = 0; i < array_size(&fun_node->body->nodes); i++) {
        struct exp_node* stmt = *(struct exp_node**)array_get(&fun_node->body->nodes, i);
        ret_val = generate_code(cg, stmt);
    }
    if (!ret_val) {
        struct type_exp* ret_type = get_ret_type(fun_node);
        enum type type = get_type(ret_type);
        ret_val = cg->ops[type].get_zero(cg->context, cg->builder);
    }
    assert(ret_val);
    LLVMBuildRet(cg->builder, ret_val);
    return fun;
}

LLVMValueRef _generate_call_node(struct code_generator* cg, struct exp_node* node)
{
    struct call_node* call = (struct call_node*)node;
    bool req_spec_cg = !string_eq_chars(&call->specialized_callee, "");
    string* callee_name = req_spec_cg? &call->specialized_callee : &call->callee;
    LLVMValueRef callee = _get_function(cg, string_get(callee_name));
    assert(callee);
    LLVMValueRef arg_values[array_size(&call->args)];
    for (size_t i = 0, e = array_size(&call->args); i != e; ++i) {
        struct exp_node* arg = *(struct exp_node**)array_get(&call->args, i);
        arg_values[i] = generate_code(cg, arg);
    }
    return LLVMBuildCall(cg->builder, callee, arg_values, array_size(&call->args), "calltmp");
}

LLVMValueRef _generate_condition_node(struct code_generator* cg, struct exp_node* node)
{
    // KSDbgInfo.emitLocation(this);
    struct condition_node* cond = (struct condition_node*)node;
    LLVMValueRef cond_v = generate_code(cg, cond->condition_node);
    assert(cond_v);

    cond_v = LLVMBuildICmp(cg->builder, LLVMIntNE, cond_v, cg->ops[TYPE_INT].get_zero(cg->context, cg->builder), "ifcond");

    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); 

    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "then");
    LLVMBasicBlockRef else_bb = LLVMCreateBasicBlockInContext(cg->context, "else");
    LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(cg->context, "ifcont");

    LLVMBuildCondBr(cg->builder, cond_v, then_bb, else_bb);
    LLVMPositionBuilderAtEnd(cg->builder, then_bb);

    LLVMValueRef then_v = generate_code(cg, cond->then_node);
    assert(then_v);
    LLVMBuildBr(cg->builder, merge_bb);
    then_bb = LLVMGetInsertBlock(cg->builder);

    LLVMAppendExistingBasicBlock(fun, else_bb);
    LLVMPositionBuilderAtEnd(cg->builder, else_bb);

    LLVMValueRef else_v = generate_code(cg, cond->else_node);
    assert(else_v);
    LLVMBuildBr(cg->builder, merge_bb);
    else_bb = LLVMGetInsertBlock(cg->builder);
    LLVMAppendExistingBasicBlock(fun, merge_bb);
    LLVMPositionBuilderAtEnd(cg->builder, merge_bb);
    enum type type = get_type(cond->then_node->type);
    LLVMValueRef phi_node = LLVMBuildPhi(cg->builder, cg->ops[type].get_type(cg->context), "iftmp");
    LLVMAddIncoming(phi_node, &then_v, &then_bb, 1);
    LLVMAddIncoming(phi_node, &else_v, &else_bb, 1);
    return phi_node;
}

LLVMValueRef _generate_global_var_node(struct code_generator* cg, struct var_node* node,
    bool is_external)
{
    const char* var_name = string_get(&node->var_name);
    LLVMValueRef gVar = _get_named_global(cg, var_name);//LLVMGetNamedGlobal(cg->module, var_name);
    LLVMValueRef exp = generate_code(cg, node->init_value);
    assert(node->base.type);
    enum type type = get_type(node->base.type);
    if (hashtable_in(&cg->gvs, var_name) && !gVar && !is_external)
        is_external = true;
    if (!gVar) {
        if (is_external) {
            gVar = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context), var_name);
            LLVMSetExternallyInitialized(gVar, true);   
        } else {
            hashtable_set(&cg->gvs, var_name, node);
            gVar = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context), var_name);
            LLVMSetExternallyInitialized(gVar, true);
            LLVMSetInitializer(gVar, cg->ops[type].get_zero(cg->context, cg->builder));
        }
    }
    LLVMBuildStore(cg->builder, exp, gVar);
    return 0;
}

LLVMValueRef _generate_var_node(struct code_generator* cg, struct exp_node* node)
{
    struct var_node* var = (struct var_node*)node;
    if (!var->base.parent)
        return _generate_global_var_node(cg, var, false);
    else
        return _generate_local_var_node(cg, var);
}

LLVMValueRef _generate_type_node(struct code_generator* cg, struct exp_node* node)
{
    (void)cg; (void)node;
    return 0;
}

LLVMValueRef _generate_local_var_node(struct code_generator* cg, struct var_node* node)
{
    // fprintf(stderr, "_generate_var_node:1 %lu!, %lu\n", node->var_names.size(),
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(cg->builder)); // builder->GetInsertBlock()->getParent();
    // fprintf(stderr, "_generate_var_node:2 %lu!\n", node->var_names.size());
    const char* var_name = string_get(&node->var_name);
    // log_info(DEBUG, "local var cg: %s", var_name.c_str());
    assert(node->init_value);
    LLVMValueRef init_val = generate_code(cg, node->init_value);
    assert(init_val);
    enum type type = get_type(node->base.type);        
    LLVMValueRef alloca = _create_entry_block_alloca(cg->ops[type].get_type(cg->context), fun, var_name);
    LLVMBuildStore(cg->builder, init_val, alloca);
    hashtable_set(&cg->named_values, var_name, alloca);
    return 0;
    // KSDbgInfo.emitLocation(this);
}

LLVMValueRef _generate_for_node(struct code_generator* cg, struct exp_node* node)
{
    struct for_node* forn = (struct for_node*)node;
    const char* var_name = string_get(&forn->var_name);
    LLVMBasicBlockRef bb = LLVMGetInsertBlock(cg->builder);
    LLVMValueRef fun = LLVMGetBasicBlockParent(bb);

    LLVMValueRef alloca = _create_entry_block_alloca(cg->ops[TYPE_INT].get_type(cg->context), fun, var_name);

    // KSDbgInfo.emitLocation(this);
    LLVMValueRef start_v = generate_code(cg, forn->start);
    assert (start_v);

    LLVMBuildStore(cg->builder, start_v, alloca);
    LLVMBasicBlockRef loop_bb = LLVMAppendBasicBlockInContext(cg->context, fun, "loop");
    LLVMBuildBr(cg->builder, loop_bb);
    LLVMPositionBuilderAtEnd(cg->builder, loop_bb);

    LLVMValueRef old_alloca = (LLVMValueRef)hashtable_get(&cg->named_values, var_name);
    hashtable_set(&cg->named_values, var_name, alloca);
    generate_code(cg, forn->body);
    LLVMValueRef step_v;
    if (forn->step) {
        step_v = generate_code(cg, forn->step);
        assert (step_v);
    } else {
        step_v = get_int_one(cg->context);
    }
    LLVMValueRef end_cond = generate_code(cg, forn->end);
    assert (end_cond);

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

    return LLVMConstNull(cg->ops[TYPE_INT].get_type(cg->context));
}

LLVMValueRef _generate_block_node(struct code_generator* cg, struct exp_node* node)
{
    struct block_node* block = (struct block_node*)node;
    LLVMValueRef codegen = 0;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node* exp = *(struct exp_node**)array_get(&block->nodes, i);
        codegen = generate_code(cg, exp);
    }
    return codegen;
}

void create_module_and_pass_manager(struct code_generator* cg,
    const char* module_name)
{
    LLVMModuleRef moduleRef = LLVMModuleCreateWithNameInContext(module_name, cg->context);
    LLVMTargetMachineRef target_marchine = create_target_machine(moduleRef);
    LLVMTargetDataRef data_layout = LLVMCreateTargetDataLayout(target_marchine);
    LLVMSetModuleDataLayout(moduleRef, data_layout);
    cg->module = moduleRef;
}

void generate_runtime_module(struct code_generator* cg)
{
    (void)cg; 
}

LLVMValueRef _generate_unk_node(struct code_generator* cg, struct exp_node* node)
{
    if (!cg || !node)
        return 0;

    return 0;
}

LLVMValueRef (*cg_fp[])(struct code_generator*, struct exp_node*) = {
    _generate_unk_node,
    _generate_literal_node,
    _generate_ident_node,
    _generate_var_node,
    _generate_type_node,
    _generate_unary_node,
    _generate_binary_node,
    _generate_condition_node,
    _generate_for_node,
    _generate_call_node,
    _generate_prototype_node,
    _generate_function_node,
    _generate_block_node
};

LLVMValueRef generate_code(struct code_generator* cg, struct exp_node* node)
{
    return cg_fp[node->node_type](cg, node);
}

LLVMTargetMachineRef create_target_machine(LLVMModuleRef module)
{
    char* target_triple = LLVMGetDefaultTargetTriple();
    LLVMSetTarget(module, target_triple);
    char* error;
    LLVMTargetRef target;
    if (LLVMGetTargetFromTriple(target_triple, &target, &error)) {
        log_info(ERROR, "error in creating target machine: %s", error);
        return 0;
    }
    const char* cpu = "generic";
    const char* features = "";
    LLVMCodeGenOptLevel opt = LLVMCodeGenLevelDefault;
    LLVMRelocMode rm = LLVMRelocDefault;
    LLVMCodeModel cm = LLVMCodeModelDefault;
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, target_triple, cpu, features, opt, rm, cm);
    LLVMSetModuleDataLayout(module, LLVMCreateTargetDataLayout(target_machine));
    return target_machine;
}
