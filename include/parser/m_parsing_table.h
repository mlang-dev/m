/*
 * parsing table definitions for parser
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 */
#ifndef __MLANG_M_PARSING_TABLE_H__
#define __MLANG_M_PARSING_TABLE_H__

#include "clib/typedef.h"
#include "parser/grammar.h"
#include "parser/node_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "parser/m_parsing_table_def.h"

#define MAX_SYMBOLS_RULE 11

struct rule_action {
    enum node_type node_type;
    u8 item_index[MAX_SYMBOLS_RULE]; // 0: is the first exp item value parsed at right side of grammar rule
    u8 item_index_count;
};

enum action_code {
    E, // indicator to do error recoverage
    S, // shift the state
    R, // reduce with production rule
    A, // complete the parsing successfully
    G, // goto for state i, and X nonterm
};

struct parser_action {
    enum action_code code;
    union {
        u16 state_index; // next state index if action is shift
        u16 rule_index; // index of production rule if action is reduce
    };
};


// converted grammer rule with integer
struct parse_rule {
    const char *rule_string;
    u16 lhs; // non terminal symbol index
    u16 rhs[MAX_SYMBOLS_RULE]; // right hand side of production rule
    u8 symbol_count; // right side of
    struct rule_action action;
};

#define MAX_KERNEL_ITEMS   11
struct parse_rule_state{
    u8 rule;    //rule index
    u8 dot;     //dot position
    const char *item_string;
};

struct parse_state_items{
    u32 item_count;
    struct parse_rule_state items[MAX_KERNEL_ITEMS];
};

#ifndef M_PARSING_INITIALIZER
// grammar rules converted to int index
extern struct parse_rule m_parsing_rules[PARSING_RULE_COUNT];
typedef struct parse_rule parsing_rules[PARSING_RULE_COUNT];

extern struct parser_action m_parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT];
typedef struct parser_action parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT];

/*for debugger references*/
extern const char *m_parsing_symbols[PARSING_SYMBOL_COUNT];
typedef const char *parsing_symbols[PARSING_SYMBOL_COUNT];

extern struct parse_state_items m_parsing_states[PARSING_STATE_COUNT];
typedef struct parse_state_items parsing_states[PARSING_STATE_COUNT];

#endif

#ifdef __cplusplus
}
#endif

#endif
