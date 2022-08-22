#include <stdio.h>
#include <stdlib.h>
#include "codegen/wasm_codegen.h"
#include <string.h>
#include "clib/typedef.h"

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

struct byte_array ba;

EXPORT u8 *parse_expression(const char *text)
{
    string func;
    string_init_chars(&func, "let run() = ");
    string_add_chars(&func, text);
    ba = parse_as_module(string_get(&func));
    free_mem((void*)text);
    string_deinit(&func);
    return ba.data;
}

EXPORT u32 get_code_size()
{
    return ba.size;
}
