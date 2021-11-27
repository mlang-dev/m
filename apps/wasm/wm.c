#include <stdio.h>
#include <stdlib.h>
#include "wasm/wasm.h"
#include "wasm/mwat.h"
#include <string.h>

#define EXPORT __attribute__((visibility("default")))

EXPORT void* alloc_mem(size_t bytes)
{
    return malloc(bytes);
}

EXPORT void free_mem(void *data)
{
    free(data);
}

EXPORT i32 str_len(const char *chars)
{
    return strlen(chars);
}

EXPORT const char *version()
{
    void * p = alloc_mem(32);
    strcpy(p, "m 0.0.17");
    return p;
}

EXPORT const char *parse_code(const char *text)
{
    const char *wat = parse_exp_as_module(text);
    free_mem((void*)text);
    return wat;
}
