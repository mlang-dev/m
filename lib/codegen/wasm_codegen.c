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
#include "parser/parser.h"
#include "sema/analyzer.h"
#include <assert.h>
#include <stdint.h>
#include <float.h>

const char wasm_magic_number[] = {0, 'a', 's', 'm'};
u8 wasm_version[] = {0x01, 0, 0, 0};
u8 type_2_const[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/0,
    /*UNIT*/0,
    /*BOOL*/OPCODE_I32CONST,
    /*CHAR*/OPCODE_I32CONST,
    /*INT*/ OPCODE_I32CONST,
    /*FLOAT*/OPCODE_F32CONST,
    /*DOUBLE*/OPCODE_F64CONST,
    /*STRING*/0,
    /*FUNCTION*/0,
    /*EXT*/0,
};

u8 type_2_wtype[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ TYPE_I32,
    /*CHAR*/ TYPE_I32,
    /*INT*/ TYPE_I32,
    /*FLOAT*/ TYPE_F32,
    /*DOUBLE*/ TYPE_F64,
    /*STRING*/ 0,
    /*FUNCTION*/ 0,
    /*EXT*/ 0,
};

u8 op_maps[OP_TOTAL][TYPE_TYPES] = {
    /*
    UNK, GENERIC, UNIT, BOOL, CHAR, INT, FLOAT, DOUBLE, STRING, FUNCTION, EXT     
    */
    /*OP_NULL   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_DOT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, },
    /*OP_OR     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, },
    /*OP_AND    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_NOT    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_BNOT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BOR    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BEOR   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BAND   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BSL    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BSR    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_EXPO   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_TIMES  */{0, 0, 0, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_F32MUL, OPCODE_F64MUL, 0, 0, 0, },
    /*OP_DIV    */{0, 0, 0, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_F32DIV, OPCODE_F64DIV, 0, 0, 0, },
    /*OP_MOD    */{0, 0, 0, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, 0, 0, 0, 0, 0, },
    /*OP_PLUS   */{0, 0, 0, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_F32ADD, OPCODE_F64ADD, 0, 0, 0, },
    /*OP_MINUS  */{0, 0, 0, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_F32SUB, OPCODE_F64SUB, 0, 0, 0, },

    /*OP_LT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_LE  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_EQ    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_GT    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_GE   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_NE  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_COND  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    
    /*OP_MUL_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_DIV_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_MOD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_ADD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_SUB_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_LEFT_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_RIGHT_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_AND_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_XOR_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_OR_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_INC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_DEC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
} ;

void wasm_codegen_init()
{
}

void _emit_code(struct byte_array *ba, struct ast_node *node);

//LEB128 encoding
u8 _emit_uint(struct byte_array *ba, u64 value)
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

u8 _emit_int(struct byte_array *ba, i64 value)
{
    int more = 1;
    u8 byte;
    u8 sign_bit;
    u8 index = 0;
    while(more) {
        byte = 0x7F & value; // low 7 bits of value
        sign_bit = byte & 0x40;
        value >>= 7; // this is arithmetic shift
        if ((value == 0 && !sign_bit) || (value == -1 && sign_bit)) {
            more = 0;
        }else{
            byte |= 0x40;
        }
        ba_add(ba, byte);
        index++;
    };
    return index;
}

/*IEEE 754 2019 little endian in bytes*/
u8 _emit_f32(struct byte_array *ba, f32 value)
{
    u8 size = sizeof(value);
    u8 byte;
    u32 bits;
    memcpy(&bits, &value, size);
    for(u8 i = 0; i < size; i++) {
        byte = 0xFF & bits; // low 7 bits of value
        bits >>= 8;
        ba_add(ba, byte);
    } 
    return size;//return 
}

u8 _emit_f64(struct byte_array *ba, f64 value)
{
    u8 size = sizeof(value);
    u8 byte;
    u64 bits;
    memcpy(&bits, &value, size);
    for (u8 i = 0; i < size; i++) {
        byte = 0xFF & bits; // low 7 bits of value
        bits >>= 8;
        ba_add(ba, byte);
    }
    return size; // return
}

void _emit_literal(struct byte_array *ba, struct ast_node *node)
{
    assert(node->type && node->type->type < TYPE_TYPES && node->type->type >= 0);
    ba_add(ba, type_2_const[node->type->type]);
    switch(node->type->type){
        default:
            assert(false);
        case TYPE_INT:
            _emit_int(ba, node->liter->int_val);
            break;
        case TYPE_FLOAT:
            _emit_f32(ba, node->liter->double_val);
            break;
        case TYPE_DOUBLE:
            _emit_f64(ba, node->liter->double_val);
            break;
    }
    
}

void _emit_unary(struct byte_array *ba, struct ast_node *node)
{
    struct ast_node *zero = 0;
    if (node->unop->opcode == OP_MINUS){
        zero = int_node_new(0, node->loc);
        zero->type = node->type;
        _emit_code(ba, zero);
    }
    _emit_code(ba, node->unop->operand);
    enum type type_index = prune(node->unop->operand->type)->type;
    assert(type_index >= 0 && type_index < TYPE_TYPES);
    assert(node->unop->opcode >= 0 && node->unop->opcode < OP_TOTAL);
    if(zero){
        u8 opcode = op_maps[node->unop->opcode][type_index];
        ba_add(ba, opcode);
        ast_node_free(zero);
    }
}

void _emit_binary(struct byte_array *ba, struct ast_node *node)
{
    _emit_code(ba, node->binop->lhs);
    _emit_code(ba, node->binop->rhs);
    enum type type_index = prune(node->binop->lhs->type)->type;
    assert(type_index >= 0 && type_index < TYPE_TYPES);
    assert(node->binop->opcode >= 0 && node->binop->opcode < OP_TOTAL);
    u8 opcode = op_maps[node->binop->opcode][type_index];
    ba_add(ba, opcode);
}

void _emit_func(struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == FUNC_NODE);
    struct byte_array func;
    ba_init(&func, 17);
    ba_add(&func, 0x00); //num local variables
    _emit_code(&func, node->func->body);
    ba_add(&func, OPCODE_END);
    //function body
    _emit_uint(ba, func.size); //function body size
    ba_add2(ba, &func);
    ba_deinit(&func);
}

