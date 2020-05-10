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

void* _generate_global_var_node(struct code_generator* cg, struct var_node* node,
    bool is_external);
void* _generate_local_var_node(struct code_generator* cg, struct var_node* node);
void* _generate_prototype_node(struct code_generator* cg, struct exp_node* node);
void* _generate_block_node(struct code_generator* cg, struct exp_node* block);

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
        string_get(STRING_POINTER(array_back(&names))), &fun_params, (struct type_exp*)create_nullary_type(TYPE_DOUBLE));
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
    fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_DOUBLE);
    array_push(&fun_params, &fun_param);
    struct source_loc loc = { 1, 0 };
    struct prototype_node* proto = create_prototype_node_default(0, loc, "print", &fun_params, (struct type_exp*)create_nullary_type(TYPE_UNIT));
    //analyze(type_env, (struct exp_node*)proto);
    array_push(&builtins, &proto);
    //args copied to the prototype node, so not needed to deinit
    return builtins;
}

struct binary_ops null_ops = { 0 };

struct binary_ops int_ops = { 
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
    LLVMIntSGE 
};

struct binary_ops float_ops = {
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
};

void _set_bin_ops(struct code_generator* cg)
{
    cg->bin_ops[TYPE_UNK] = float_ops;
    cg->bin_ops[TYPE_GENERIC] = float_ops;
    cg->bin_ops[TYPE_UNIT] = float_ops;
    cg->bin_ops[TYPE_BOOL] = float_ops;
    cg->bin_ops[TYPE_CHAR] = float_ops;
    cg->bin_ops[TYPE_INT] = float_ops;
    cg->bin_ops[TYPE_DOUBLE] = float_ops;
    cg->bin_ops[TYPE_FUNCTION] = float_ops;
    cg->bin_ops[TYPE_PRODUCT] = float_ops;

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
    cg->builtins = _get_builtins(context);
    cg->module = 0;
    _set_bin_ops(cg);
    hashtable_init(&cg->gvs);
    hashtable_init(&cg->protos);
    hashtable_init(&cg->named_values);
    return cg;
}


void cg_free(struct code_generator* cg)
{
    LLVMDisposeBuilder((LLVMBuilderRef)cg->builder);
    //delete (llvm::legacy::FunctionPassManager*)cg->fpm;
    if (cg->module)
        LLVMDisposeModule((LLVMModuleRef)cg->module);
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
        return (LLVMValueRef)_generate_prototype_node(cg, fp);

    return 0;
}

LLVMValueRef _get_global_variable(struct code_generator* cg, const char* name)
{
    LLVMValueRef gv = LLVMGetNamedGlobal(cg->module, name);
    if (gv)
        return gv;
    struct var_node* fgv = (struct var_node*)hashtable_get(&cg->gvs, name); //.find(name);
    if (fgv) {
        return (LLVMValueRef)_generate_global_var_node(cg, fgv, true);
    }

    return 0;
}

LLVMValueRef _create_entry_block_alloca(struct code_generator* cg,
    LLVMValueRef fun,
    const char* var_name)
{
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMBasicBlockRef bb = LLVMGetEntryBasicBlock(fun);
    LLVMPositionBuilder(builder, bb, LLVMGetFirstInstruction(bb));
    LLVMValueRef alloca = LLVMBuildAlloca(builder, LLVMDoubleTypeInContext(cg->context), var_name);
    LLVMDisposeBuilder(builder);
    return alloca;
}

void _create_argument_allocas(struct code_generator* cg, struct prototype_node* node,
    LLVMValueRef fun)
{
    for (unsigned i = 0; i < LLVMCountParams(fun); i++) {
        struct var_node* param = (struct var_node*)array_get(&node->fun_params, i);
        LLVMValueRef alloca = _create_entry_block_alloca(cg, fun, string_get(&param->var_name));

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

        LLVMBuildStore((LLVMBuilderRef)cg->builder, LLVMGetParam(fun, i), alloca);
        hashtable_set(&cg->named_values, string_get(&param->var_name), alloca);
    }
}

void* _generate_num_node(struct code_generator* cg, struct exp_node* node)
{
    if (node->type && is_int_type(node->type->type)) {
        int value = (int)((struct num_node*)node)->double_val;
        return LLVMConstInt(LLVMInt64TypeInContext(cg->context), value, true);
    }
    return LLVMConstReal(LLVMDoubleTypeInContext(cg->context), ((struct num_node*)node)->double_val);
}

