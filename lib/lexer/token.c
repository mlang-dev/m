/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * token.c implements common terminals required in m. we actually can generate the terminal list from grammar.
 */
#include "lexer/token.h"
#include "clib/hashtable.h"
#include <assert.h>

void token_init()
{
    struct token_pattern* tps = get_token_pattern_array();
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &tps[i];
        if(tp->name&&tp->pattern&&!tp->re){
            tp->re = regex_new(tp->pattern);
            assert(tp->re);
        }
        if(tp->name){
            tp->symbol_name = to_symbol(tp->name);
        }
    }
}

void token_deinit()
{
    struct token_pattern* tps = get_token_pattern_array();
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &tps[i];
        if(tp->re){
            regex_free(tp->re);
            tp->re = 0;
        }
    }
}

void tok_clean(struct token *tok)
{
    if(tok->token_type == TOKEN_LITERAL_STRING && tok->str_val){
        FREE((void*)tok->str_val);
        tok->str_val = 0;
    }
}

struct token_patterns get_token_patterns()
{
    struct token_pattern* tps = get_token_pattern_array();
    struct token_patterns tpss = { tps, TERMINAL_COUNT };
    return tpss;
}

const char *get_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < OP_TOTAL);
    return get_token_pattern_by_opcode(opcode)->name;
}

symbol get_terminal_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode)
{
    if (token_type == TOKEN_OP){
        assert(opcode >= 0 && opcode < OP_TOTAL);
        return get_token_pattern_by_opcode(opcode)->symbol_name;
    }else{
        return get_token_pattern_by_token_type(token_type)->symbol_name;
    }
}

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode)
{
    struct token_pattern* tps = get_token_pattern_array();
    assert(opcode > 0 && opcode < TERMINAL_COUNT);
    return &tps[(int)TOKEN_OP + (int)opcode];
}

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type)
{
    struct token_pattern* tps = get_token_pattern_array();
    assert(token_type >= 0 && token_type <= TOKEN_OP);
    return &tps[(int)token_type];
}

u16 get_terminal_token_index(enum token_type token_type, enum op_code opcode)
{
    if(token_type == TOKEN_OP)
        return (u16)TOKEN_OP + (u16)opcode;
    else 
        return (u16)token_type;
}

bool is_terminal(u16 symbol_index)
{
    return symbol_index < TERMINAL_COUNT;
}

bool is_relational_op(enum op_code opcode)
{
    return opcode >= OP_LT && opcode <= OP_NE;
}

enum op_code get_op_code_from_assign_op(enum op_code assign_op)
{
    switch (assign_op)
    {
    case OP_MUL_ASSN:
        return OP_STAR;
    case OP_DIV_ASSN:
        return OP_DIVISION;
    case OP_MOD_ASSN:
        return OP_MODULUS;
    case OP_ADD_ASSN:
        return OP_PLUS;
    case OP_SUB_ASSN:
        return OP_MINUS;
    case OP_LEFT_ASSN:
        return OP_BSL;
    case OP_RIGHT_ASSN:
        return OP_RIGHT_ASSN;
    case OP_AND_ASSN:
        return OP_BAND;
    case OP_XOR_ASSN:
        return OP_BITEXOR;
    case OP_OR_ASSN:
        return OP_BITOR;
    default:
        assert(false);
        return OP_NULL;
    }
}

