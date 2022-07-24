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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stack_item{
    u16 state_index;
    struct ast_node *ast;
};

struct lalr_parser{
    //parser implementation
    // state stack
    struct stack_item stack[MAX_STATES];
    u32 stack_top;

    struct lalr_parser_generator *pg;
};

struct lalr_parser *lalr_parser_new(const char *grammar);
void lalr_parser_free(struct lalr_parser *parser);
struct ast_node *parse_text(struct lalr_parser *parser, const char *text);

#ifdef __cplusplus
}
#endif

#endif
