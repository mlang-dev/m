/*
 * lalr_parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for an LALR parser
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
#define MAX_

struct grule{
    u8 lhs;   //non terminal symbol index
    u8 rhs[MAX_SYMBOLS_RULE]; //right hand side of production rule
    u8 symbol_count;
};

struct parse_item {
    u8 rule; //rule index
    u8 dot;  //dot position
};

link_list(parse_item_list, parse_item_list_entry, struct parse_item)

struct parse_state{
    struct parse_item_list items;
};

struct lalr_parser{
    //state stack
    u16 stack[MAX_STATES];
    u16 stack_top;

    //action table for terminal symbols, tokens
    struct parser_action parsing_table[MAX_STATES][128];

    //grammar rules
    struct grule rules[MAX_RULES];
    u16 rule_count;
    struct grammar *g;
};

struct lalr_parser *lalr_parser_new(const char *grammar);
void lalr_parser_free(struct lalr_parser *parser);
struct ast_node *parse_text(struct lalr_parser *parser, const char *text);

#ifdef __cplusplus
}
#endif

#endif
