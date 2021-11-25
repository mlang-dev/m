#include <stdio.h>
#include <stdlib.h>
#include "wasm/wasm.h"

#define EXPORT __attribute__((visibility("default")))

EXPORT void* alloc(size_t bytes)
{
    return malloc(bytes);
}
