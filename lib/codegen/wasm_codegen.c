/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen functions
 * 
 */
#include "codegen/wasm_codegen.h"
#include "parser/tok.h"
#include "clib/array.h"
#include <assert.h>

symbol BINOP = 0;
symbol UNOP = 0;
symbol FUNC = 0;
const char *ops[256];
void wasm_codegen_init()
{
    BINOP = to_symbol2_0("binop");
    UNOP = to_symbol2_0("unop");
    FUNC = to_symbol2_0("func");
    ops['+'] = "i32.add";
    ops['-'] = "i32.sub";
    ops['*'] = "i32.mul";
    ops['/'] = "i32.div";
}
/*
unsigned char *_wasm_generate(struct ast_node *ast, const char *text)
{
    string s;
    string_init(&s);
    if(!ast){
        return s;
    }
    if(ast->node_type == FUNC){
        string_append(&s, "(func $");
        struct ast_node *fname = *(struct ast_node**)array_get(&ast->children, 0);
        assert(fname->node_type == IDENT_TOKEN);
        string_add_chars2(&s, &text[fname->loc.start], fname->loc.end - fname->loc.start);
        string_append(&s, " (result i32)\n");
        //func body
        struct ast_node *fbody = *(struct ast_node**)array_back(&ast->children);
        string s_fbody = _wasm_generate(fbody, text);
        string_add2(&s, &s_fbody);
        string_append(&s, ")\n");

        //export the function
        string_append(&s, "(export \"");
        string_add_chars2(&s, &text[fname->loc.start], fname->loc.end - fname->loc.start);
        string_append(&s, "\" (func $");
        string_add_chars2(&s, &text[fname->loc.start], fname->loc.end - fname->loc.start);
        string_append(&s, ")");
        string_append(&s, ")\n");
    }
    else if (ast->node_type == BINOP){
        //0, 2 is operand, 1 is operator
        assert(array_size(&ast->children)==3);
        struct ast_node *op = *(struct ast_node**)array_get(&ast->children, 1);
        string_append(&s, "(");
        string_append(&s, ops[(int)text[op->loc.start]]);
        string_append(&s, "\n");
        struct ast_node *child = *(struct ast_node**)array_get(&ast->children, 0);
        string op1 = _wasm_generate(child, text);
        string_add2(&s, &op1);
        child = *(struct ast_node**)array_get(&ast->children, 2);
        string op2 = _wasm_generate(child, text);
        string_add2(&s, &op2);
        string_append(&s, ")\n");
    }
    else if(ast->node_type == UNOP){

    }
    else if(ast->node_type == NUM_TOKEN){
        string_append(&s, "(i32.const ");
        string_add_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
        string_append(&s, ")\n");
    }
    else if(ast->node_type){
        string_init_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
    }
    else {
        string_init(&s);
    }

    return s;
}

string wasm_generate(struct ast_node *ast, const char *text)
{
    string s;
    string_init_chars2(&s, "\n", 1);
    //string wat = _wasm_generate(ast, text);
    string_add2(&s, &wat);
    return s;
}
*/
