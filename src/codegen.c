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

LLVMValueRef _generate_global_var_node(struct code_generator* cg, struct var_node* node,
    bool is_external);
LLVMValueRef _generate_local_var_node(struct code_generator* cg, struct var_node* node);
LLVMValueRef _generate_prototype_node(struct code_generator* cg, struct exp_node* node);
LLVMValueRef _generate_block_node(struct code_generator* cg, struct exp_node* block);

const char* buiiltin_funs[] = {
    "llvm.sin",
    "llvm.cos",
    "llvm.sqrt",
};

struct prototype_node* _create_for_id(LLVMContextRef context, const char* name)
{
    LLVMTypeRef types[1] = { LLVMDoubleTypeInContext(context) };
    unsigned id = LLVMLookupIntrinsicID(name, strlen(name));
    LLVMTypeRef fun = LLVMIntrinsicGetType(context, id, types, 1);
    size_t param_count = LLVMCountParamTypes(fun);
    LLVMTypeRef* params = malloc(param_count * sizeof(LLVMTypeRef));
    LLVMGetParamTypes(fun, params);
    //;llvm::Intrinsic::getName(id, types);
    ARRAY_FUN_PARAM(fun_params);
    struct var_node fun_param;
    for (size_t i = 0; i < param_count; i++) {
        fun_param.var_name = str_format("arg%d", i);      
        fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_DOUBLE);
        array_push(&fun_params, &fun_param);
    }
    string str_name;
    string_init_chars(&str_name, name);
    struct array names = string_split(&str_name, '.');
    //log_info(DEBUG, "get func: %d, name: %s", id, names.back().c_str());
    struct source_loc loc = { 1, 0 };
    struct prototype_node* node = create_prototype_node_default(0, loc,
        string_get(STRING_POINTER(array_back(&names))), &fun_params, (struct type_exp*)create_nullary_type(TYPE_DOUBLE), false);
    string_deinit(&str_name);
    array_deinit(&names);
    free(params);
    return node;
    //return 0;
}

struct array _get_builtins(LLVMContextRef context)
{
    struct array builtins;
    array_init(&builtins, sizeof(struct exp_node*));
    int builtins_num = ARRAY_SIZE(buiiltin_funs);
    for (int i = 0; i < builtins_num; i++) {
        struct prototype_node* proto = _create_for_id(context, buiiltin_funs[i]);
        //analyze(type_env, (struct exp_node*)proto);
        array_push(&builtins, &proto);
    }
    ARRAY_FUN_PARAM(fun_params);
    struct var_node fun_param;
    string_init_chars(&fun_param.var_name, "char");
    fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_INT);
    array_push(&fun_params, &fun_param);
    struct source_loc loc = { 1, 0 };
    struct prototype_node* proto = create_prototype_node_default(0, loc, "print", &fun_params, 
        (struct type_exp*)create_nullary_type(TYPE_INT), false);
    //analyze(type_env, (struct exp_node*)proto);
    array_push(&builtins, &proto);
    //args copied to the prototype node, so not needed to deinit
    return builtins;
}


LLVMValueRef get_int_const(LLVMContextRef context, LLVMBuilderRef builder, void* value)
{
    (void)builder;
    return LLVMConstInt(LLVMInt64TypeInContext(context), *(int*)value, true);
}

LLVMValueRef get_bool_const(LLVMContextRef context, LLVMBuilderRef builder, void* value)
{
    (void)builder;
    return LLVMConstInt(LLVMInt1TypeInContext(context), *(int*)value, true);
}

LLVMValueRef get_double_const(LLVMContextRef context, LLVMBuilderRef builder, void* value)
{
    (void)builder;
    return LLVMConstReal(LLVMDoubleTypeInContext(context), *(double*)value);
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
    return LLVMConstInt(LLVMInt64TypeInContext(context), 0, true);
}

LLVMValueRef get_bool_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstInt(LLVMInt1TypeInContext(context), 0, true);
}

