#include <stdio.h>
#include <stdlib.h>
#include "wasm/wasm.h"
#include <string.h>
#include "clib/typedef.h"

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

struct byte_array ba;

EXPORT u8 *parse_code(const char *text)
{
    ba = parse_exp_as_module(text);
    free_mem((void*)text);
    return ba.data;
}

EXPORT u32 get_code_size()
{
    return ba.size;
}
