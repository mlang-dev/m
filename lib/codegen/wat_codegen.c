/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wat codegen functions
 * 
 */
#include "codegen/wat_codegen.h"
#include "lexer/lexer.h"
#include "clib/array.h"
#include "parser/ast.h"
#include <assert.h>


const char *ops[OP_TOTAL];
void wat_codegen_init()
{
    ops[OP_PLUS] = "i32.add";
    ops[OP_MINUS] = "i32.sub";
    ops[OP_TIMES] = "i32.mul";
    ops[OP_DIVISION] = "i32.div";
}

string _wat_generate(struct ast_node *ast, const char *text)
{
    string s;
    string_init(&s);
    if(!ast){
        return s;
    }
    if(ast->node_type == FUNC_NODE){
        string_append(&s, "(func $");
        string_add(&s, ast->func->func_type->ft->name);
        string_append(&s, " (result i32)\n");
        //func body
        string s_fbody = _wat_generate(ast->func->body, text);
        string_add(&s, &s_fbody);
        string_append(&s, ")\n");

        //export the function
        string_append(&s, "(export \"");
        string_add(&s, ast->func->func_type->ft->name);
        string_append(&s, "\" (func $");
        string_add(&s, ast->func->func_type->ft->name);
        string_append(&s, ")");
        string_append(&s, ")\n");
    }
    else if (ast->node_type == BINARY_NODE){
        string_append(&s, "(");
        string_append(&s, ops[(int)ast->binop->opcode]);
        string_append(&s, "\n");
        string op1 = _wat_generate(ast->binop->lhs, text);
        string_add(&s, &op1);
        string op2 = _wat_generate(ast->binop->rhs, text);
        string_add(&s, &op2);
        string_append(&s, ")\n");
    }
    else if(ast->node_type == UNARY_NODE){

    }
    else if(ast->node_type == LITERAL_NODE){
        string_append(&s, "(i32.const ");
        string_add_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
        string_append(&s, ")\n");
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
    string_add(&s, &wat);
    return s;
}

const char *wat_module_start = "\n(module";
const char *wat_module_end = ")\n";

string wat_emit(struct amodule mod)
{
    string wat = wat_generate(mod.root_ast, mod.code);

    string wat_mod_code;
    string_init_chars(&wat_mod_code, wat_module_start);
    string_add(&wat_mod_code, &wat);
    string_add_chars(&wat_mod_code, wat_module_end);

    string_deinit(&wat);
    return wat_mod_code;
}