LLVMValueRef get_double_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)builder;
    return LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0);
}

LLVMValueRef get_str_zero(LLVMContextRef context, LLVMBuilderRef builder)
{
    (void)context;
    return get_str_const(context, builder, "");
}

LLVMValueRef get_int_one(LLVMContextRef context)
{
    return LLVMConstInt(LLVMInt64TypeInContext(context), 1, true);
}

LLVMValueRef get_bool_one(LLVMContextRef context)
{
    return LLVMConstInt(LLVMInt1TypeInContext(context), 1, true);
}

LLVMValueRef get_double_one(LLVMContextRef context)
{
    return LLVMConstReal(LLVMDoubleTypeInContext(context), 1.0);
}

struct ops null_ops = { 0 };

LLVMTypeRef get_str_type(LLVMContextRef context)
{
    return LLVMPointerType(LLVMInt8TypeInContext(context), 0);
}

struct ops bool_ops = { 
    LLVMInt1TypeInContext,
    get_bool_const,
    get_bool_zero,
    get_bool_one,
    LLVMBuildAdd,
    LLVMBuildSub,
    LLVMBuildMul,
    LLVMBuildSDiv,
    (cmp_op)LLVMBuildICmp,
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
    LLVMInt64TypeInContext,
    get_int_const,
    get_int_zero,
    get_int_one,
    LLVMBuildAdd,
    LLVMBuildSub,
    LLVMBuildMul,
    LLVMBuildSDiv,
    (cmp_op)LLVMBuildICmp,
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
    (cmp_op)LLVMBuildICmp,
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
    LLVMDoubleTypeInContext,
    get_double_const,
    get_double_zero,
    get_double_one,
    LLVMBuildFAdd,
    LLVMBuildFSub,
    LLVMBuildFMul,
    LLVMBuildFDiv,
    (cmp_op)LLVMBuildFCmp,
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
    cg->ops[TYPE_PRODUCT] = double_ops;

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
    //cg->builtins = _get_builtins(context);
    array_init(&cg->builtins, sizeof(struct exp_node*));
    cg->module = 0;
    _set_bin_ops(cg);
    hashtable_init(&cg->gvs);
    hashtable_init(&cg->protos);
    hashtable_init(&cg->named_values);
    return cg;
}


