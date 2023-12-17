/*
 * parsing table definitions for parser
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 */
#ifndef __MLANG_M_PARSING_TABLE_H__
#define __MLANG_M_PARSING_TABLE_H__

#include "clib/typedef.h"
#include "parser/node_type.h"
#include "pgen/lalr_parser_generator.h"
#include "parser/m_parsing_table.h"

#ifdef __cplusplus
extern "C" {
#endif

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
