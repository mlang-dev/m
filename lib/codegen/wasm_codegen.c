#include "codegen/wasm_codegen.h"
#include "parser/tok.h"
#include "clib/array.h"
#include <assert.h>

symbol BINOP = 0;
symbol UNOP = 0;
const char *ops[256];
void wasm_codegen_init()
{
    BINOP = to_symbol2("binop", 5);
    UNOP = to_symbol2("unop", 4);
    ops['+'] = "i32.add";
    ops['-'] = "i32.sub";
    ops['*'] = "i32.mul";
    ops['/'] = "i32.div";
}

string _generate(struct ast_node *ast, const char *text)
{
    string s;
    string_init(&s);
    if(!ast){
        return s;
    }
    if (ast->node_type == BINOP){
        //0, 2 is operand, 1 is operator
        assert(array_size(&ast->children)==3);
        struct ast_node *child = *(struct ast_node**)array_get(&ast->children, 0);
        string op1 = _generate(child, text);
        string_add2(&s, &op1);
        child = *(struct ast_node**)array_get(&ast->children, 2);
        string op2 = _generate(child, text);
        string_add2(&s, &op2);
        struct ast_node *op = *(struct ast_node**)array_get(&ast->children, 1);
        string_append(&s, ops[text[op->loc.start]]);
        string_append(&s, "\n");
    }
    else if(ast->node_type == UNOP){

    }
    else if(ast->node_type == NUM_TOKEN){
        string_append(&s, "i32.const ");
        string_add_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
        string_append(&s, "\n");
    }
    else if(ast->node_type){
        string_init_chars2(&s, &text[ast->loc.start], ast->loc.end - ast->loc.start);
    }
    else {
        string_init(&s);
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

string generate(struct ast_node *ast, const char *text)
{
    string s;
    string_init_chars2(&s, "\n", 1);
    string wat = _generate(ast, text);
    string_add2(&s, &wat);
    return s;
}
