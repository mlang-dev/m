/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * LLVM IR Code Generation Functions
 */
#include "llvm-c/Core.h"
#include "llvm-c/Target.h"
#include "llvm-c/TargetMachine.h"

#include "codegen.h"
#include "clib/util.h"
#include "clib/array.h"

void* _generate_global_var_node(code_generator* cg, var_node* node,
    bool is_external = false);
void* _generate_local_var_node(code_generator* cg, var_node* node);
void* _generate_prototype_node(code_generator* cg, exp_node* node);
void* _generate_block_node(code_generator* cg, exp_node* block);

code_generator* cg_new(menv* env, parser* parser)
{
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    code_generator* cg = new code_generator();
    cg->parser = parser;
    cg->context = env->context;
    cg->builder = LLVMCreateBuilderInContext((LLVMContextRef)env->context);
    return cg;
}

void cg_free(code_generator* cg)
{
    LLVMDisposeBuilder((LLVMBuilderRef)cg->builder);
    //delete (llvm::legacy::FunctionPassManager*)cg->fpm;
    if(cg->module)
        LLVMDisposeModule((LLVMModuleRef)cg->module);
    delete cg;
}

LLVMValueRef _get_function(code_generator* cg, const char* name)
{
    // First, see if the function has already been added to the current module.
    if (LLVMValueRef f = LLVMGetNamedFunction((LLVMModuleRef)cg->module, name))
        return f;

    // If not, check whether we can codegen the declaration from some existing
    // prototype.
    auto fp = cg->protos.find(name);
    if (fp != cg->protos.end())
        return (LLVMValueRef)_generate_prototype_node(cg, (exp_node*)fp->second);

    // If no existing prototype exists, return null.
    return nullptr;
}

LLVMValueRef _get_global_variable(code_generator* cg, std::string name)
{
    // First, see if the function has already been added to the current module.
    if (LLVMValueRef gv = LLVMGetNamedGlobal((LLVMModuleRef)cg->module, name.c_str()))
        return gv;

    // If not, it's defined in other module, we can codegen the external
    // declaration from existing type.
    auto fgv = cg->gvs.find(name);
    if (fgv != cg->gvs.end()) {
        // log_info(DEBUG, "found defition before");
        return (LLVMValueRef)_generate_global_var_node(cg, fgv->second, true);
    }

    // If no existing prototype exists, return null.
    return nullptr;
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
        LLVMValueRef alloca = _create_entry_block_alloca(cg, fun, node->args[i].c_str());

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
        cg->named_values[node->args[i]] = alloca;
    }
}

void* _generate_num_node(code_generator* cg, exp_node* node)
{
    return LLVMConstReal(LLVMDoubleTypeInContext((LLVMContextRef)cg->context), ((num_node*)node)->double_val);
}

void* _generate_ident_node(code_generator* cg, exp_node* node)
{
    auto ident = (ident_node*)node;
    std::string idname(string_get(&ident->name));
    LLVMValueRef v = (LLVMValueRef)cg->named_values[idname];
    if (!v) {
        LLVMValueRef gVar = _get_global_variable(cg, idname);
        if (gVar) {
            return LLVMBuildLoad((LLVMBuilderRef)cg->builder, gVar, string_get(&ident->name));
        } else {
            log_info(ERROR, "Unknown variable name: %s", string_get(&ident->name));
            return 0;
        }
    }
    return LLVMBuildLoad((LLVMBuilderRef)cg->builder, v, string_get(&ident->name));
}

void* _generate_binary_node(code_generator* cg, exp_node* node)
{
    auto bin = (binary_node*)node;
    LLVMValueRef lv = (LLVMValueRef)generate_code(cg, bin->lhs);
    LLVMValueRef rv = (LLVMValueRef)generate_code(cg, bin->rhs);
    if (!lv || !rv)
        return nullptr;
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
        std::string fname = std::string("binary") + std::string(string_get(&bin->op));
        LLVMValueRef fun = _get_function(cg, fname.c_str());
        assert(fun && "binary operator not found!");
        LLVMValueRef ops[2] = { (LLVMValueRef)lv, (LLVMValueRef)rv };
        return LLVMBuildCall(builder, fun, ops, 2, "binop");
    }
}

void* _generate_call_node(code_generator* cg, exp_node* node)
{
    auto call = (call_node*)node;
    LLVMValueRef callee = _get_function(cg, string_get(&call->callee));
    if (!callee)
        return log_info(ERROR, "Unknown function referenced: %s", string_get(&call->callee));
    if (LLVMCountParams(callee) != call->args.size())
        return log_info(ERROR,
            "Incorrect number of arguments passed: callee (prototype "
            "generated in llvm): %lu, calling: %lu",
            LLVMCountParams(callee), call->args.size());

    std::vector<LLVMValueRef> arg_values;
    for (unsigned long i = 0, e = call->args.size(); i != e; ++i) {
        arg_values.push_back((LLVMValueRef)generate_code(cg, call->args[i]));
        if (!arg_values.back())
            return 0;
    }
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    return LLVMBuildCall(builder, callee, arg_values.data(), arg_values.size(), "calltmp");
}

