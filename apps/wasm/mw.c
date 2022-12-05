#include "wasm/mw.h"
#include "clib/typedef.h"
#include "compiler/engine.h"
#include "codegen/wasm/cg_wasm.h"
#include <stdlib.h>
#include <string.h>

const char *version()
{
    void * p = malloc(32);
    strcpy(p, "m - 0.0.38");
    return p;
}

u32 code_size = 0;

u8 *compile_code(const char *text)
{
    struct engine *engine = engine_wasm_new();
    struct cg_wasm *cg = (struct cg_wasm*)engine->be->cg;
    compile_to_wasm(engine, text);
    u8 *data = cg->ba.data;
    code_size = cg->ba.size;
    cg->ba.data = 0;
    free((void *)text);
    engine_free(engine);
    return data;
}

u32 get_code_size()
{
    return code_size;
}
