#include "codegen.h"
#include "util.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"

llvm::Value* ErrorValue(const char* str){
    fprintf(stderr, "Error: %s\n", str);
    return 0;
}

llvm::Function* ErrorFun(const char * str) {
    fprintf(stderr, "Error: %s\n", str);
    return 0;
}

LLVMCodeGenerator::LLVMCodeGenerator(Parser* parser):_parser(parser){
    _builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
}

LLVMCodeGenerator::~LLVMCodeGenerator(){
    delete _builder;
}

llvm::Module* LLVMCodeGenerator::GetModuleForNewFunction() {
    // If we have a Module that hasn't been JITed, use that.
    if (_module)
        return _module;
    
    // Otherwise create a new Module.
    std::string module_name = MakeUniqueName("mjit_module_");
    llvm::Module *module = new llvm::Module(module_name, llvm::getGlobalContext());
    _modules.push_back(module);
    _module = module;
    return module;
}



/*virtual*/void* LLVMCodeGenerator::generate(NumNode* node)
{
    return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(node->_val));
}

/*virtual*/void* LLVMCodeGenerator::generate(IdentNode* node)
{
    llvm::Value *v = _named_values[node->_name];
    if (!v)
        return ErrorValue("Unknown variable name");
    return _builder->CreateLoad(v, node->_name.c_str());
}

/*virtual*/void* LLVMCodeGenerator::generate(BinaryNode* node)
{
    llvm::Value* lv = (llvm::Value*)node->_lhs->codegen(this);
    llvm::Value* rv = (llvm::Value*)node->_rhs->codegen(this);
    if(!lv || !rv)
        return nullptr;
    switch (node->_op) {
        case '+':
            return _builder->CreateFAdd(lv, rv, "addtmp");
        case '-':
            return _builder->CreateFSub(lv, rv, "subtmp");
        case '*':
            return _builder->CreateFMul(lv, rv, "multmp");
        case '/':
            return _builder->CreateFDiv(lv, rv, "divtmp");
        case '<':
            lv = _builder->CreateFCmpULT(lv, rv, "cmptmp");
            return _builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(llvm::getGlobalContext()), "booltmp");
        default:
            return ErrorValue("unrecognized binary operator");
    }
    
}

/*virtual*/void* LLVMCodeGenerator::generate(CallExpNode* node)
{
    llvm::Function *callee = _module->getFunction(node->_callee);
    if(!callee)
        return ErrorValue("Unknown function referenced");
    if(callee->arg_size() != node->_args.size())
        return ErrorValue("Incorrect number of arguments passed");
    std::vector<llvm::Value*> arg_values;
    for(unsigned long i = 0, e=node->_args.size(); i!=e; ++i){
        arg_values.push_back((llvm::Value*)node->_args[i]->codegen(this));
        if(!arg_values.back())
            return 0;
    }
    return _builder->CreateCall(callee, arg_values, "calltmp");
}

/*virtual*/void* LLVMCodeGenerator::generate(PrototypeNode* node)
{
    std::vector<llvm::Type*> doubles(node->_args.size(), llvm::Type::getDoubleTy(llvm::getGlobalContext()));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), doubles, false);
    llvm::Function* fun = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MakeFunctionName(node->_name),
                                                 GetModuleForNewFunction());
    unsigned i = 0;
    for(auto &arg : fun->args())
        arg.setName(node->_args[i++]);
    return fun;
}


/// CreateArgumentAllocas - Create an alloca for each argument and register the
/// argument in the symbol table so that references to it will succeed.
void LLVMCodeGenerator::_CreateArgumentAllocas(PrototypeNode* node, llvm::Function *fun) {
    llvm::Function::arg_iterator arg_it = fun->arg_begin();
    for (size_t i = 0, e = node->_args.size(); i != e; ++i, ++arg_it) {
        // Create an alloca for this variable.
        llvm::AllocaInst *alloca = _CreateEntryBlockAlloca(fun, node->_args[i]);
        
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
        _builder->CreateStore(arg_it, alloca);
        
        // Add arguments to variable symbol table.
        _named_values[node->_args[i]] = alloca;
    }
}

/*virtual*/void* LLVMCodeGenerator::generate(FunctionNode* node)
{
    _named_values.clear();
    auto fun = (llvm::Function*)node->_prototype->codegen(this);
    if(!fun)
        return 0;
    
    if(node->_prototype->isBinaryOp())
        _parser->_op_precedence[node->_prototype->GetOpName()] = node->_prototype->_precedence;
    
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", fun);
    _builder->SetInsertPoint(bb);

    _CreateArgumentAllocas(node->_prototype, fun);
    if(llvm::Value* ret_val = (llvm::Value*)node->_body->codegen(this)){
        _builder->CreateRet(ret_val);
        llvm::verifyFunction(*fun);
        return fun;
    }
    fun->eraseFromParent();
    if(node->_prototype->isBinaryOp())
        _parser->_op_precedence.erase(node->_prototype->GetOpName());
    return 0;
}