void* _generate_prototype_node(code_generator* cg, exp_node* node)
{
    auto proto = (prototype_node*)node;
    cg->protos[std::string(string_get(&proto->name))] = proto;
    LLVMContextRef context = (LLVMContextRef)cg->context;
    array doubles;
    array_init(&doubles, sizeof(LLVMTypeRef));
    for (auto& arg : proto->args){
        LLVMTypeRef doubleType = LLVMDoubleTypeInContext(context);
        array_push(&doubles, &doubleType);
    }
    LLVMTypeRef ft =  LLVMFunctionType(LLVMDoubleTypeInContext(context), (LLVMTypeRef*)array_data(&doubles), array_size(&doubles), false);
    LLVMValueRef fun = LLVMAddFunction((LLVMModuleRef)cg->module, string_get(&proto->name), ft);
    unsigned i = 0;
    for (unsigned i = 0; i< LLVMCountParams(fun); i++){
        LLVMValueRef param = LLVMGetParam(fun, i);
        LLVMSetValueName2(param, proto->args[i].c_str(), proto->args[i].size());
    }
    array_deinit(&doubles);
    return fun;
}

void* _generate_function_node(code_generator* cg, exp_node* node)
{
    auto funn = (function_node*)node;
    cg->named_values.clear();
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
    for (auto stmt : funn->body->nodes) {
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
    auto unary = (unary_node*)node;
    LLVMValueRef operand_v = (LLVMValueRef)generate_code(cg, unary->operand);
    if (operand_v == 0)
        return 0;

    std::string fname = std::string("unary") + std::string(string_get(&unary->op));
    LLVMValueRef fun = _get_function(cg, fname.c_str());
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
            std::string varname(var_name);
            cg->gvs[varname] = node;
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
    auto var = (var_node*)node;
    if (!var->base.parent)
        return _generate_global_var_node(cg, var);
    else
        return _generate_local_var_node(cg, var);
}

void* _generate_local_var_node(code_generator* cg, var_node* node)
{
    std::vector<LLVMValueRef> old_bindings;

    LLVMContextRef context = (LLVMContextRef)cg->context;
    LLVMBuilderRef builder = (LLVMBuilderRef)cg->builder;
    // fprintf(stderr, "_generate_var_node:1 %lu!, %lu\n", node->var_names.size(),
    LLVMValueRef fun = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));// builder->GetInsertBlock()->getParent();
    // fprintf(stderr, "_generate_var_node:2 %lu!\n", node->var_names.size());

    // Register all variables and emit their initializer.
    const std::string var_name(string_get(&node->var_name));
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

    LLVMValueRef alloca = _create_entry_block_alloca(cg, fun, var_name.c_str());
    LLVMBuildStore(builder, init_val, alloca);
    // Remember the old variable binding so that we can restore the binding when
    // we unrecurse.
    old_bindings.push_back((LLVMValueRef)cg->named_values[var_name]);

    // Remember this binding.
    cg->named_values[var_name] = alloca;
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
    auto forn = (for_node*)node;
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
    std::string varname(var_name);
    LLVMValueRef old_alloca = (LLVMValueRef)cg->named_values[varname];
    cg->named_values[varname] = alloca;

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
        cg->named_values[varname] = old_alloca;
    else
        cg->named_values.erase(varname);

    // for expr always returns 0.0.
    return LLVMConstNull(LLVMDoubleTypeInContext(context));
}

void* _generate_block_node(code_generator* cg, exp_node* node)
{
    auto block = (block_node*)node;
    void* codegen;
    for (auto exp : block->nodes) {
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
    for (auto node: parser->ast->builtins) {
        generate_code(cg, node);
    }
}

void* _generate_unk_node(code_generator* cg, exp_node* node)
{
    return nullptr;
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
        return nullptr;
    }
    auto cpu = "generic";
    auto features = "";
    LLVMCodeGenOptLevel opt = LLVMCodeGenOptLevel::LLVMCodeGenLevelDefault;
    LLVMRelocMode rm = LLVMRelocMode::LLVMRelocDefault;
    LLVMCodeModel cm = LLVMCodeModel::LLVMCodeModelDefault;
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, target_triple, cpu, features, opt, rm, cm);
    LLVMSetModuleDataLayout(module, LLVMCreateTargetDataLayout(target_machine));
    return target_machine;
}
