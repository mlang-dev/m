#include "util.h"
#include "codegen.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
    
llvm::LLVMContext gContext;

code_generator* create_code_generator(parser* parser){
    code_generator* cg = (code_generator*)malloc(sizeof(code_generator));
    cg->parser = parser;
    cg->context = &gContext;
    cg->builder = new llvm::IRBuilder<>(gContext);
    return cg;
}

void destroy_code_generator(code_generator* cg){
    delete (llvm::IRBuilder<>*)cg->builder;
    free(cg);
}

llvm::Value* ErrorValue(const char* str){
    fprintf(stderr, "Error: %s\n", str);
    return 0;
}

llvm::Function* ErrorFun(const char * str) {
    fprintf(stderr, "Error: %s\n", str);
    return 0;
}

llvm::Module* _getModuleForNewFunction(code_generator* cg) {
    // If we have a Module that hasn't been JITed, use that.
    if (cg->module)
        return (llvm::Module*)cg->module;
    
    // Otherwise create a new Module.
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    std::string module_name = MakeUniqueName("mjit_module_");
    llvm::Module *module = new llvm::Module(module_name, *context);
    cg->modules.push_back(module);
    cg->module = module;
    return module;
}

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
llvm::AllocaInst* _createEntryBlockAlloca(code_generator* cg, llvm::Function *fun,
                                          const std::string &var_name) {
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    llvm::IRBuilder<> builder(&fun->getEntryBlock(),
                     fun->getEntryBlock().begin());
    return builder.CreateAlloca(llvm::Type::getDoubleTy(*context), 0,
                             var_name.c_str());
}


/// CreateArgumentAllocas - Create an alloca for each argument and register the
/// argument in the symbol table so that references to it will succeed.
void _createArgumentAllocas(code_generator* cg, prototype_node* node, llvm::Function *fun) {
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    llvm::Function::arg_iterator arg_it = fun->arg_begin();
    for (size_t i = 0, e = node->_args.size(); i != e; ++i, ++arg_it) {
        // Create an alloca for this variable.
        llvm::AllocaInst *alloca = _createEntryBlockAlloca(cg, fun, node->_args[i]);
        
        // Create a debug descriptor for the variable.
        /*DIScope *Scope = KSDbgInfo.LexicalBlocks.back();
        DIFile *Unit = DBuilder->createFile(KSDbgInfo.TheCU->getFilename(),
                                            KSDbgInfo.TheCU->getDirectory());
        DILocalVariable *D = DBuilder->createLocalVariable(
                                                           dwarf::DW_TAG_arg_variable, Scope, Args[Idx], Unit, Line,
                                                           KSDbgInfo.getDoubleTy(), Idx);
        
        DBuilder->insertDeclare(Alloca, D, DBuilder->createExpression(),
                                DebugLoc::get(Line, 0, Scope),
                                Builder.GetInsertBlock());
        */
        
        // Store the initial value into the alloca.
        builder->CreateStore(arg_it, alloca);
        
        // Add arguments to variable symbol table.
        cg->named_values[node->_args[i]] = alloca;
    }
}

void* _generateNumNode(code_generator*cg, num_node* node)
{
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    return llvm::ConstantFP::get(*context, llvm::APFloat(node->_val));
}

void* _generateIdentNode(code_generator*cg, ident_node* node)
{
    llvm::Value *v = (llvm::Value *)cg->named_values[node->_name];
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    if (!v)
        return ErrorValue("Unknown variable name");
    return builder->CreateLoad(v, node->_name.c_str());
}

void* _generateBinaryNode(code_generator*cg, binary_node* node)
{
    llvm::Value* lv = (llvm::Value*)generate(cg, node->_lhs);
    llvm::Value* rv = (llvm::Value*)generate(cg, node->_rhs);
    if(!lv || !rv)
        return nullptr;
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    switch (node->_op) {
        case '+':
            return builder->CreateFAdd(lv, rv, "addtmp");
        case '-':
            return builder->CreateFSub(lv, rv, "subtmp");
        case '*':
            return builder->CreateFMul(lv, rv, "multmp");
        case '/':
            return builder->CreateFDiv(lv, rv, "divtmp");
        case '<':{
            llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
            lv = builder->CreateFCmpULT(lv, rv, "cmptmp");
            return builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*context), "booltmp");
        }
        default:
            return ErrorValue("unrecognized binary operator");
    }
}

