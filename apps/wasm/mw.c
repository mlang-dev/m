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
    strcpy(p, "m - 0.0.20");
    return p;
}

u32 code_size = 0;

EXPORT u8 *compile_code(const char *text)
{
    struct wasm_module module;
    wasm_codegen_init(&module);
    parse_as_module(&module, text);
    u8 *data = module.ba.data;
    code_size = module.ba.size;
    module.ba.data = 0;
    free_mem((void *)text);
    wasm_codegen_deinit(&module);
    return data;
}

EXPORT u32 get_code_size()
{
    return code_size;
}

EXPORT void print(const char *restrict fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}