void* _generate_ident_node(struct code_generator* cg, struct exp_node* node)
{
    struct ident_node* ident = (struct ident_node*)node;
    const char* idname = string_get(&ident->name);
    LLVMValueRef v = (LLVMValueRef)hashtable_get(&cg->named_values, idname);
    if (!v) {
        LLVMValueRef gVar = _get_global_variable(cg, idname);
        if (gVar) {
            return LLVMBuildLoad(cg->builder, gVar, idname);
        } else {
            log_info(ERROR, "Unknown variable name: %s", idname);
            return 0;
        }
    }
    return LLVMBuildLoad(cg->builder, v, idname);
}

void* _generate_binary_node(struct code_generator* cg, struct exp_node* node)
{
    struct binary_node* bin = (struct binary_node*)node;
    LLVMValueRef lv = (LLVMValueRef)generate_code(cg, bin->lhs);
    LLVMValueRef rv = (LLVMValueRef)generate_code(cg, bin->rhs);
    if (!lv || !rv)
        return 0;
    assert(bin->lhs->type);
    struct binary_ops* ops = &cg->bin_ops[bin->lhs->type->type];
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
        //return lv;
        return LLVMBuildUIToFP(cg->builder, lv, LLVMDoubleTypeInContext(cg->context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, ">")) {
        lv = ops->cmp(cg->builder, ops->cmp_gt, lv, rv, "cmpgttmp");
        return LLVMBuildUIToFP(cg->builder, lv, LLVMDoubleTypeInContext(cg->context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, "==")) {
        lv = ops->cmp(cg->builder, ops->cmp_eq, lv, rv, "cmpeqtmp");
        return LLVMBuildUIToFP(cg->builder, lv, LLVMDoubleTypeInContext(cg->context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, "!=")) {
        lv = ops->cmp(cg->builder, ops->cmp_neq, lv, rv, "cmpneqtmp");
        return LLVMBuildUIToFP(cg->builder, lv, LLVMDoubleTypeInContext(cg->context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, "<=")) {
        lv = ops->cmp(cg->builder, ops->cmp_le, lv, rv, "cmpletmp");
        return LLVMBuildUIToFP(cg->builder, lv, LLVMDoubleTypeInContext(cg->context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, ">=")) {
        lv = ops->cmp(cg->builder, ops->cmp_ge, lv, rv, "cmpgetmp");
        return LLVMBuildUIToFP(cg->builder, lv, LLVMDoubleTypeInContext(cg->context),
            "booltmp");
    } else {
        string f_name;
        string_init_chars(&f_name, "binary");
        string_add(&f_name, &bin->op);
        LLVMValueRef fun = _get_function(cg, string_get(&f_name));
        assert(fun && "binary operator not found!");
        LLVMValueRef ops[2] = { (LLVMValueRef)lv, (LLVMValueRef)rv };
        return LLVMBuildCall(cg->builder, fun, ops, 2, "binop");
    }
}

void* _generate_call_node(struct code_generator* cg, struct exp_node* node)
{
    struct call_node* call = (struct call_node*)node;
    LLVMValueRef callee = _get_function(cg, string_get(&call->callee));
    if (!callee)
        return log_info(ERROR, "Unknown function referenced: %s", string_get(&call->callee));
    if (LLVMCountParams(callee) != array_size(&call->args))
        return log_info(ERROR,
            "Incorrect number of arguments passed: callee (prototype "
            "generated in llvm): %lu, calling: %lu",
            LLVMCountParams(callee), array_size(&call->args));

    LLVMValueRef arg_values[array_size(&call->args)];
    for (unsigned long i = 0, e = array_size(&call->args); i != e; ++i) {
        LLVMValueRef ret = (LLVMValueRef)generate_code(cg, *(struct exp_node**)array_get(&call->args, i));
        if (!ret)
            return 0;
        arg_values[i] = ret;
    }
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    return LLVMBuildCall(builder, callee, arg_values, array_size(&call->args), "calltmp");
}

void* _generate_prototype_node(struct code_generator* cg, struct exp_node* node)
{
    struct prototype_node* proto = (struct prototype_node*)node;
    //string *str = (string*)array_get(&proto->args, 0);
    //log_info(DEBUG, "generating prototype node: %s", string_get(str));
    hashtable_set(&cg->protos, string_get(&proto->name), proto);
    LLVMTypeRef arg_types[array_size(&proto->fun_params)];
    struct type_oper* proto_type = (struct type_oper*)proto->base.type;
    assert(proto_type->base.kind == KIND_OPER);
    struct type_exp* ret = (struct type_exp*)array_back(&proto_type->args);
    assert(ret);
    for (size_t i = 0; i < array_size(&proto->fun_params); i++) {
        struct type_exp* type = *(struct type_exp**)array_get(&proto_type->args, i);
        if(is_int_type(type->type))
             arg_types[i] = LLVMInt64TypeInContext(cg->context);
        else
             arg_types[i] = LLVMDoubleTypeInContext(cg->context);
    }
    LLVMTypeRef ret_type;
    if(is_int_type(ret->type))
        ret_type = LLVMInt64TypeInContext(cg->context);
    else
        ret_type = LLVMDoubleTypeInContext(cg->context);
    LLVMTypeRef ft = LLVMFunctionType(ret_type, arg_types, array_size(&proto->fun_params), false);
    LLVMValueRef fun = LLVMAddFunction((LLVMModuleRef)cg->module, string_get(&proto->name), ft);
    for (unsigned i = 0; i < LLVMCountParams(fun); i++) {
        LLVMValueRef param = LLVMGetParam(fun, i);
        struct var_node* fun_param = (struct var_node*)array_get(&proto->fun_params, i);
        LLVMSetValueName2(param, string_get(&fun_param->var_name), string_size(&fun_param->var_name));
    }
    return fun;
}

void* _generate_function_node(struct code_generator* cg, struct exp_node* node)
{
    struct function_node* fun_node = (struct function_node*)node;
    hashtable_clear(&cg->named_values);
    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMValueRef fun = (LLVMValueRef)_generate_prototype_node(cg, (struct exp_node*)fun_node->prototype);
    if (!fun)
        return 0;
    // if (is_binary_op(node->prototype)){
    //   log_info(DEBUG, "found a binary op def ! op:%c, prec: %d", get_op_name(node->prototype), node->prototype->precedence);
    //   (*cg->parser->op_precedences)[get_op_name(node->prototype)] =
    //       node->prototype->precedence;
    // }
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(context, fun, "entry");
    LLVMPositionBuilderAtEnd(builder, bb);
    _create_argument_allocas(cg, fun_node->prototype, fun);
    LLVMValueRef ret_val;
    for (size_t i = 0; i < array_size(&fun_node->body->nodes); i++) {
        struct exp_node* stmt = *(struct exp_node**)array_get(&fun_node->body->nodes, i);
        ret_val = (LLVMValueRef)generate_code(cg, stmt);
    }
    if (!ret_val) {
        struct type_exp* ret_type = get_ret_type(fun_node);
        if (is_int_type(ret_type->type))
            ret_val = LLVMConstInt(LLVMInt64TypeInContext(context), 0, true);
        else
            ret_val = LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0);
    }
    if (ret_val) {
        LLVMBuildRet(builder, ret_val);
        // Function *pfun = (Function*)fun;
        // ((llvm::legacy::FunctionPassManager*)cg->fpm)->run(*pfun);
        // llvm::verifyFunction(*pfun);
        return fun;
    }
    LLVMDeleteFunction(fun);
    // if (is_binary_op(node->prototype))
    //   cg->parser->op_precedences->erase(get_op_name(node->prototype));
    return 0;
}

void* _generate_unary_node(struct code_generator* cg, struct exp_node* node)
{
    struct unary_node* unary = (struct unary_node*)node;
    LLVMValueRef operand_v = (LLVMValueRef)generate_code(cg, unary->operand);
    if (operand_v == 0)
        return 0;
    string fname;
    string_init_chars(&fname, "unary");
    string_add(&fname, &unary->op);
    LLVMValueRef fun = _get_function(cg, string_get(&fname));
    if (fun == 0)
        return log_info(ERROR, "Unknown unary operator");

    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    // KSDbgInfo.emitLocation(this);
    return LLVMBuildCall(builder, fun, &operand_v, 1, "unop");
}

void* _generate_condition_node(struct code_generator* cg, struct exp_node* node)
{
    // KSDbgInfo.emitLocation(this);
    struct condition_node* cond = (struct condition_node*)node;
    LLVMValueRef cond_v = (LLVMValueRef)generate_code(cg, cond->condition_node);
    if (cond_v == 0)
        return 0;

    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    LLVMContextRef context = (LLVMContextRef)cg->context;
    cond_v = LLVMBuildFCmp(builder, LLVMRealONE, cond_v, LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0), "ifcond");

    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder)); 

    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlockInContext(context, fun, "then");
    LLVMBasicBlockRef else_bb = LLVMCreateBasicBlockInContext(context, "else");
    LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(context, "ifcont");

    LLVMBuildCondBr(builder, cond_v, then_bb, else_bb);
    LLVMPositionBuilderAtEnd(builder, then_bb);

    LLVMValueRef then_v = (LLVMValueRef)generate_code(cg, cond->then_node);
    if (then_v == 0)
        return 0;

    LLVMBuildBr(builder, merge_bb);
    then_bb = LLVMGetInsertBlock(builder);

    LLVMAppendExistingBasicBlock(fun, else_bb);
    LLVMPositionBuilderAtEnd(builder, else_bb);

    LLVMValueRef else_v = (LLVMValueRef)generate_code(cg, cond->else_node);
    if (else_v == 0)
        return 0;

    LLVMBuildBr(builder, merge_bb);
    else_bb = LLVMGetInsertBlock(builder);
    LLVMAppendExistingBasicBlock(fun, merge_bb);
    LLVMPositionBuilderAtEnd(builder, merge_bb);
    LLVMValueRef phi_node = LLVMBuildPhi(builder, LLVMDoubleTypeInContext(context), "iftmp");
    LLVMAddIncoming(phi_node, &then_v, &then_bb, 1);
    LLVMAddIncoming(phi_node, &else_v, &else_bb, 1);
    return phi_node;
}

