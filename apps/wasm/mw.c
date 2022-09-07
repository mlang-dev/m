#include "wasm/mw.h"
#include "clib/typedef.h"
#include "codegen/wasm_codegen.h"


const char *version()
{
    void * p = malloc(32);
    strcpy(p, "m - 0.0.22");
    return p;
}

u32 code_size = 0;

u8 *compile_code(const char *text)
{
    struct wasm_module module;
    wasm_codegen_init(&module);
    parse_as_module(&module, text);
    u8 *data = module.ba.data;
    code_size = module.ba.size;
    module.ba.data = 0;
    free((void *)text);
    wasm_codegen_deinit(&module);
    return data;
}

u32 get_code_size()
{
    return code_size;
}
