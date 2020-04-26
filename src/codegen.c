/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation Functions
 */
#include <stdlib.h>
#include <assert.h>

#include "llvm-c/Core.h"
#include "llvm-c/Target.h"
#include "llvm-c/TargetMachine.h"

#include "codegen.h"
#include "clib/util.h"
#include "clib/array.h"
#include "clib/object.h"

void* _generate_global_var_node(code_generator* cg, var_node* node,
    bool is_external);
void* _generate_local_var_node(code_generator* cg, var_node* node);
void* _generate_prototype_node(code_generator* cg, exp_node* node);
void* _generate_block_node(code_generator* cg, exp_node* block);

code_generator* cg_new(menv* env, parser* parser)
{
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    code_generator* cg = (code_generator*)malloc(sizeof(code_generator));
    cg->parser = parser;
    cg->context = env->context;
    cg->builder = LLVMCreateBuilderInContext((LLVMContextRef)env->context);
    hashtable_init(&cg->gvs);
    hashtable_init(&cg->protos);
    hashtable_init(&cg->named_values);
    return cg;
}

void cg_free(code_generator* cg)
{
    LLVMDisposeBuilder((LLVMBuilderRef)cg->builder);
    //delete (llvm::legacy::FunctionPassManager*)cg->fpm;
    if(cg->module)
        LLVMDisposeModule((LLVMModuleRef)cg->module);
    hashtable_deinit(&cg->gvs);
    hashtable_deinit(&cg->protos);
    hashtable_deinit(&cg->named_values);
    free(cg);
    //LLVMShutdown();
}

LLVMValueRef _get_function(code_generator* cg, const char* name)
{
    // First, see if the function has already been added to the current module.
    LLVMValueRef f = LLVMGetNamedFunction((LLVMModuleRef)cg->module, name);
    if (f)
        return f;

    // If not, check whether we can codegen the declaration from some existing
    // prototype.
    exp_node* fp = (exp_node*)hashtable_get(&cg->protos, name);
    if (fp)
        return (LLVMValueRef)_generate_prototype_node(cg, fp);

    // If no existing prototype exists, return null.
    return NULL;
}

LLVMValueRef _get_global_variable(code_generator* cg, const char *name)
{
    // First, see if the function has already been added to the current module.
    LLVMValueRef gv = LLVMGetNamedGlobal((LLVMModuleRef)cg->module, name);
    if (gv)
        return gv;

    // If not, it's defined in other module, we can codegen the external
    // declaration from existing type.
    var_node* fgv = (var_node*)hashtable_get(&cg->gvs, name);//.find(name);
    if (fgv) {
        // log_info(DEBUG, "found defition before");
        return (LLVMValueRef)_generate_global_var_node(cg, fgv, true);
    }

    // If no existing prototype exists, return null.
    return NULL;
}

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
LLVMValueRef _create_entry_block_alloca(code_generator* cg,
    LLVMValueRef fun,
    const char *var_name)
{
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMBasicBlockRef bb = LLVMGetEntryBasicBlock(fun);
    LLVMPositionBuilder(builder, bb, LLVMGetFirstInstruction(bb));
    LLVMValueRef alloca = LLVMBuildAlloca(builder, LLVMDoubleTypeInContext((LLVMContextRef)cg->context), var_name);
    LLVMDisposeBuilder(builder);
    return alloca;
}

/// _create_argument_allocas - Create an alloca for each argument and register
/// the argument in the symbol table so that references to it will succeed.
void _create_argument_allocas(code_generator* cg, prototype_node* node,
    LLVMValueRef fun)
{
    for (unsigned i = 0; i< LLVMCountParams(fun); i++) {
        // Create an alloca for this variable.
        LLVMValueRef alloca = _create_entry_block_alloca(cg, fun, string_get((string*)array_get(&node->args, i)));

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

        // Store the initial value into the alloca.
        //builder->CreateStore(arg_it, alloca);
        LLVMBuildStore((LLVMBuilderRef)cg->builder, LLVMGetParam(fun, i), alloca);

        // Add arguments to variable symbol table.
        hashtable_set(&cg->named_values, string_get((string*)array_get(&node->args, i)), alloca);
    }
}

void* _generate_num_node(code_generator* cg, exp_node* node)
{
    return LLVMConstReal(LLVMDoubleTypeInContext((LLVMContextRef)cg->context), ((num_node*)node)->double_val);
}

