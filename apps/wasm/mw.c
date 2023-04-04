#include "wasm/mw.h"
#include "clib/typedef.h"
#include "clib/string.h"
#include "compiler/engine.h"
#include "codegen/wasm/cg_wasm.h"
#include "lexer/lexer.h"
#include "app/app.h"

#include <stdlib.h>
#include <string.h>



const char *version()
{
    return engine_version();
}

u32 code_size = 0;

u8 *compile_code(const char *text)
{
    app_init();
    struct engine *engine = engine_wasm_new();
    struct cg_wasm *cg = (struct cg_wasm*)engine->be->cg;
    compile_to_wasm(engine, text);
    u8 *data = cg->ba.data;
    code_size = cg->ba.size;
    cg->ba.data = 0;
    free((void *)text);
    engine_free(engine);
    app_deinit();
    return data;
} 

u8 *highlight_code(const char *text)
{
    app_init();
    struct frontend *fe = frontend_init();
    struct lexer * lexer = lexer_new_with_string(text);
    const char *highlighted = highlight(lexer, text);
    lexer_free(lexer);
    frontend_deinit(fe);
    app_deinit();
    return (u8*)highlighted;
}

u32 get_code_size()
{
    return code_size;
}