void _emit_block(struct byte_array *ba, struct ast_node *node)
{   
    u32 block_size = array_size(&node->block->nodes);
    for(u32 i = 0; i < block_size; i++){
        struct ast_node *child = *(struct ast_node **)array_get(&node->block->nodes, i);
        _emit_code(ba, child);
    }
}

void _emit_code(struct byte_array *ba, struct ast_node *node)
{
    switch(node->node_type){
        case FUNC_NODE:
            _emit_func(ba, node);
            break;
        case BLOCK_NODE:
            _emit_block(ba, node);
            break;
        case BINARY_NODE:
            _emit_binary(ba, node);
            break;
        case UNARY_NODE:
            _emit_unary(ba, node);
            break;
        case LITERAL_NODE:
            _emit_literal(ba, node);
            break;
        default:
            assert(false);
    }
}

struct byte_array emit_wasm(struct ast_node *node)
{   
    struct byte_array ba, section;
    ba_init(&ba, 17); ba_init(&section, 17);
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
    _emit_uint(&section, num_func);
    struct ast_node *func;
    u32 i, size, j;
    struct type_oper *to;
    for (i = 0; i < num_func; i++) {
        func = *(struct ast_node **)array_get(&node->block->nodes, i);
        assert(func->node_type == FUNC_NODE);
        assert(func->type->kind == KIND_OPER);
        struct type_oper *func_type = (struct type_oper *)func->type;
        u8 num_params = array_size(&func_type->args) - 1;
        ba_add(&section, TYPE_FUNC);
        ba_add(&section, num_params); // num params
        for(j = 0; j < num_params; j++){
            to = *(struct type_oper **)array_get(&func_type->args, j);
            assert(to->base.type > 0 && to->base.type < TYPE_TYPES);
            ba_add(&section, type_2_wtype[to->base.type]);
        }
        to = *(struct type_oper **)array_back(&func_type->args);
        assert(to->base.type > 0 && to->base.type < TYPE_TYPES);
        ba_add(&section, 0x01); // num result
        ba_add(&section, type_2_wtype[to->base.type]); // i32 output
    }
    _emit_uint(&ba, section.size); // set section size
    ba_add2(&ba, &section);       // copy section data

    ba_reset(&section);
    // function section
    ba_add(&ba, FUNCTION_SECTION);
    ba_add(&section, num_func); // one func types
    for(i = 0; i < num_func; i++)
        ba_add(&section, i); // index 0 of type sections
    _emit_uint(&ba, section.size); // section size
    ba_add2(&ba, &section);
    ba_reset(&section);

    // export section
    ba_add(&ba, EXPORT_SECTION);
    _emit_uint(&section, num_func); //num of exports
    u8 *str;
    for(i=0; i<num_func; i++){
        func = *(struct ast_node **)array_get(&node->block->nodes, i);
        size = string_size(func->func->func_type->ft->name);
        str = (u8*)string_get(func->func->func_type->ft->name);
        _emit_uint(&section, size);
        for(j = 0; j < size; j++){
            ba_add(&section, str[j]);
        }
        ba_add(&section, EXPORT_FUNC);
        _emit_uint(&section, i); //func index
    }
    _emit_uint(&ba, section.size); // section size
    ba_add2(&ba, &section);
    ba_reset(&section);

    //code section
    ba_add(&ba, CODE_SECTION);
    _emit_uint(&section, num_func); //num functions
    _emit_code(&section, node);
    _emit_uint(&ba, section.size); // section size
    ba_add2(&ba, &section);
    ba_reset(&section);
    ba_deinit(&section);
    return ba;
}

struct ast_node *_create_block_node()
{
    struct array children;
    array_init(&children, sizeof(struct ast_node *));
    return block_node_new(&children);
}


/*
 * collect global statements into _start function
 */
struct ast_node *_decorate_as_module(struct hashtable *symbol_2_int_types, struct ast_node *block)
{
    assert(block->node_type == BLOCK_NODE);
    struct array _start_nodes;
    array_init(&_start_nodes, sizeof(struct ast_node *));
    u32 nodes = array_size(&block->block->nodes);
    struct ast_node *module = _create_block_node();
    struct ast_node *node;
    for (u32 i = 0; i < nodes; i++) {
        node = *(struct ast_node **)array_get(&block->block->nodes, i);
        if (node->node_type == FUNC_NODE){
            block_node_add(module, node);
        }else{
            array_push(&_start_nodes, &node);
        }
    }
    struct ast_node *_start_func = wrap_nodes_as_function(symbol_2_int_types, to_symbol("_start"), &_start_nodes);
    block_node_add(module, _start_func);
    free_block_node(block, false);
    return module;
}

struct byte_array parse_as_module(const char *expr)
{
    frontend_init();
    wasm_codegen_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = _decorate_as_module(&parser->symbol_2_int_types, parse_code(parser, expr));
    struct sema_context *c = sema_context_new(&parser->symbol_2_int_types, 0, 0, 0);
    analyze(c, ast);
    struct byte_array ba = emit_wasm(ast);
    ast_node_free(ast);
    sema_context_free(c);
    parser_free(parser);
    frontend_deinit();
    return ba;
}
