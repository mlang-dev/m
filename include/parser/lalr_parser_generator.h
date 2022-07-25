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
#include "lexer/lexer.h"
#include "clib/symbol.h"
#include "clib/hashtable.h"
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
        u16 state_index; //next state index if action is shift
        u16 rule_index;  //index of production rule if action is reduce
    };
};

#define MAX_STATES 512
#define MAX_RULES 128
#define MAX_AUGMENTED_RULES 1024
#define MAX_SYMBOLS_RULE 7

//converted grammer rule with integer
struct parse_rule{
    u16 lhs;   //non terminal symbol index
    u16 rhs[MAX_SYMBOLS_RULE]; //right hand side of production rule
    u8 symbol_count; //right side of 
    struct semantic_action action;
};

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
