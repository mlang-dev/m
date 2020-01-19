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
#include "codegen.h"
#include "util.h"

class JIT {    
public:
    JIT(CodeGenerator* cg);
    ~JIT();

    llvm::Function *GetFunction(const std::string fun_name);
    void *GetPointerToFunction(llvm::Function *fun);
    void *GetSymbolAddress(const std::string &Name);

private:
    typedef std::vector<llvm::ExecutionEngine *> EngineVector;
    
    CodeGenerator* _cg;
    EngineVector _engines;
};

class MM : public llvm::SectionMemoryManager {
    MM(const MM &) = delete;
    void operator=(const MM &) = delete;
    
public:
    MM(JIT *jit) : _jit(jit) {}
    ~MM() override {}
    
    /// This method returns the address of the specified symbol.
    /// Our implementation will attempt to find symbols in other
    /// modules associated with the MCJITHelper to cross link symbols
    /// from one generated module to another.
    uint64_t getSymbolAddress(const std::string &name) override;
    
private:
    JIT *_jit;
};