#include "clib/typedef.h"
#include "codegen/wasm_codegen.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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
    strcpy(p, "m 0.0.18");
    return p;
}

struct byte_array ba;

EXPORT u8 *compile_code(const char *text)
{
    ba = parse_as_module(text);
    free_mem((void *)text);
    return ba.data;
}

EXPORT u32 get_code_size()
{
    return ba.size;
}

EXPORT i32 print(const char *restrict fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vfprintf(stdout, fmt, ap);
    va_end(ap);
    return ret;
}
