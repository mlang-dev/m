/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen functions
 * 
 */
#include "codegen/wasm_codegen.h"
#include "wasm/wasm.h"
#include "lexer/lexer.h"
#include "clib/array.h"
#include "clib/util.h"
#include <assert.h>
#include <stdint.h>

symbol BINOP = 0;
symbol UNOP = 0;
symbol FUNC = 0;

uint8_t ops[256];
const char wasm_magic_number[] = {0, 'a', 's', 'm'};
uint8_t wasm_version[] = {0x01, 0, 0, 0};

void wasm_codegen_init()
{
    BINOP = to_symbol2_0("binop");
    UNOP = to_symbol2_0("unop");
    FUNC = to_symbol2_0("func");
    ops['+'] = OPCODE_I32ADD;
    ops['-'] = OPCODE_I32SUB;
    ops['*'] = OPCODE_I32MUL;
    ops['/'] = OPCODE_I32DIV_S;
}

struct context{
    struct byte_array ba;
    const char *code;
};

void _wasm_emit(struct context *c, struct ast_node *ast)
{
}

struct byte_array wasm_emit(struct amodule mod)
{   
    struct context c;
    c.code = mod.code;
    ba_init(&c.ba, 0);
    for(size_t i = 0; i < ARRAY_SIZE(wasm_magic_number); i++){
        ba_add(&c.ba, wasm_magic_number[i]);
    }
    for(size_t i = 0; i < ARRAY_SIZE(wasm_version); i++){
        ba_add(&c.ba, wasm_version[i]);
    }
    _wasm_emit(&c, mod.root_ast);
    return c.ba;
}

