#include "codegen/codegen.h"
#include "codegen/x86_64_abi.h"
#include "codegen/type_size_info.h"

enum Class {
    INTEGER,  //This class consists of integral types that fit into one of the general purpose registers.
    SSE,  // The class consists of types that fit into a vector register.
    SSEUP, //The class consists of types that fit into a vector register and can be passed and returned in the upper bytes of it.
    X87, //These classes consists of types that will be returned via the x87 FPU.
    X87UP,
    COMPLEX_X87, //This class consists of types that will be returned via the x87 FPU.
    NO_CLASS, //This class is used as initializer in the algorithms. It will be used for padding and empty structures and unions.
    MEMORY //This class consists of types that will be passed and returned in memory via the stack.
};

void _classify(struct code_generator *cg, struct type_exp* te, uint64_t offset_base, enum Class *low, enum Class *high)
{
    enum Class *current;
    *low = *high = NO_CLASS;
    current = offset_base < 64 ? low : high;
    *current = MEMORY;
    if (te->type < TYPE_EXT){
        //builtin types
        if (te->type == TYPE_UNIT){
            *current = NO_CLASS;
        } //else if (te->type == TYPE_INT128 or UINT128) { *low = *high = INTEGER;}
        else if (te->type == TYPE_BOOL || te->type == TYPE_STRING){
            *current = INTEGER;
        }
        else if (te->type == TYPE_DOUBLE){
            *current = SSE;
        }//TODO: LONG_DOUBLE
    } /*else if pointer then make current pointing to INTEGER*/
    //TODO: vector, complex, int type with specified bitwidth, constant array
    else if (te->type == TYPE_EXT) {
        //struct type, TargetInfo.cpp 3016
        struct type_size_info tsi = get_type_size_info(cg, te);
        
    }
}

struct abi_arg_info _classify_return_type(struct code_generator *cg, struct exp_node *exp)
{
    enum Class low, high;
    _classify(cg, exp->type, 0, &low, &high);
}

///compute abi info
void x86_64_update_abi_info(struct code_generator *cg, struct cg_fun_args *fa)
{
    unsigned free_int_regs = 6;
    unsigned free_sse_regs = 8;
    unsigned needed_int, needed_sse;
    fa->ret.info = _classify_return_type(cg, fa->ret.node);
}