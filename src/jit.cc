#include "jit.h"
#include "util.h"


uint64_t MM::getSymbolAddress(const std::string &name) {
    uint64_t fun_addr = SectionMemoryManager::getSymbolAddress(name);
    if (fun_addr)
        return fun_addr;
    
    uint64_t jit_addr = (uint64_t)_jit->GetSymbolAddress(name);
    if (!jit_addr)
        llvm::report_fatal_error("Program used extern function '" + name +
                           "' which could not be resolved!");
    
    return jit_addr;
}

JIT::JIT(CodeGenerator* cg){
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    _cg = cg;
}

JIT::~JIT() {
    EngineVector::iterator begin = _engines.begin();
    EngineVector::iterator end = _engines.end();
    EngineVector::iterator it;
    for (it = begin; it != end; ++it)
        delete *it;
}

void *JIT::GetPointerToFunction(llvm::Function *fun) {
    // See if an existing instance of MCJIT has this function.
    EngineVector::iterator begin = _engines.begin();
    EngineVector::iterator end = _engines.end();
    EngineVector::iterator it;
    
    //fprintf(stderr, "getting pointer to function %d...\n", (intptr_t)(void*)fun);
    for (it = begin; it != end; ++it) {
        //fprintf(stderr, "iterating 1..");
        void *p_fun = (*it)->getPointerToFunction(fun);
        //fprintf(stderr, "iterating 2:%d..", p_fun);
        if (p_fun){
            fprintf(stderr, "found the function. returning it");
            return p_fun;
        }
    }
    
    // If we didn't find the function, see if we can generate it.
    //fprintf(stderr, "didn't find the function, regenerating :%d... ", _code_generator->_module?1:0);
    if (_cg->module) {
        std::string ErrStr;
        llvm::ExecutionEngine *execution_engine =
        llvm::EngineBuilder(std::unique_ptr<llvm::Module>((llvm::Module*)_cg->module))
        .setErrorStr(&ErrStr)
        .setMCJITMemoryManager(std::unique_ptr<MM>(new MM(this)))
        .create();
        if (!execution_engine) {
            fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
            exit(1);
        }
        
        llvm::Module* module = (llvm::Module*)_cg->module;
        // Create a function pass manager for this engine
        auto *function_pass_manager = new llvm::legacy::FunctionPassManager(module);
        
        // Set up the optimizer pipeline.  Start with registering info about how the
        // target lays out data structures.
        module->setDataLayout(execution_engine->getDataLayout());
        // Provide basic AliasAnalysis support for GVN.
        function_pass_manager->add(llvm::createCostModelAnalysisPass());
        // Promote allocas to registers.
        function_pass_manager->add(llvm::createPromoteMemoryToRegisterPass());
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        function_pass_manager->add(llvm::createInstructionCombiningPass());
        // Reassociate expressions.
        function_pass_manager->add(llvm::createReassociatePass());
        // Eliminate Common SubExpressions.
        function_pass_manager->add(llvm::createNewGVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        function_pass_manager->add(llvm::createCFGSimplificationPass());
        function_pass_manager->doInitialization();
        
        // For each function in the module
        llvm::Module::iterator it;
        llvm::Module::iterator end = module->end();
        for (it = module->begin(); it != end; ++it) {
            // Run the FPM on this function
            function_pass_manager->run(*it);
        }
        
        // We don't need this anymore
        delete function_pass_manager;
        
        _cg->module = NULL;
        _engines.push_back(execution_engine);
        execution_engine->finalizeObject();
        return execution_engine->getPointerToFunction(fun);
    }
    return 0;
}

void *JIT::GetSymbolAddress(const std::string &name) {
    // Look for the symbol in each of our execution engines.
    EngineVector::iterator begin = _engines.begin();
    EngineVector::iterator end = _engines.end();
    EngineVector::iterator it;
    for (it = begin; it != end; ++it) {
        uint64_t fun_addr = (*it)->getFunctionAddress(name);
        if (fun_addr) {
            return (void *)fun_addr;
        }
    }
    return 0;
}