void* _generate_global_var_node(struct code_generator* cg, struct var_node* node,
    bool is_external)
{
    const char* var_name = string_get(&node->var_name);
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMModuleRef module = (LLVMModuleRef)cg->module;
    LLVMValueRef gVar = LLVMGetNamedGlobal(module, var_name);
    LLVMValueRef exp = (LLVMValueRef)generate_code(cg, node->init_value);
    if (!gVar) {
        if (is_external) {
            gVar = LLVMAddGlobal(module, LLVMDoubleTypeInContext(context), var_name);
            LLVMSetExternallyInitialized(gVar, true);
            return gVar;
        } else {
            hashtable_set(&cg->gvs, var_name, node);
            gVar = LLVMAddGlobal(module, LLVMDoubleTypeInContext(context), var_name);
            LLVMSetExternallyInitialized(gVar, true);
            LLVMSetInitializer(gVar, LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0));
            LLVMBuildStore(builder, exp, gVar);
        }
    }
    return 0;
}

void* _generate_var_node(struct code_generator* cg, struct exp_node* node)
{
    struct var_node* var = (struct var_node*)node;
    if (!var->base.parent)
        return _generate_global_var_node(cg, var, false);
    else
        return _generate_local_var_node(cg, var);
}

void* _generate_local_var_node(struct code_generator* cg, struct var_node* node)
{
    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    // fprintf(stderr, "_generate_var_node:1 %lu!, %lu\n", node->var_names.size(),
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder)); // builder->GetInsertBlock()->getParent();
    // fprintf(stderr, "_generate_var_node:2 %lu!\n", node->var_names.size());
    const char* var_name = string_get(&node->var_name);
    // log_info(DEBUG, "local var cg: %s", var_name.c_str());
    struct exp_node* init = node->init_value;

    LLVMValueRef init_val;
    if (init) {
        init_val = (LLVMValueRef)generate_code(cg, init);
        if (init_val == 0)
            return 0;
    } else {
        init_val = LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0);
    }

    LLVMValueRef alloca = _create_entry_block_alloca(cg, fun, var_name);
    LLVMBuildStore(builder, init_val, alloca);
    hashtable_set(&cg->named_values, var_name, alloca);
    return 0;
    // KSDbgInfo.emitLocation(this);
}

