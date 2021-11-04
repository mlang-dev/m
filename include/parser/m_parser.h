/*
 * parser.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for an earley parser
 */
#ifndef __MLANG_EARLEY_PARSER_H__
#define __MLANG_EARLEY_PARSER_H__

#include "lexer/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct earley_parser {
    struct tokenizer *tokenizer;
};

struct earley_parser *earley_parser_new(const char *grammar);
void earley_parser_free(struct earley_parser *parser);

#ifdef __cplusplus
}
#endif

#endif
