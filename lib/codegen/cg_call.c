#include "codegen/cg_call.h"

struct address zero_address()
{
    struct address adr;
    adr.pointer = 0;
    adr.alignment = 0;
    return adr;
}

void emit_call()
{

}