void* LLVMCodeGenerator::generate(UnaryNode* node){
    llvm::Value *operand_v = (llvm::Value*)node->_operand->codegen(this);
    if (operand_v == 0)
        return 0;
    
    llvm::Function *fun = _module->getFunction(std::string("unary") + node->_op);
    if (fun == 0)
        return ErrorValue("Unknown unary operator");
    
    //KSDbgInfo.emitLocation(this);
    return _builder->CreateCall(fun, operand_v, "unop");
    
}

void *LLVMCodeGenerator::generate(ConditionNode* node) {
    //KSDbgInfo.emitLocation(this);
    
    llvm::Value *cond_v = (llvm::Value*)node->_condition->codegen(this);
    if (cond_v == 0)
        return 0;
    
    // Convert condition to a bool by comparing equal to 0.0.
    cond_v = _builder->CreateFCmpONE(
                                   cond_v, llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(0.0)), "ifcond");
    
    llvm::Function *fun = _builder->GetInsertBlock()->getParent();
    
    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    llvm::BasicBlock *then_bb =
    llvm::BasicBlock::Create(llvm::getGlobalContext(), "then", fun);
    llvm::BasicBlock *else_bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "else");
    llvm::BasicBlock *merge_bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "ifcont");
    
    _builder->CreateCondBr(cond_v, then_bb, else_bb);
    
    // Emit then value.
    _builder->SetInsertPoint(then_bb);
    
    llvm::Value *then_v = (llvm::Value*)node->_then->codegen(this);
    if (then_v == 0)
        return 0;
    
    _builder->CreateBr(merge_bb);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    then_bb = _builder->GetInsertBlock();
    
    // Emit else block.
    fun->getBasicBlockList().push_back(else_bb);
    _builder->SetInsertPoint(else_bb);
    
    llvm::Value *else_v = (llvm::Value*)node->_else->codegen(this);
    if (else_v == 0)
        return 0;
    
    _builder->CreateBr(merge_bb);
    // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
    else_bb = _builder->GetInsertBlock();
    
    // Emit merge block.
    fun->getBasicBlockList().push_back(merge_bb);
    _builder->SetInsertPoint(merge_bb);
    llvm::PHINode *phi_node =
    _builder->CreatePHI(llvm::Type::getDoubleTy(llvm::getGlobalContext()), 2, "iftmp");
    
    phi_node->addIncoming(then_v, then_bb);
    phi_node->addIncoming(else_v, else_bb);
    return phi_node;
}


/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
llvm::AllocaInst* LLVMCodeGenerator::_CreateEntryBlockAlloca(llvm::Function *fun,
                                          const std::string &var_name) {
    llvm::IRBuilder<> builder(&fun->getEntryBlock(),
                     fun->getEntryBlock().begin());
    return builder.CreateAlloca(llvm::Type::getDoubleTy(llvm::getGlobalContext()), 0,
                             var_name.c_str());
}


void *LLVMCodeGenerator::generate(VarNode* node) {
    std::vector<llvm::AllocaInst *> old_bindings;
    
    llvm::Function *fun = _builder->GetInsertBlock()->getParent();
    
    // Register all variables and emit their initializer.
    for (size_t i = 0, e = node->_var_names.size(); i != e; ++i) {
        const std::string &var_name = node->_var_names[i].first;
        ExpNode *init = node->_var_names[i].second;
        
        // Emit the initializer before adding the variable to scope, this prevents
        // the initializer from referencing the variable itself, and permits stuff
        // like this:
        //  var a = 1 in
        //    var a = a in ...   # refers to outer 'a'.
        llvm::Value *init_val;
        if (init) {
            init_val = (llvm::Value*)init->codegen(this);
            if (init_val == 0)
                return 0;
        } else { // If not specified, use 0.0.
            init_val = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(0.0));
        }
        
        llvm::AllocaInst *alloca = _CreateEntryBlockAlloca(fun, var_name);
        _builder->CreateStore(init_val, alloca);
        
        // Remember the old variable binding so that we can restore the binding when
        // we unrecurse.
        old_bindings.push_back(_named_values[var_name]);
        
        // Remember this binding.
        _named_values[var_name] = alloca;
    }
    
    //KSDbgInfo.emitLocation(this);
    
    // Codegen the body, now that all vars are in scope.
    llvm::Value *body_val = (llvm::Value*)node->_body->codegen(this);
    if (body_val == 0)
        return 0;
    
    // Pop all our variables from scope.
    for (size_t i = 0, e = node->_var_names.size(); i != e; ++i)
        _named_values[node->_var_names[i].first] = old_bindings[i];
    
    // Return the body computation.
    return body_val;
}

