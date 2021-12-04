/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * functions to convert ast_node to wasm module
 * 
 */
#include "codegen/ast_wasm.h"
#include "wasm/wasm.h"
#include "parser/tok.h"
#include "clib/array.h"
#include "clib/util.h"
#include <assert.h>
#include <stdint.h>

symbol BINOP = 0;
symbol UNOP = 0;
symbol FUNC = 0;

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

void _wasm_emit(struct ast_node *ast)
{
    if(!ast){
        return;
    }
    if(ast->node_type == FUNC){
        struct ast_node *fname = *(struct ast_node**)array_get(&ast->children, 0);
        assert(fname->node_type == IDENT_TOKEN);
        //string_add_chars2(&s, &c->code[fname->loc.start], fname->loc.end - fname->loc.start);
        //func body
        struct ast_node *fbody = *(struct ast_node**)array_back(&ast->children);
        _wasm_emit(c, fbody);

        //export the function
        // string_append(&s, "(export \"");
        // string_add_chars2(&s, &text[fname->loc.start], fname->loc.end - fname->loc.start);
        // string_append(&s, "\" (func $");
        // string_add_chars2(&s, &text[fname->loc.start], fname->loc.end - fname->loc.start);
        // string_append(&s, ")");
        // string_append(&s, ")\n");
    }
    else if (ast->node_type == BINOP){
        //0, 2 is operand, 1 is operator
        // assert(array_size(&ast->children)==3);
        // struct ast_node *op = *(struct ast_node**)array_get(&ast->children, 1);
        // string_append(&s, "(");
        // string_append(&s, ops[(int)text[op->loc.start]]);
        // string_append(&s, "\n");
        // struct ast_node *child = *(struct ast_node**)array_get(&ast->children, 0);
        // string op1 = _wasm_generate(child, text);
        // string_add2(&s, &op1);
        // child = *(struct ast_node**)array_get(&ast->children, 2);
        // string op2 = _wasm_generate(child, text);
        // string_add2(&s, &op2);
        // string_append(&s, ")\n");
    }
    else if(ast->node_type == UNOP){

    }
    else if(ast->node_type == NUM_TOKEN){
        // string_append(&s, "(i32.const ");
        // string_add_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
        // string_append(&s, ")\n");
    }
    else if(ast->node_type){
        // string_init_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
    }
    else {
        // string_init(&s);
    }
}

struct wasm_module * to_wasm_module(struct amodule mod)
{   
    struct wasm_module *wm = wasm_module_new();

    return vm;
}

