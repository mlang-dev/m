#include "parser.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

class LLVMCodeGenerator : public CodeGenerator
{
    typedef std::vector<llvm::Module *> ModuleVector;
    ModuleVector _modules;

    std::map<std::string, llvm::AllocaInst* > _named_values;
    llvm::IRBuilder<>* _builder;
    llvm::LLVMContext _context;
    
    Parser* _parser;
    
    llvm::AllocaInst* _CreateEntryBlockAlloca(llvm::Function *fun, const std::string &var_name);
    void _CreateArgumentAllocas(PrototypeNode* node, llvm::Function *fun);
public:
    llvm::Module* _module;
    
    void* generate(NumNode* node);
    void* generate(IdentNode* node);
    void* generate(BinaryNode* node);
    void* generate(CallExpNode* node);
    void* generate(PrototypeNode* node);
    void* generate(FunctionNode* node);
    void* generate(ConditionNode* node);
    void* generate(ForNode* node);
    void* generate(UnaryNode* node);
    void* generate(VarNode* node);
    LLVMCodeGenerator(Parser* parser);
    ~LLVMCodeGenerator();
    llvm::Module *GetModuleForNewFunction();
    void Dump();
    llvm::Function *GetFunction(const std::string fun_name);
};

