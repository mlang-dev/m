/*
 * parser.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for an earley parser
 */
#ifndef __MLANG_PARSER_H__
#define __MLANG_PARSER_H__

#include "parser/grammar.h"

#ifdef __cplusplus
extern "C" {
#endif

struct parser {
    struct grammar *grammar;
};

struct parser *parser_new(const char *grammar);
void parser_free(struct parser *parser);

#ifdef __cplusplus
}
#endif

#endif