void cg_free(struct code_generator* cg)
{
    LLVMDisposeBuilder(cg->builder);
    //delete (llvm::legacy::FunctionPassManager*)cg->fpm;
    if (cg->module)
        LLVMDisposeModule(cg->module);
    LLVMContextDispose(cg->context);
    hashtable_deinit(&cg->gvs);
    hashtable_deinit(&cg->protos);
    hashtable_deinit(&cg->named_values);
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

LLVMValueRef _generate_call_node(struct code_generator* cg, struct exp_node* node)
{
    struct call_node* call = (struct call_node*)node;
    LLVMValueRef callee = _get_function(cg, string_get(&call->callee));
    if(!callee)
        return log_info(ERROR, "Unknown function referenced: %s", string_get(&call->callee));
    if (LLVMCountParams(callee) != array_size(&call->args))
        return log_info(ERROR,
            "Incorrect number of arguments passed: callee (prototype "
            "generated in llvm): %lu, calling: %lu",
            LLVMCountParams(callee), array_size(&call->args));
    LLVMValueRef arg_values[array_size(&call->args)];
    for (size_t i = 0, e = array_size(&call->args); i != e; ++i) {
        LLVMValueRef ret = generate_code(cg, *(struct exp_node**)array_get(&call->args, i));
        arg_values[i] = ret;
    }
    return LLVMBuildCall(cg->builder, callee, arg_values, array_size(&call->args), "calltmp");
}

LLVMValueRef _generate_prototype_node(struct code_generator* cg, struct exp_node* node)
{
    struct prototype_node* proto = (struct prototype_node*)node;
    //string *str = (string*)array_get(&proto->args, 0);
    //log_info(DEBUG, "generating prototype node: %s", string_get(&proto->name));
    hashtable_set(&cg->protos, string_get(&proto->name), proto);
    LLVMTypeRef arg_types[array_size(&proto->fun_params)];
    struct type_oper* proto_type = (struct type_oper*)proto->base.type;
    assert(proto_type->base.kind == KIND_OPER);
    struct type_exp* ret = *(struct type_exp**)array_back(&proto_type->args);
    //printf("ret num of types: %zu, %p\n", array_size(&proto_type->args), (void*)ret);
    enum type rettype = get_type(ret);
    //assert(ret && ret->type >= 0 && ret->type < TYPE_TYPES);
    //printf("ret type: %d\n", rettype);
    for (size_t i = 0; i < array_size(&proto->fun_params); i++) {
        struct type_exp* type_exp = *(struct type_exp**)array_get(&proto_type->args, i);
        //assert(type_exp && type_exp->type >= 0 && type_exp->type < TYPE_TYPES);
        arg_types[i] = cg->ops[get_type(type_exp)].get_type(cg->context);
    }
    LLVMTypeRef ret_type = cg->ops[rettype].get_type(cg->context);
    LLVMTypeRef ft = LLVMFunctionType(ret_type, arg_types, array_size(&proto->fun_params), proto->is_variadic);
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
    hashtable_clear(&cg->named_values);
    LLVMValueRef fun = _generate_prototype_node(cg, (struct exp_node*)fun_node->prototype);
    assert (fun);
    // if (is_binary_op(node->prototype)){
    //   log_info(DEBUG, "found a binary op def ! op:%c, prec: %d", get_op_name(node->prototype), node->prototype->precedence);
    //   (*cg->parser->op_precedences)[get_op_name(node->prototype)] =
    //       node->prototype->precedence;
    // }
    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(cg->context, fun, "entry");
    LLVMPositionBuilderAtEnd(cg->builder, bb);
    _create_argument_allocas(cg, fun_node->prototype, fun);
    LLVMValueRef ret_val;
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
    // Function *pfun = (Function*)fun;
    // ((llvm::legacy::FunctionPassManager*)cg->fpm)->run(*pfun);
    // llvm::verifyFunction(*pfun);
    return fun;
}

LLVMValueRef _generate_condition_node(struct code_generator* cg, struct exp_node* node)
{
    // KSDbgInfo.emitLocation(this);
    struct condition_node* cond = (struct condition_node*)node;
    LLVMValueRef cond_v = generate_code(cg, cond->condition_node);
    assert(cond_v);

    //cond_v = LLVMBuildFCmp(builder, LLVMRealONE, cond_v, LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0), "ifcond");
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

// void test(struct code_generator* cg, const char* str)
// {
//   LLVMValueRef str_constant = LLVMConstStringInContext(cg->context, str, strlen(str), false);
//   LLVMGetTypeOfValue(str_constant)
//   LLVMValueRef gVar = LLVMAddGlobal(cg->module, cg->ops[type].get_type(cg->context), var_name);
//   LLVMSetUnnamedAddr(gVar, LLVMGlobalUnnamedAddr);
//   LLVMSetAlignment(gVar, 1);
//   return GV;
// }

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

    return LLVMConstNull(LLVMInt64TypeInContext(cg->context));
}

LLVMValueRef _generate_block_node(struct code_generator* cg, struct exp_node* node)
{
    struct block_node* block = (struct block_node*)node;
    LLVMValueRef codegen;
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

void generate_runtime_module(struct code_generator* cg, struct array* builtins)
{
    for (size_t i = 0; i < array_size(builtins); i++) {
        struct exp_node* node = *(struct exp_node**)array_get(builtins, i);
        //struct prototype_node* proto = (struct prototype_node*)node;
        //string proto_str = to_string(proto->base.type);
        //printf("generating: %s: %s\n", string_get(&proto->name), string_get(&proto_str));
        generate_code(cg, node);
    }
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
