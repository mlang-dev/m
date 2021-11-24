#include <stdio.h>
#include <stdlib.h>

#define WASM_EXPORT __attribute__((visibility("default"))) 

WASM_EXPORT void* alloc(size_t bytes)
{
    return malloc(bytes);
}
