/*
 * lalr_parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for an LALR parser
 */
#ifndef __MLANG_LALR_PARSER_H__
#define __MLANG_LALR_PARSER_H__

#include "parser/lalr_parser_generator.h"
#include "parser/m_parsing_table.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stack_item{
    u16 state_index;
    struct ast_node *ast;
};

struct lalr_parser{
    //  parser implementation
    //  state stack
    struct stack_item stack[MAX_STATES];
    u32 stack_top;

    // two dimentional array:
    //      row is parse state, and columns are grammar symbols
    //      content of the cell is action to drive the parser 
    parsing_table *parsing_table;
    // symbols of grammar rules are converted to int index as parsing rules
    parsing_rules *parsing_rules;
};

struct lalr_parser *lalr_parser_new(parsing_table *parsing_table, parsing_rules *parsing_rules);
void lalr_parser_free(struct lalr_parser *parser);
struct ast_node *parse_code(struct lalr_parser *parser, const char *text);
struct lalr_parser *parser_new();

#ifdef __cplusplus
}
#endif

#endif
