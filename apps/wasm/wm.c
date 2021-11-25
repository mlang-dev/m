#include <stdio.h>
#include <stdlib.h>
#include "wasm/wasm.h"

#define EXPORT __attribute__((visibility("default")))

EXPORT void* alloc(size_t bytes)
{
    return malloc(bytes);
}

EXPORT i32 add_math(i32 a, i32 b)
{
    return a + b;
}
