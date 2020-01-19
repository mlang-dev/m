#include "parser.h"

using namespace std;

typedef struct code_generator{
    void* context;
    void* builder;
    Parser* parser;
    map<std::string, void* > named_values; //llvm::AllocaInst*
    vector<void *> modules;
    void* module;
} code_generator;

code_generator* create_code_generator(Parser* parser);
void destroy_code_generator(code_generator* cg);

void* generate(code_generator* cg, ExpNode* node);

void* generate_prototype_node(code_generator* cg, PrototypeNode* node);
void* generate_function_node(code_generator* cg, FunctionNode* node);