void* _generateCallExpNode(code_generator*cg, call_node* node)
{
    llvm::Module* module = (llvm::Module*)cg->module;
    llvm::Function *callee = module->getFunction(node->_callee);
    if(!callee)
        return ErrorValue("Unknown function referenced");
    if(callee->arg_size() != node->_args.size())
        return ErrorValue("Incorrect number of arguments passed");
    std::vector<llvm::Value*> arg_values;
    for(unsigned long i = 0, e=node->_args.size(); i!=e; ++i){
        arg_values.push_back((llvm::Value*)generate(cg, node->_args[i]));
        if(!arg_values.back())
            return 0;
    }
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    return builder->CreateCall(callee, arg_values, "calltmp");
}

void* generate_prototype_node(code_generator* cg, prototype_node* node)
{
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    std::vector<llvm::Type*> doubles(node->_args.size(), llvm::Type::getDoubleTy(*context));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), doubles, false);
    llvm::Function* fun = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MakeFunctionName(node->_name),
                                                 _getModuleForNewFunction(cg));
    unsigned i = 0;
    for(auto &arg : fun->args())
        arg.setName(node->_args[i++]);
    return fun;
}



void* generate_function_node(code_generator* cg, function_node* node)
{
    cg->named_values.clear();
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    auto fun = (llvm::Function*)generate_prototype_node(cg, node->_prototype);
    if(!fun)
        return 0;
    if(node->_prototype->isBinaryOp())
        (*cg->parser->op_precedences)[node->_prototype->GetOpName()] = node->_prototype->_precedence;
    
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context, "entry", fun);
    builder->SetInsertPoint(bb);

    _createArgumentAllocas(cg, node->_prototype, fun);
    if(llvm::Value* ret_val = (llvm::Value*)generate(cg, node->_body)){
        builder->CreateRet(ret_val);
        llvm::verifyFunction(*fun);
        return fun;
    }
    fun->eraseFromParent();
    if(node->_prototype->isBinaryOp())
        cg->parser->op_precedences->erase(node->_prototype->GetOpName());
    return 0;
}

void* _generateUnaryNode(code_generator* cg, unary_node* node){
    llvm::Value *operand_v = (llvm::Value*)generate(cg, node->_operand);
    if (operand_v == 0)
        return 0;
    
    llvm::Function *fun = ((llvm::Module*)cg->module)->getFunction(std::string("unary") + node->_op);
    if (fun == 0)
        return ErrorValue("Unknown unary operator");
    
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    //KSDbgInfo.emitLocation(this);
    return builder->CreateCall(fun, operand_v, "unop");
    
}

void* _generateConditionNode(code_generator* cg, condition_node* node) {
    //KSDbgInfo.emitLocation(this);
    
    llvm::Value *cond_v = (llvm::Value*)generate(cg, node->_condition);
    if (cond_v == 0)
        return 0;
    
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    // Convert condition to a bool by comparing equal to 0.0.
    cond_v = builder->CreateFCmpONE(
                                   cond_v, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "ifcond");
    
    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    
    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    llvm::BasicBlock *then_bb =
    llvm::BasicBlock::Create(*context, "then", fun);
    llvm::BasicBlock *else_bb = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock *merge_bb = llvm::BasicBlock::Create(*context, "ifcont");
    
    builder->CreateCondBr(cond_v, then_bb, else_bb);
    
    // Emit then value.
    builder->SetInsertPoint(then_bb);
    
    llvm::Value *then_v = (llvm::Value*)generate(cg, node->_then);
    if (then_v == 0)
        return 0;
    
    builder->CreateBr(merge_bb);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    then_bb = builder->GetInsertBlock();
    
    // Emit else block.
    fun->getBasicBlockList().push_back(else_bb);
    builder->SetInsertPoint(else_bb);
    
    llvm::Value *else_v = (llvm::Value*)generate(cg, node->_else);
    if (else_v == 0)
        return 0;
    
    builder->CreateBr(merge_bb);
    // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
    else_bb = builder->GetInsertBlock();
    
    // Emit merge block.
    fun->getBasicBlockList().push_back(merge_bb);
    builder->SetInsertPoint(merge_bb);
    llvm::PHINode *phi_node =
    builder->CreatePHI(llvm::Type::getDoubleTy(*context), 2, "iftmp");
    
    phi_node->addIncoming(then_v, then_bb);
    phi_node->addIncoming(else_v, else_bb);
    return phi_node;
}




