/*
 * parsing table definitions for parser
 * 
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 */
#ifndef __MLANG_M_PARSING_TABLE_H__
#define __MLANG_M_PARSING_TABLE_H__

#include "clib/typedef.h"
#include "parser/grammar.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "parser/m_parsing_table_def.h"

enum action_code {
    ACTION_ERROR, // indicator to do error recoverage
    ACTION_SHIFT, // shift the state
    ACTION_REDUCE, // reduce with production rule
    ACTION_ACCEPT, // complete the parsing successfully
    ACTION_GOTO, // goto for state i, and X nonterm
};

struct parser_action {
    enum action_code code;
    union {
        u16 state_index; // next state index if action is shift
        u16 rule_index; // index of production rule if action is reduce
    };
};

#define MAX_SYMBOLS_RULE 7

// converted grammer rule with integer
struct parse_rule {
    u16 lhs; // non terminal symbol index
    u16 rhs[MAX_SYMBOLS_RULE]; // right hand side of production rule
    u8 symbol_count; // right side of
    struct semantic_action action;
};


#ifndef M_PARSING_INITIALIZER
// grammar rules converted to int index
extern struct parse_rule m_parsing_rules[PARSING_RULE_COUNT];
typedef struct parse_rule parsing_rules[PARSING_RULE_COUNT];

extern struct parser_action m_parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT];
typedef struct parser_action parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT];
#endif

#ifdef __cplusplus
}
#endif

#endif
