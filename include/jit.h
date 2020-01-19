#include "codegen.h"
#include "util.h"

typedef struct JIT {    
    CodeGenerator* cg;
    std::vector<void*>* engines;
}JIT;

JIT* createJIT(CodeGenerator* cg);
void destroyJIT(JIT* jit);
void *GetFunction(JIT* jit, const std::string fun_name);
void *GetPointerToFunction(JIT* jit, void* fun);
void *GetSymbolAddress(JIT* jit, const std::string& name);
