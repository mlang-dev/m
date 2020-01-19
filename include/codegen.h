#include "parser.h"

using namespace std;

typedef struct CodeGenerator{
    void* context;
    void* builder;
    Parser* parser;
    map<std::string, void* > named_values; //llvm::AllocaInst*
    vector<void *> modules;
    void* module;
}CodeGenerator;

CodeGenerator* createCodeGenerator(void* context, void* builder, Parser* parser);

void* generate(CodeGenerator* cg, ExpNode* node);

// void* generateNumNode(CodeGenerator* cg, NumNode* node);
// void* generateIdentNode(CodeGenerator* cg, IdentNode* node);
// void* generateBinaryNode(CodeGenerator* cg, BinaryNode* node);
// void* generateCallExpNode(CodeGenerator* cg, CallExpNode* node);
void* generatePrototypeNode(CodeGenerator* cg, PrototypeNode* node);
void* generateFunctionNode(CodeGenerator* cg, FunctionNode* node);
// void* generateConditionNode(CodeGenerator* cg, ConditionNode* node);
// void* generateForNode(CodeGenerator* cg, ForNode* node);
// void* generateUnaryNode(CodeGenerator* cg, UnaryNode* node);
// void* generateVarNode(CodeGenerator* cg, VarNode* node);

// class LLVMCodeGenerator
// {
//     typedef std::vector<llvm::Module *> ModuleVector;
//     ModuleVector _modules;

//     std::map<std::string, llvm::AllocaInst* > _named_values;
//     llvm::IRBuilder<>* _builder;
//     llvm::LLVMContext _context;
    
//     Parser* _parser;
    
//     llvm::AllocaInst* _CreateEntryBlockAlloca(llvm::Function *fun, const std::string &var_name);
//     void _CreateArgumentAllocas(PrototypeNode* node, llvm::Function *fun);
// public:
//     llvm::Module* _module;
    
//     LLVMCodeGenerator(Parser* parser);
//     ~LLVMCodeGenerator();
//     llvm::Module *GetModuleForNewFunction();
//     void Dump();
//     llvm::Function *GetFunction(const std::string fun_name);
// };

