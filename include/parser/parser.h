/*
 * parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for the mlang parser
 */
#ifndef __MLANG_PARSER_H__
#define __MLANG_PARSER_H__

#include "clib/hashtable.h"
#include "parser/parser_def.h"
#include "parser/m_parsing_table.h"
#include "sema/type.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stack_item{
    u16 state_index;
    struct ast_node *ast;
};

struct parser{
    /*parser states*/
    struct type_context *tc;
    //  parser implementation
    //  state stack
    struct stack_item stack[MAX_STATES];
    u32 stack_top;

    // two dimentional array:
    //      row is parse state, and columns are grammar symbols
    //      content of the cell is the action to drive the parser 
    parsing_table *pt;
    // symbols of grammar rules are converted to int index as parsing rules
    parsing_rules *pr;

    //parsing symbol descriptions
    parsing_symbols *psd;

    //parsing state descriptions
    parsing_states *pstd;
};

void parser_free(struct parser *parser);
struct ast_node *parse_code(struct parser *parser, const char *text);
struct ast_node *parse_file(struct parser *parser, const char *file_name);
struct ast_node *parse_repl_code(struct parser *parser, void (*fun)(void *, struct ast_node *), void *jit);
struct parser *parser_new();

#ifdef __cplusplus
}
#endif

#endif
