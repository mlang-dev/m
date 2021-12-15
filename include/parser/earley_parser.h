/*
 * parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for a generic parser
 */
#ifndef __MLANG_PARSER_H__
#define __MLANG_PARSER_H__

#include "parser/grammar.h"
#include "lexer/lexer.h"
#include "clib/symbol.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif


struct parser {
    struct grammar *grammar;
    struct lexer lexer;
};

struct expr_parse{
    struct expr *expr; //rule expression applied in current position of tokens
    size_t parsed;  //number of expr in the rule parsed, or pointing to the next rule expr to be parse for each expression in the rule.
    struct rule *rule; //rule applied in current position of tokens
    size_t start_state_index; //0-based index of parse state
};

struct complete_parse{
    size_t end_state_index;
    struct expr_parse *ep;
};

// for each position of tokens we maintain a partial parse state: n tokens will have n parse states
struct parse_state{
    size_t state_index;
    struct tok tok;   
    struct array expr_parses; //number of parses for current position of tokens
    //struct array complete_parses;
    struct hashtable complete_parses;
};

struct parse_states {
    struct array states; // number of parse state for current position of tokens
};

struct parser *parser_new(const char *grammar);
void parser_free(struct parser *parser);
struct ast_node *parse(struct parser *parser, const char *text);

#ifdef __cplusplus
}
#endif

#endif