void* LLVMCodeGenerator::generate(ForNode* node) {
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
    
    llvm::Function *fun = _builder->GetInsertBlock()->getParent();
    
    // Create an alloca for the variable in the entry block.
    llvm::AllocaInst *alloca = _CreateEntryBlockAlloca(fun, node->_var_name);
    
    //KSDbgInfo.emitLocation(this);
    
    // Emit the start code first, without 'variable' in scope.
    llvm::Value *start_v = (llvm::Value*)node->_start->codegen(this);
    if (start_v == 0)
        return 0;
    
    // Store the value into the alloca.
    _builder->CreateStore(start_v, alloca);
    
    // Make the new basic block for the loop header, inserting after current
    // block.
    llvm::BasicBlock *loop_bb =
    llvm::BasicBlock::Create(llvm::getGlobalContext(), "loop", fun);
    
    // Insert an explicit fall through from the current block to the LoopBB.
    _builder->CreateBr(loop_bb);
    
    // Start insertion in LoopBB.
    _builder->SetInsertPoint(loop_bb);
    
    // Within the loop, the variable is defined equal to the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    llvm::AllocaInst *old_alloca = (llvm::AllocaInst*)_named_values[node->_var_name];
    _named_values[node->_var_name] = alloca;
    
    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    if (node->_body->codegen(this) == 0)
        return 0;
    
    // Emit the step value.
    llvm::Value *step_v;
    if (node->_step) {
        step_v = (llvm::Value*)node->_step->codegen(this);
        if (step_v == 0)
            return 0;
    } else {
        // If not specified, use 1.0.
        step_v = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(1.0));
    }
    
    // Compute the end condition.
    llvm::Value *end_cond = (llvm::Value*)node->_end->codegen(this);
    if (end_cond == 0)
        return end_cond;
    
    // Reload, increment, and restore the alloca.  This handles the case where
    // the body of the loop mutates the variable.
    llvm::Value *cur_var = _builder->CreateLoad(alloca, node->_var_name.c_str());
    llvm::Value *next_var = _builder->CreateFAdd(cur_var, step_v, "nextvar");
    _builder->CreateStore(next_var, alloca);
    
    // Convert condition to a bool by comparing equal to 0.0.
    end_cond = _builder->CreateFCmpONE(
                                      end_cond, llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(0.0)), "loopcond");
    
    // Create the "after loop" block and insert it.
    llvm::BasicBlock *after_bb =
    llvm::BasicBlock::Create(llvm::getGlobalContext(), "afterloop", fun);
    
    // Insert the conditional branch into the end of LoopEndBB.
    _builder->CreateCondBr(end_cond, loop_bb, after_bb);
    
    // Any new code will be inserted in AfterBB.
    _builder->SetInsertPoint(after_bb);
    
    // Restore the unshadowed variable.
    if (old_alloca)
        _named_values[node->_var_name] = old_alloca;
    else
        _named_values.erase(node->_var_name);
    
    // for expr always returns 0.0.
    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(llvm::getGlobalContext()));
}

llvm::Function *LLVMCodeGenerator::GetFunction(const std::string fun_name) {
    ModuleVector::iterator begin = _modules.begin();
    ModuleVector::iterator end = _modules.end();
    ModuleVector::iterator it;
    for (it = begin; it != end; ++it) {
        llvm::Function *F = (*it)->getFunction(fun_name);
        if (F) {
            if (*it == _module)
                return F;
            
            assert(_module != NULL);
            
            // This function is in a module that has already been JITed.
            // We need to generate a new prototype for external linkage.
            llvm::Function *fun = _module->getFunction(fun_name);
            if (fun && !fun->empty()) {
                ErrorFun("redefinition of function across modules");
                return 0;
            }
            
            // If we don't have a prototype yet, create one.
            if (!fun)
                fun = llvm::Function::Create(F->getFunctionType(), llvm::Function::ExternalLinkage,
                                             fun_name, _module);
            return fun;
        }
    }
    return NULL;
}


void LLVMCodeGenerator::Dump() {
    ModuleVector::iterator begin = _modules.begin();
    ModuleVector::iterator end = _modules.end();
    ModuleVector::iterator it;
    for (it = begin; it != end; ++it){
        (*it)->dump();
    }
}