void* _generate_for_node(struct code_generator* cg, struct exp_node* node)
{
    struct for_node* forn = (struct for_node*)node;
    const char* var_name = string_get(&forn->var_name);
    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    LLVMBasicBlockRef bb = LLVMGetInsertBlock(builder);
    LLVMValueRef fun = LLVMGetBasicBlockParent(bb);

    LLVMValueRef alloca = _create_entry_block_alloca(cg, (LLVMValueRef)fun, var_name);

    // KSDbgInfo.emitLocation(this);
    LLVMValueRef start_v = (LLVMValueRef)generate_code(cg, forn->start);
    if (start_v == 0)
        return 0;

    LLVMBuildStore(builder, start_v, alloca);
    LLVMBasicBlockRef loop_bb = LLVMAppendBasicBlockInContext(context, fun, "loop");
    LLVMBuildBr(builder, loop_bb);
    LLVMPositionBuilderAtEnd(builder, loop_bb);

    LLVMValueRef old_alloca = (LLVMValueRef)hashtable_get(&cg->named_values, var_name);
    hashtable_set(&cg->named_values, var_name, alloca);

    if (generate_code(cg, forn->body) == 0)
        return 0;

    LLVMValueRef step_v;
    if (forn->step) {
        step_v = (LLVMValueRef)generate_code(cg, forn->step);
        if (step_v == 0)
            return 0;
    } else {
        step_v = LLVMConstReal(LLVMDoubleTypeInContext(context), 1.0);
    }

    LLVMValueRef end_cond = (LLVMValueRef)generate_code(cg, forn->end);
    if (end_cond == 0)
        return end_cond;

    LLVMValueRef cur_var = LLVMBuildLoad(builder, alloca, var_name);
    LLVMValueRef next_var = LLVMBuildFAdd(builder, cur_var, step_v, "nextvar");
    LLVMBuildStore(builder, next_var, alloca);
    end_cond = LLVMBuildFCmp(builder, LLVMRealONE, end_cond, LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0), "loopcond");

    LLVMBasicBlockRef after_bb = LLVMAppendBasicBlockInContext(context, fun, "afterloop");

    LLVMBuildCondBr(builder, end_cond, loop_bb, after_bb);
    LLVMPositionBuilderAtEnd(builder, after_bb);

    if (old_alloca)
        hashtable_set(&cg->named_values, var_name, old_alloca);
    else
        hashtable_remove(&cg->named_values, var_name);

    return LLVMConstNull(LLVMDoubleTypeInContext(context));
}

