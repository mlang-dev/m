/*
 * lr_parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for an LR parser
 */
#ifndef __MLANG_LALR_PARSER_H__
#define __MLANG_LALR_PARSER_H__

#include "parser/grammar.h"
#include "lexer/lexer.h"
#include "clib/symbol.h"
#include "parser/ast.h"
#include "clib/stack.h"
#include "lexer/token.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum action_code{
    ACTION_ERROR, //indicator to do error recoverage 
    ACTION_SHIFT, //shift the state
    ACTION_REDUCE, //reduce with production rule
    ACTION_ACCEPT, //complete the parsing successfully
    ACTION_GOTO, //goto for state i, and X nonterm
};

struct parser_action{
    enum action_code code;
    union {
        u16 state_index; //if action is shift
        u16 rule_index;  //index of production rule if action is reduce
    };
};

#define MAX_STATES 512
#define MAX_RULES 128
#define MAX_SYMBOLS_RULE 7

struct grule{
    u8 lhs;   //non terminal symbol index
    u8 rhs[MAX_SYMBOLS_RULE]; //right hand side of production rule
    u8 symbol_count;
};

link_list(index_list, index_list_entry, u8)

struct parse_item {
    u8 rule; //rule index
    u8 dot;  //dot position
    u8 lookahead;//
};

link_list(parse_item_list, parse_item_list_entry, struct parse_item)

struct parse_state{
    struct parse_item_list items;
    u16 item_count;
};

struct rule_symbol_data{
    bool is_nullable;
    struct index_list first_list;//first set
    struct index_list follow_list;//follow set

    struct index_list rule_list; //rule indexs
};

struct lr_parser{
    //state stack
    u16 stack[MAX_STATES];
    u16 stack_top;

    //action table for terminal symbols, tokens
    struct parser_action parsing_table[MAX_STATES][MAX_GRAMMAR_SYMBOLS];

    struct parse_state parse_states[MAX_STATES];
    u16 parse_state_count;

    //grammar rule symbol data: store isnullable, first set and follow set
    struct rule_symbol_data symbol_data[MAX_GRAMMAR_SYMBOLS];

    //grammar rules
    struct grule rules[MAX_RULES];
    u16 rule_count;
    struct grammar *g;
};

struct lr_parser *lr_parser_new(const char *grammar);
void lr_parser_free(struct lr_parser *parser);
struct ast_node *parse_text(struct lr_parser *parser, const char *text);

#ifdef __cplusplus
}
#endif

#endif