void* _generateVarNode(code_generator* cg, var_node* node) {
    std::vector<llvm::AllocaInst *> old_bindings;
    
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
    llvm::Function *fun = builder->GetInsertBlock()->getParent();
   
    // Register all variables and emit their initializer.
    for (size_t i = 0, e = node->_var_names.size(); i != e; ++i) {
        const std::string &var_name = node->_var_names[i].first;
        exp_node *init = node->_var_names[i].second;
        
        // Emit the initializer before adding the variable to scope, this prevents
        // the initializer from referencing the variable itself, and permits stuff
        // like this:
        //  var a = 1 in
        //    var a = a in ...   # refers to outer 'a'.
        llvm::Value *init_val;
        if (init) {
            init_val = (llvm::Value*)generate(cg, init);
            if (init_val == 0)
                return 0;
        } else { // If not specified, use 0.0.
            init_val = llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
        }
        
        llvm::AllocaInst *alloca = _createEntryBlockAlloca(cg, fun, var_name);
        builder->CreateStore(init_val, alloca);
        
        // Remember the old variable binding so that we can restore the binding when
        // we unrecurse.
        old_bindings.push_back((llvm::AllocaInst*)cg->named_values[var_name]);
        
        // Remember this binding.
        cg->named_values[var_name] = alloca;
    }
    
    //KSDbgInfo.emitLocation(this);
    
    // Codegen the body, now that all vars are in scope.
    llvm::Value *body_val = (llvm::Value*)generate(cg, node->_body);
    if (body_val == 0)
        return 0;
    
    // Pop all our variables from scope.
    for (size_t i = 0, e = node->_var_names.size(); i != e; ++i)
        cg->named_values[node->_var_names[i].first] = old_bindings[i];
    
    // Return the body computation.
    return body_val;
}

