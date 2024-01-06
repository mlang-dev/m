/*
 * parser_def.h
 *
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for parser definition including macros, data structures and function prototypes
 * This file will be copied to the parser directory by parser generator pgen
 */
#ifndef __MLANG_PGEN_PARSER_DEF_H__
#define __MLANG_PGEN_PARSER_DEF_H__

#include "clib/typedef.h"
#define MAX_KERNEL_ITEMS   11

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

#define MAX_SYMBOLS_RULE 16 

struct rule_action {
    u8 node_type;
    u8 item_index[MAX_SYMBOLS_RULE]; // 0: is the first exp item value parsed at right side of grammar rule
    u8 item_index_count;
};
// converted grammer rule with integer
struct parse_rule {
    const char *rule_string;
    u16 lhs; // non terminal symbol index
    u16 rhs[MAX_SYMBOLS_RULE]; // right hand side of production rule
    u8 symbol_count; // right side of
    struct rule_action action;
};

#define MAX_STATES 512

#endif
