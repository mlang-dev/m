/*
 * lalr_parser_generator.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for an LALR parser generator:
 *      LALR parser generator convert grammar into parsing table for the LALR parser to consume.
 */
#ifndef __MLANG_LALR_PARSER_GENERATOR_H__
#define __MLANG_LALR_PARSER_GENERATOR_H__

#include "parser/grammar.h"
#include "parser/m_parsing_table.h"
#include "parser/parser_def.h"
#include "lexer/pgen_token.h"
#include "clib/symbol.h"
#include "clib/hashtable.h"
#include "clib/stack.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

link_list2(index_list, index_list_entry, u16)

struct parse_item {
    u8 rule;    //rule index
    u8 dot;     //dot position
    struct index_list lookaheads; /*look ahead list for reduction*/
};

link_list2(parse_item_list, parse_item_list_entry, struct parse_item)

struct parse_state{
    u8 kernel_item_count;
    struct parse_item_list items;  //list of parse_items
};

struct rule_symbol_data{
    bool is_nullable; //whether the nonterm symbol is nullable
    struct index_list first_list;//first set
    struct index_list follow_list;//follow set
    struct index_list rule_list; //rules indexed by symbol
};

struct lalr_parser_generator{
    /*input for generator*/
    //parser generator states
    struct grammar *g;
    struct parse_state parse_states[MAX_STATES];
    u16 parse_state_count;

    //grammar rule symbol data: store isnullable, first set and follow set per symbol
    struct rule_symbol_data symbol_data[MAX_GRAMMAR_SYMBOLS];

    /*generator output*/
    // action table for terminal symbols, tokens
    struct parser_action parsing_table[MAX_STATES][MAX_GRAMMAR_SYMBOLS];
    // grammar rules converted to int index
    struct parse_rule parsing_rules[MAX_RULES];
    u16 rule_count;

    // grammar augmented rules
    struct parse_rule augmented_rules[MAX_AUGMENTED_RULES];
    u16 augmented_rule_count;

    u16 total_symbol_count; /*total symbol including terminal and nonterm*/
    /*argumented LALR garmmar rule with nonterminal symbol with subscript of from state and to state
    (ref: Bermudez and Logothetis Theorem)
    augmented_symbol_map: is the hashtable data structure
        the key is u64 int: high u32 is symbol index, in low 32bits high u16 is from_state, low u16 is to_state
        the data is u16 symbol index
    */
    struct hashtable augmented_symbol_map;
};

struct lalr_parser_generator *lalr_parser_generator_new(const char *grammar);
void lalr_parser_generator_free(struct lalr_parser_generator *parser);

#ifdef __cplusplus
}
#endif

#endif
