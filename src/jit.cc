#include "jit.h"
#include "util.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Target/TargetMachine.h"

std::vector<void *> g_engines;

void *_get_symbol_address(JIT* jit, const std::string& name);

class MM : public llvm::SectionMemoryManager {
    MM(const MM &) = delete;
    void operator=(const MM &) = delete;
    
public:
    MM(JIT *jit) : _jit(jit) {}
    
    /// This method returns the address of the specified symbol.
    /// Our implementation will attempt to find symbols in other
    /// modules associated with the MCJITHelper to cross link symbols
    /// from one generated module to another.
    uint64_t getSymbolAddress(const std::string &name) override;
    
private:
    JIT *_jit;
};

uint64_t MM::getSymbolAddress(const std::string &name) {
    uint64_t fun_addr = SectionMemoryManager::getSymbolAddress(name);
    if (fun_addr)
        return fun_addr;
    
    uint64_t jit_addr = (uint64_t)_get_symbol_address(_jit, name);
    if (!jit_addr)
        llvm::report_fatal_error("Program used extern function '" + name +
                           "' which could not be resolved!");
    return jit_addr;
}

JIT* create_jit(code_generator* cg){
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    auto jit = new JIT();
    jit->cg = cg;
    jit->engines = &g_engines;
    return jit;
}

void destroy_jit(JIT* jit){
    std::vector<void*>::iterator begin = (*jit->engines).begin();
    std::vector<void*>::iterator end = (*jit->engines).end();
    std::vector<void*>::iterator it;
    for (it = begin; it != end; ++it){
        llvm::ExecutionEngine* ee = (llvm::ExecutionEngine*)*it;
        delete ee;
    }
    delete jit;
}

uint64_t get_pointer_to_variable(JIT* jit, void* value){
    // std::vector<void*>::iterator begin = (*jit->engines).begin();
    // std::vector<void*>::iterator end = (*jit->engines).end();
    // std::vector<void*>::iterator it;
    // auto llvmValue = (llvm::Value*)value;
    // //fprintf(stderr, "getting pointer to function %d...\n", (intptr_t)(void*)fun);
    // for (it = begin; it != end; ++it) {
    //     //fprintf(stderr, "iterating 1..");
    //     llvm::ExecutionEngine* ee = (llvm::ExecutionEngine*)(*it);
    //     auto pgVar = ee->getGlobalValueAddress(global_name);
    //     //fprintf(stderr, "iterating execution engine:%ld..\n", (long)p_fun);
    //     if (pgVar){
    //         fprintf(stderr, "found the global value. returning it\n");
    //         return pgVar;
    //     }
    // }
    // return 0;
    return 0;
}

void* get_pointer_to_function(JIT* jit, void* fun) {
    // See if an existing instance of MCJIT has this function.
    std::vector<void*>::iterator begin = (*jit->engines).begin();
    std::vector<void*>::iterator end = (*jit->engines).end();
    std::vector<void*>::iterator it;
    //fprintf(stderr, "getting pointer to function %d...\n", (intptr_t)(void*)fun);
    for (it = begin; it != end; ++it) {
        //fprintf(stderr, "iterating 1..");
        llvm::ExecutionEngine* ee = (llvm::ExecutionEngine*)(*it);
        void *p_fun = ee->getPointerToFunction((llvm::Function*)fun);
        fprintf(stderr, "iterating execution engine:%ld..\n", (long)p_fun);
        if (p_fun){
            fprintf(stderr, "found the function. returning it\n");
            return p_fun;
        }
    }
    
    // If we didn't find the function, see if we can generate it.
    //fprintf(stderr, "didn't find the function, regenerating :%d... ", _code_generator->_module?1:0);
    if (jit->cg->module) {
        std::string ErrStr;
        llvm::ExecutionEngine *execution_engine =
        llvm::EngineBuilder(std::unique_ptr<llvm::Module>((llvm::Module*)jit->cg->module))
        .setErrorStr(&ErrStr)
        .setMCJITMemoryManager(std::unique_ptr<MM>(new MM(jit)))
        .create();
        if (!execution_engine) {
            log(ERROR, "Could not create ExecutionEngine: %s", ErrStr.c_str());
            exit(1);
        }
        
        llvm::Module* module = (llvm::Module*)jit->cg->module;
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
        
        //jit->cg->module = NULL;
        //(*jit->engines).push_back(execution_engine);
        execution_engine->finalizeObject();
        return execution_engine->getPointerToFunction((llvm::Function*)fun);
    }
    return 0;
}

void* _get_symbol_address(JIT* jit, const std::string &name) {
    // Look for the symbol in each of our execution engines.
    std::vector<void*>::iterator begin = (*jit->engines).begin();
    std::vector<void*>::iterator end = (*jit->engines).end();
    std::vector<void*>::iterator it;
    for (it = begin; it != end; ++it) {
        uint64_t fun_addr = ((llvm::ExecutionEngine*)(*it))->getFunctionAddress(name);
        if (fun_addr) {
            return (void *)fun_addr;
        }
    }
    return 0;
}