void* _generate_ident_node(code_generator* cg, exp_node* node)
{
    ident_node* ident = (ident_node*)node;
    const char *idname = string_get(&ident->name);
    LLVMValueRef v = (LLVMValueRef)hashtable_get(&cg->named_values, idname);
    if (!v) {
        LLVMValueRef gVar = _get_global_variable(cg, idname);
        if (gVar) {
            return LLVMBuildLoad((LLVMBuilderRef)cg->builder, gVar, idname);
        } else {
            log_info(ERROR, "Unknown variable name: %s", idname);
            return 0;
        }
    }
    return LLVMBuildLoad((LLVMBuilderRef)cg->builder, v, idname);
}

void* _generate_binary_node(code_generator* cg, exp_node* node)
{
    binary_node* bin = (binary_node*)node;
    LLVMValueRef lv = (LLVMValueRef)generate_code(cg, bin->lhs);
    LLVMValueRef rv = (LLVMValueRef)generate_code(cg, bin->rhs);
    if (!lv || !rv)
        return NULL;
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    LLVMContextRef context = (LLVMContextRef)cg->context;
    if (string_eq_chars(&bin->op, "+"))
        return LLVMBuildFAdd(builder, lv, rv, "addtmp");
    else if (string_eq_chars(&bin->op,"-"))
        return LLVMBuildFSub(builder, lv, rv, "subtmp");
    else if (string_eq_chars(&bin->op, "*"))
        return LLVMBuildFMul(builder, lv, rv, "multmp");
    else if (string_eq_chars(&bin->op, "/"))
        return LLVMBuildFDiv(builder, lv, rv, "divtmp");
    else if (string_eq_chars(&bin->op, "<")) {
        lv = LLVMBuildFCmp(builder, LLVMRealULT, lv, rv, "cmptmp");
        return LLVMBuildUIToFP(builder, lv, LLVMDoubleTypeInContext(context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, ">")) {
        lv = LLVMBuildFCmp(builder, LLVMRealUGT, lv, rv, "cmptmp");
        return LLVMBuildUIToFP(builder, lv, LLVMDoubleTypeInContext(context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, "==")) {
        lv = LLVMBuildFCmp(builder, LLVMRealUEQ, lv, rv, "cmptmp");
        return LLVMBuildUIToFP(builder, lv, LLVMDoubleTypeInContext(context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, "!=")) {
        lv = LLVMBuildFCmp(builder, LLVMRealUNE, lv, rv, "cmptmp");
        return LLVMBuildUIToFP(builder, lv, LLVMDoubleTypeInContext(context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, "<=")) {
        lv = LLVMBuildFCmp(builder, LLVMRealULE, lv, rv, "cmptmp");
        return LLVMBuildUIToFP(builder, lv, LLVMDoubleTypeInContext(context),
            "booltmp");
    } else if (string_eq_chars(&bin->op, ">=")) {
        lv = LLVMBuildFCmp(builder, LLVMRealUGE, lv, rv, "cmptmp");
        return LLVMBuildUIToFP(builder, lv, LLVMDoubleTypeInContext(context),
            "booltmp");
    } else {
        string f_name;
        string_init_chars(&f_name, "binary");
        string_add(&f_name, &bin->op);
        LLVMValueRef fun = _get_function(cg, string_get(&f_name));
        assert(fun && "binary operator not found!");
        LLVMValueRef ops[2] = { (LLVMValueRef)lv, (LLVMValueRef)rv };
        return LLVMBuildCall(builder, fun, ops, 2, "binop");
    }
}

void* _generate_call_node(code_generator* cg, exp_node* node)
{
    call_node* call = (call_node*)node;
    LLVMValueRef callee = _get_function(cg, string_get(&call->callee));
    if (!callee)
        return log_info(ERROR, "Unknown function referenced: %s", string_get(&call->callee));
    if (LLVMCountParams(callee) != array_size(&call->args))
        return log_info(ERROR,
            "Incorrect number of arguments passed: callee (prototype "
            "generated in llvm): %lu, calling: %lu",
            LLVMCountParams(callee), array_size(&call->args));

    LLVMValueRef arg_values[array_size(&call->args)];//variable-length array, C99 feature
    for (unsigned long i = 0, e = array_size(&call->args); i != e; ++i) {
        LLVMValueRef ret = (LLVMValueRef)generate_code(cg, *(exp_node**)array_get(&call->args, i));
        if (!ret)
            return 0;
        arg_values[i] = ret;
    }
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    return LLVMBuildCall(builder, callee, arg_values, array_size(&call->args), "calltmp");
}

void* _generate_prototype_node(code_generator* cg, exp_node* node)
{
    prototype_node *proto = (prototype_node*)node;
    //string *str = (string*)array_get(&proto->args, 0);
    //log_info(DEBUG, "generating prototype node: %s", string_get(str));
 
    hashtable_set(&cg->protos, string_get(&proto->name), proto);
    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMTypeRef doubles[array_size(&proto->args)];
    for (unsigned i = 0; i< array_size(&proto->args); i++){
        doubles[i] = LLVMDoubleTypeInContext(context);
    }
    LLVMTypeRef ft =  LLVMFunctionType(LLVMDoubleTypeInContext(context), doubles, array_size(&proto->args), false);
    LLVMValueRef fun = LLVMAddFunction((LLVMModuleRef)cg->module, string_get(&proto->name), ft);
    for (unsigned i = 0; i< LLVMCountParams(fun); i++){
        LLVMValueRef param = LLVMGetParam(fun, i);
        string* arg = (string*)array_get(&proto->args, i);
        LLVMSetValueName2(param, string_get(arg), arg->base.size);
    }
    return fun;
}

void* _generate_function_node(code_generator* cg, exp_node* node)
{
    function_node* funn = (function_node*)node;
    //cg->named_values.clear();
    hashtable_clear(&cg->named_values);
    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMValueRef fun = (LLVMValueRef)_generate_prototype_node(cg, (exp_node*)funn->prototype);
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
    _create_argument_allocas(cg, funn->prototype, fun);
    LLVMValueRef ret_val;
    for (size_t i = 0; i < array_size(&funn->body->nodes); i++) {
        exp_node* stmt = *(exp_node**)array_get(&funn->body->nodes, i);
        ret_val = (LLVMValueRef)generate_code(cg, stmt);
    }
    if (!ret_val) {
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

void* _generate_unary_node(code_generator* cg, exp_node* node)
{
    unary_node* unary = (unary_node*)node;
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

void* _generate_condition_node(code_generator* cg, exp_node* node)
{
    // KSDbgInfo.emitLocation(this);
    condition_node* cond = (condition_node*)node;
    LLVMValueRef cond_v = (LLVMValueRef)generate_code(cg, cond->condition_node);
    if (cond_v == 0)
        return 0;

    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    LLVMContextRef context = (LLVMContextRef)cg->context;
    // Convert condition to a bool by comparing equal to 0.0.
    cond_v = LLVMBuildFCmp(builder, LLVMRealONE, cond_v, LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0), "ifcond");

    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));// builder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlockInContext(context, fun, "then");
    LLVMBasicBlockRef else_bb = LLVMCreateBasicBlockInContext(context, "else");
    LLVMBasicBlockRef merge_bb = LLVMCreateBasicBlockInContext(context, "ifcont");

    LLVMBuildCondBr(builder, cond_v, then_bb, else_bb);
    // Emit then value.
    LLVMPositionBuilderAtEnd(builder, then_bb);

    LLVMValueRef then_v = (LLVMValueRef)generate_code(cg, cond->then_node);
    if (then_v == 0)
        return 0;

    LLVMBuildBr(builder, merge_bb);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
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

void* _generate_global_var_node(code_generator* cg, var_node* node,
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

void* _generate_var_node(code_generator* cg, exp_node* node)
{
    var_node* var = (var_node*)node;
    if (!var->base.parent)
        return _generate_global_var_node(cg, var, false);
    else
        return _generate_local_var_node(cg, var);
}

void* _generate_local_var_node(code_generator* cg, var_node* node)
{
    //std::vector<LLVMValueRef> old_bindings;

    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    // fprintf(stderr, "_generate_var_node:1 %lu!, %lu\n", node->var_names.size(),
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));// builder->GetInsertBlock()->getParent();
    // fprintf(stderr, "_generate_var_node:2 %lu!\n", node->var_names.size());

    // Register all variables and emit their initializer.
    const char * var_name = string_get(&node->var_name);
    // log_info(DEBUG, "local var cg: %s", var_name.c_str());
    exp_node* init = node->init_value;

    // Emit the initializer before adding the variable to scope, this prevents
    // the initializer from referencing the variable itself, and permits stuff
    // like this:
    //  var a = 1 in
    //    var a = a in ...   # refers to outer 'a'.
    LLVMValueRef init_val;
    if (init) {
        init_val = (LLVMValueRef)generate_code(cg, init);
        if (init_val == 0)
            return 0;
    } else { // If not specified, use 0.0.
        init_val = LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0);
    }

    LLVMValueRef alloca = _create_entry_block_alloca(cg, fun, var_name);
    LLVMBuildStore(builder, init_val, alloca);
    // Remember the old variable binding so that we can restore the binding when
    // we unrecurse.
    //old_bindings.push_back((LLVMValueRef)hashtable_get_p(&cg->named_values, var_name));
    hashtable_set(&cg->named_values, var_name, alloca);
    // Remember this binding.
    //cg->named_values[var_name] = alloca;
    return 0;
    // KSDbgInfo.emitLocation(this);
}

void* _generate_for_node(code_generator* cg, exp_node* node)
{
    // Output this as:
    //   var = alloca double
    //   ...
    //   start = startexpr
    //   store start -> var
    //   goto loop
    // loop:
    //   ...
    //   bodyexpr
    //   ...
    // loopend:
    //   step = stepexpr
    //   endcond = endexpr
    //
    //   curvar = load var
    //   nextvar = curvar + step
    //   store nextvar -> var
    //   br endcond, loop, endloop
    // outloop:
    for_node* forn = (for_node*)node;
    const char* var_name = string_get(&forn->var_name);
    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    LLVMBasicBlockRef bb = LLVMGetInsertBlock(builder);
    LLVMValueRef fun = LLVMGetBasicBlockParent(bb);

    // Create an alloca for the variable in the entry block.
    LLVMValueRef alloca = _create_entry_block_alloca(cg, (LLVMValueRef)fun, var_name);

    // KSDbgInfo.emitLocation(this);
    // Emit the start code first, without 'variable' in scope.
    LLVMValueRef start_v = (LLVMValueRef)generate_code(cg, forn->start);
    if (start_v == 0)
        return 0;

    // Store the value into the alloca.
    LLVMBuildStore(builder, start_v, alloca);
    // Make the new basic block for the loop header, inserting after current
    // block.
    LLVMBasicBlockRef loop_bb = LLVMAppendBasicBlockInContext(context, fun, "loop");

    // Insert an explicit fall through from the current block to the LoopBB.
    LLVMBuildBr(builder, loop_bb);

    // Start insertion in LoopBB.
    LLVMPositionBuilderAtEnd(builder, loop_bb);

    // Within the loop, the variable is defined equal  the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    LLVMValueRef old_alloca = (LLVMValueRef)hashtable_get(&cg->named_values, var_name);
    hashtable_set(&cg->named_values, var_name, alloca);

    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    if (generate_code(cg, forn->body) == 0)
        return 0;

    // Emit the step value.
    LLVMValueRef step_v;
    if (forn->step) {
        step_v = (LLVMValueRef)generate_code(cg, forn->step);
        if (step_v == 0)
            return 0;
    } else {
        // If not specified, use 1.0.
        step_v = LLVMConstReal(LLVMDoubleTypeInContext(context), 1.0);
    }

    // Compute the end condition.
    LLVMValueRef end_cond = (LLVMValueRef)generate_code(cg, forn->end);
    if (end_cond == 0)
        return end_cond;

    // Reload, increment, and restore the alloca.  This handles the case where
    // the body of the loop mutates the variable.
    LLVMValueRef cur_var = LLVMBuildLoad(builder, alloca, var_name);
    LLVMValueRef next_var = LLVMBuildFAdd(builder, cur_var, step_v, "nextvar");
    LLVMBuildStore(builder, next_var, alloca);
    // Convert condition to a bool by comparing non-equal to 0.0.
    end_cond = LLVMBuildFCmp(builder, LLVMRealONE, end_cond, LLVMConstReal(LLVMDoubleTypeInContext(context), 0.0), "loopcond");

    // Create the "after loop" block and insert it.
    LLVMBasicBlockRef after_bb = LLVMAppendBasicBlockInContext(context, fun, "afterloop");

    // Insert the conditional branch into the end of LoopEndBB.
    LLVMBuildCondBr(builder, end_cond, loop_bb, after_bb);

    // Any new code will be inserted in AfterBB.
    LLVMPositionBuilderAtEnd(builder, after_bb);

    // Restore the unshadowed variable.
    if (old_alloca)
        //cg->named_values[varname] = old_alloca;
        hashtable_set(&cg->named_values, var_name, old_alloca);
    else
        hashtable_remove(&cg->named_values, var_name);

    // for expr always returns 0.0.
    return LLVMConstNull(LLVMDoubleTypeInContext(context));
}

void* _generate_block_node(code_generator* cg, exp_node* node)
{
    block_node* block = (block_node*)node;
    void* codegen;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        exp_node* exp = *(exp_node**)array_get(&block->nodes, i);
        codegen = generate_code(cg, exp);
    }
    return codegen;
}

void create_module_and_pass_manager(code_generator* cg,
    const char* module_name)
{
    // Open a new module.
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

void generate_runtime_module(code_generator* cg, parser* parser)
{
    for (size_t i = 0; i < array_size(&parser->ast->builtins); i++) {
        exp_node *node = *(exp_node**)array_get(&parser->ast->builtins, i);
        generate_code(cg, node);
    }
}

void* _generate_unk_node(code_generator* cg, exp_node* node)
{
    if (!cg || !node)
        return 0;

    return NULL;
}

void* (*cg_fp[])(code_generator*, exp_node*) = {
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

void* generate_code(code_generator* cg, exp_node* node)
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
        return NULL;
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