void* _generate_block_node(struct code_generator* cg, struct exp_node* node)
{
    struct block_node* block = (struct block_node*)node;
    void* codegen;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node* exp = *(struct exp_node**)array_get(&block->nodes, i);
        codegen = generate_code(cg, exp);
    }
    return codegen;
}

void create_module_and_pass_manager(struct code_generator* cg,
    const char* module_name)
{
    LLVMModuleRef moduleRef = LLVMModuleCreateWithNameInContext(module_name, (LLVMContextRef)cg->context);
    LLVMTargetMachineRef target_marchine = (LLVMTargetMachineRef)create_target_machine(moduleRef);
    LLVMTargetDataRef data_layout = LLVMCreateTargetDataLayout(target_marchine);
    LLVMSetModuleDataLayout(moduleRef, data_layout);
    cg->module = moduleRef;
    // // Create a new pass manager attached to it.
    // llvm::legacy::FunctionPassManager* fpm = new llvm::legacy::FunctionPassManager(module);
    // cg->fpm = fpm;

    // // Promote allocas to registers.
    // fpm->add(createPromoteMemoryToRegisterPass());
    // // Do simple "peephole" optimizations and bit-twiddling optzns.
    // fpm->add(createInstructionCombiningPass());
    // // Reassociate expressions.
    // fpm->add(createReassociatePass());
    // // Eliminate Common SubExpressions.
    // fpm->add(createNewGVNPass());
    // // Simplify the control flow graph (deleting unreachable blocks, etc).
    // fpm->add(createCFGSimplificationPass());

    // fpm->doInitialization();
}

void generate_runtime_module(struct code_generator* cg, struct array* builtins)
{
    for (size_t i = 0; i < array_size(builtins); i++) {
        struct exp_node* node = *(struct exp_node**)array_get(builtins, i);
        generate_code(cg, node);
    }
}

void* _generate_unk_node(struct code_generator* cg, struct exp_node* node)
{
    if (!cg || !node)
        return 0;

    return 0;
}

void* (*cg_fp[])(struct code_generator*, struct exp_node*) = {
    _generate_unk_node,
    _generate_num_node,
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

void* generate_code(struct code_generator* cg, struct exp_node* node)
{
    return cg_fp[node->node_type](cg, node);
}

void* create_target_machine(void* pmodule)
{
    LLVMModuleRef module = (LLVMModuleRef)pmodule;
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
