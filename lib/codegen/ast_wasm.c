/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * functions to convert ast_node to wasm module
 * 
 */
#include "codegen/ast_wasm.h"
#include "wasm/wasm.h"
#include "lexer/lexer.h"
#include "clib/array.h"
#include "clib/util.h"
#include <assert.h>
#include <stdint.h>

symbol BINOP = 0;
symbol UNOP = 0;
symbol FUNC = 0;

void wasm_codegen_init()
{
    BINOP = to_symbol("binop");
    UNOP = to_symbol("unop");
    FUNC = to_symbol("func");
    ops['+'] = OPCODE_I32ADD;
    ops['-'] = OPCODE_I32SUB;
    ops['*'] = OPCODE_I32MUL;
    ops['/'] = OPCODE_I32DIV_S;
}

struct context{
    struct byte_array ba;
    const char *code;
};

void _wasm_emit(struct ast_node *ast)
{
}

struct wasm_module * to_wasm_module(struct amodule mod)
{   
    struct wasm_module *wm = wasm_module_new();

    return vm;
}

