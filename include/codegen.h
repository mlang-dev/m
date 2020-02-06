#include "parser.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"

using namespace std;

typedef struct code_generator{
    void* context;
    void* builder;
    parser* parser;
    map<std::string, void*> named_values; //llvm::AllocaInst*
    //vector<void *> modules;
    unique_ptr<llvm::Module> module;
    unique_ptr<llvm::legacy::FunctionPassManager> fpm;
    map<string, prototype_node*> protos;
    map<string, var_node*> gvs; 
} code_generator;

code_generator* create_code_generator(parser* parser);
void destroy_code_generator(code_generator* cg);
void create_module_and_pass_manager(code_generator* cg);
void* generate_code(code_generator* cg, exp_node* node);
void generate_runtime_module(code_generator* cg, parser* parser);
