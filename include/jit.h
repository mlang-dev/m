#include "codegen.h"
#include "util.h"

typedef struct JIT {    
    code_generator* cg;
    std::vector<void*>* engines;
}JIT;

JIT* create_jit(code_generator* cg);
void destroy_jit(JIT* jit);
void* get_fun_ptr_to_execute(JIT* jit, void* fun);
uint64_t get_pointer_to_variable(JIT* jit, void* value);
void optimize_function(JIT* jit, void* fun);
