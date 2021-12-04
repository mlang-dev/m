/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wat codegen functions
 * 
 */
#include "codegen/wat_codegen.h"
#include "parser/tok.h"
#include "clib/array.h"
#include <assert.h>

symbol BINOP = 0;
symbol UNOP = 0;
symbol FUNC = 0;
symbol PROG = 0;
const char *ops[256];
void wat_codegen_init()
{
    BINOP = to_symbol2_0("binop");
    UNOP = to_symbol2_0("unop");
    FUNC = to_symbol2_0("func");
    PROG = to_symbol2_0("prog");
    ops['+'] = "i32.add";
    ops['-'] = "i32.sub";
    ops['*'] = "i32.mul";
    ops['/'] = "i32.div";
}

string _wat_generate(struct ast_node *ast, const char *text)
{
    string s;
    string_init(&s);
    if(!ast){
        return s;
    }
    if(ast->node_type_name == FUNC){
        string_append(&s, "(func $");
        struct ast_node *fname = *(struct ast_node**)array_get(&ast->children, 0);
        assert(fname->node_type_name == IDENT_TOKEN);
        string_add_chars2(&s, &text[fname->loc.start], fname->loc.end - fname->loc.start);
        string_append(&s, " (result i32)\n");
        //func body
        struct ast_node *fbody = *(struct ast_node**)array_back(&ast->children);
        string s_fbody = _wat_generate(fbody, text);
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
    else if (ast->node_type_name == BINOP){
        //0, 2 is operand, 1 is operator
        assert(array_size(&ast->children)==3);
        struct ast_node *op = *(struct ast_node**)array_get(&ast->children, 1);
        string_append(&s, "(");
        string_append(&s, ops[(int)text[op->loc.start]]);
        string_append(&s, "\n");
        struct ast_node *child = *(struct ast_node**)array_get(&ast->children, 0);
        string op1 = _wat_generate(child, text);
        string_add2(&s, &op1);
        child = *(struct ast_node**)array_get(&ast->children, 2);
        string op2 = _wat_generate(child, text);
        string_add2(&s, &op2);
        string_append(&s, ")\n");
    }
    else if(ast->node_type_name == UNOP){

    }
    else if(ast->node_type_name == NUM_TOKEN){
        string_append(&s, "(i32.const ");
        string_add_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
        string_append(&s, ")\n");
    }
    else if(ast->node_type_name == PROG){
    }
    else if(ast->node_type_name){
        assert(false);
        string_init_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
    }
    else {
    }
    /*
    size_t child_count = array_size(&ast->children);
    for(size_t i = 0; i < child_count; i++){
        struct ast_node *child = *(struct ast_node**)array_get(&ast->children, i);
        string child_s = generate(child, text);
        if(string_size(&s))
            string_add_chars2(&s, " ", 1);
        string_add(&s, &child_s);
    }*/

    return s;
}

string wat_generate(struct ast_node *ast, const char *text)
{
    string s;
    string_init_chars2(&s, "\n", 1);
    string wat = _wat_generate(ast, text);
    string_add2(&s, &wat);
    return s;
}

const char *wat_module_start = "\n(module";
const char *wat_module_end = ")\n";

string wat_emit(struct amodule mod)
{
    string wat = wat_generate(mod.root_ast, mod.code);

    string wat_mod_code;
    string_init_chars2(&wat_mod_code, wat_module_start, strlen(wat_module_start));
    string_add2(&wat_mod_code, &wat);
    string_add_chars2(&wat_mod_code, wat_module_end, strlen(wat_module_end));

    string_deinit(&wat);
    return wat_mod_code;
}