void* _generateForNode(code_generator* cg, for_node* node) {
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
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
   
    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    
    // Create an alloca for the variable in the entry block.
    llvm::AllocaInst *alloca = _createEntryBlockAlloca(cg, fun, node->_var_name);
    
    //KSDbgInfo.emitLocation(this);
    
    // Emit the start code first, without 'variable' in scope.
    llvm::Value *start_v = (llvm::Value*)generate(cg, node->_start);
    if (start_v == 0)
        return 0;
    
    // Store the value into the alloca.
    builder->CreateStore(start_v, alloca);
    
    // Make the new basic block for the loop header, inserting after current
    // block.
    llvm::BasicBlock *loop_bb =
    llvm::BasicBlock::Create(*context, "loop", fun);
    
    // Insert an explicit fall through from the current block to the LoopBB.
    builder->CreateBr(loop_bb);
    
    // Start insertion in LoopBB.
    builder->SetInsertPoint(loop_bb);
    
    // Within the loop, the variable is defined equal  the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    llvm::AllocaInst *old_alloca = (llvm::AllocaInst*)cg->named_values[node->_var_name];
    cg->named_values[node->_var_name] = alloca;
    
    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    if (generate(cg, node->_body) == 0)
        return 0;
    
    // Emit the step value.
    llvm::Value *step_v;
    if (node->_step) {
        step_v = (llvm::Value*)generate(cg, node->_step);
        if (step_v == 0)
            return 0;
    } else {
        // If not specified, use 1.0.
        step_v = llvm::ConstantFP::get(*context, llvm::APFloat(1.0));
    }
    
    // Compute the end condition.
    llvm::Value *end_cond = (llvm::Value*)generate(cg, node->_end);
    if (end_cond == 0)
        return end_cond;
    
    // Reload, increment, and restore the alloca.  This handles the case where
    // the body of the loop mutates the variable.
    llvm::Value *cur_var = builder->CreateLoad(alloca, node->_var_name.c_str());
    llvm::Value *next_var = builder->CreateFAdd(cur_var, step_v, "nextvar");
    builder->CreateStore(next_var, alloca);
    
    // Convert condition to a bool by comparing equal to 0.0.
    end_cond = builder->CreateFCmpONE(
                                      end_cond, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "loopcond");
    
    // Create the "after loop" block and insert it.
    llvm::BasicBlock *after_bb =
    llvm::BasicBlock::Create(*context, "afterloop", fun);
    
    // Insert the conditional branch into the end of LoopEndBB.
    builder->CreateCondBr(end_cond, loop_bb, after_bb);
    
    // Any new code will be inserted in AfterBB.
    builder->SetInsertPoint(after_bb);
    
    // Restore the unshadowed variable.
    if (old_alloca)
        cg->named_values[node->_var_name] = old_alloca;
    else
        cg->named_values.erase(node->_var_name);
    
    // for expr always returns 0.0.
    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*context));
}


void* generate(code_generator*cg, exp_node* node){
    switch(node->type){
        case NUMBER_NODE:
            return _generateNumNode(cg, (num_node*)node);
        case IDENT_NODE:
            return _generateIdentNode(cg, (ident_node*)node);
        case BINARY_NODE:
            return _generateBinaryNode(cg, (binary_node*)node);
        case CALL_NODE:
            return _generateCallExpNode(cg, (call_node*)node);
        case PROTOTYPE_NODE:
            return generate_prototype_node(cg, (prototype_node*)node);
        case FUNCTION_NODE:
            return generate_function_node(cg, (function_node*)node);
        case CONDITION_NODE:
            return _generateConditionNode(cg, (condition_node*)node);
        case FOR_NODE:
            return _generateForNode(cg, (for_node*)node);
        case UNARY_NODE:
            return _generateUnaryNode(cg, (unary_node*)node);
        case VAR_NODE:
            return _generateVarNode(cg, (var_node*)node);
    }
}

llvm::Function * GetFunction(code_generator* cg, const std::string fun_name) {
    vector<void*>::iterator begin = cg->modules.begin();
    vector<void*>::iterator end = cg->modules.end();
    vector<void*>::iterator it;
    llvm::Module* m;
    for (it = begin; it != end; ++it) {
        m = (llvm::Module*)(*it);
        llvm::Function *F = m->getFunction(fun_name);
        if (F) {
            if (m == cg->module)
                return F;
            
            assert(cg->module != NULL);
            
            // This function is in a module that has already been JITed.
            // We need to generate a new prototype for external linkage.
            llvm::Function *fun = ((llvm::Module*)(cg->module))->getFunction(fun_name);
            if (fun && !fun->empty()) {
                ErrorFun("redefinition of function across modules");
                return 0;
            }
            
            // If we don't have a prototype yet, create one.
            if (!fun)
                fun = llvm::Function::Create(F->getFunctionType(), llvm::Function::ExternalLinkage,
                                             fun_name, (llvm::Module*)cg->module);
            return fun;
        }
    }
    return NULL;
}


// void LLVMCodeGenerator::Dump() {
//     ModuleVector::iterator begin = _modules.begin();
//     ModuleVector::iterator end = _modules.end();
//     ModuleVector::iterator it;
//     for (it = begin; it != end; ++it){
//         dump((*it));
//     }
// }

