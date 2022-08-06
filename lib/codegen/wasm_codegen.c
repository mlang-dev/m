/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen functions
 * 
 */
#include "codegen/wasm_codegen.h"
#include "clib/array.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/init.h"
#include "lexer/lexer.h"
#include "parser/amodule.h"
#include "parser/lalr_parser.h"
#include "parser/m_grammar.h"
#include "sema/analyzer.h"
#include <assert.h>
#include <stdint.h>

symbol BINOP = 0;
symbol UNOP = 0;
symbol FUNC = 0;

//u8 ops[256];
const char wasm_magic_number[] = {0, 'a', 's', 'm'};
u8 wasm_version[] = {0x01, 0, 0, 0};

void wasm_codegen_init()
{
    BINOP = to_symbol("binop");
    UNOP = to_symbol("unop");
    FUNC = to_symbol("func");
    // ops['+'] = OPCODE_I32ADD;
    // ops['-'] = OPCODE_I32SUB;
    // ops['*'] = OPCODE_I32MUL;
    // ops['/'] = OPCODE_I32DIV_S;
}

struct context{
    struct byte_array ba;
    const char *code;
};

//LEB128 encoding
u8 _emit_u32(struct byte_array *ba, u32 value)
{
    u8 byte;
    u8 index = 0;
    do{
        byte = 0x7F & value; //low 7 bits of value
        value >>= 7;
        if(value != 0){
            byte |= 0x80;    //set bit 7 as 1, more bytes to come
        }
        ba_add(ba, byte);
        index++;
    }while(value!=0);
    return index;
}

void _wasm_emit(struct byte_array *ba, struct ast_node *ast)
{
    //code section
    ba_add(ba, CODE_SECTION);
    ba_add(ba, 0x09); //section size
    ba_add(ba, 0x01); //num functions
    //function body
    ba_add(ba, 0x07); //function body size
    ba_add(ba, 0x00); //num local variables
    ba_add(ba, OPCODE_I32CONST);
    ba_add(ba, 1);
    ba_add(ba, OPCODE_I32CONST);
    ba_add(ba, 2);
    ba_add(ba, OPCODE_I32ADD);
    ba_add(ba, OPCODE_END);
}

struct byte_array wasm_emit(struct ast_node *node)
{   
    struct byte_array ba, section;
    ba_init(&ba, 2); ba_init(&section, 17);
    for(size_t i = 0; i < ARRAY_SIZE(wasm_magic_number); i++){
        ba_add(&ba, wasm_magic_number[i]);
    }
    for(size_t i = 0; i < ARRAY_SIZE(wasm_version); i++){
        ba_add(&ba, wasm_version[i]);
    }
    // 0.    custom section
    // 1.    type section       : type signature of the function
    // 2.    import section
    // 3.    function section   : associate code section with type section
    // 4.    table section
    // 5.    memory section
    // 6.    global section
    // 7.    export section
    // 8.    start section
    // 9.    element section
    // 10.   code section       : local variable info and bytecode of function
    // 11.   data section
    // 12.   data count section
    // type section
    assert(node->node_type == BLOCK_NODE);
    u32 num_func;
    num_func = array_size(&node->block->nodes);
    ba_add(&ba, TYPE_SECTION);
    // section size
    // how many function types
    _emit_u32(&section, num_func);
    struct ast_node *func;
    for(u32 i = 0; i < num_func; i++){
        func = *(struct ast_node **)array_get(&node->block->nodes, i);
        assert(func->node_type == FUNC_NODE);
        ba_add(&section, TYPE_FUNC);
        ba_add(&section, 0x00); // num params
        ba_add(&section, 0x01); // num result
        ba_add(&section, TYPE_I32); // i32 output
    }
    _emit_u32(&ba, section.size); // set section size
    ba_add2(&ba, &section);       // copy section data

    ba_reset(&section);
    // function section
    ba_add(&ba, FUNCTION_SECTION);
    ba_add(&ba, 0x02); // section size
    ba_add(&ba, 0x01); // one func types
    ba_add(&ba, 0x00); // index 0 of type sections

    // export section
    ba_add(&ba, EXPORT_SECTION);
    ba_add(&ba, 0x07); // section size

    ba_add(&ba, 0x01);
    ba_add(&ba, 0x03);
    ba_add(&ba, 'r');
    ba_add(&ba, 'u');
    ba_add(&ba, 'n');
    ba_add(&ba, EXPORT_FUNC);
    ba_add(&ba, 0x00);

    _wasm_emit(&ba, node);
    return ba;
}

struct byte_array parse_as_module(const char *expr)
{
    frontend_init();
    wasm_codegen_init();
    struct sema_context *c = sema_context_new(0, 0, 0);
    struct lalr_parser *parser = parser_new();
    struct ast_node *ast = parse_text(parser, expr);
    analyze(c, ast);
    struct byte_array ba = wasm_emit(ast);
    sema_context_free(c);
    lalr_parser_free(parser);
    frontend_deinit();
    return ba;
}
