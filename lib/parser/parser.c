/*
 * parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement a generic parser, taking a EBNF grammar text and parse text 
 * into ast according to the grammar
 */
#include "parser/parser.h"
#include "clib/array.h"
#include "clib/util.h"
#include "parser/grammar.h"

struct parser *parser_new(const char *grammar_text)
{
    struct grammar *grammar = grammar_new(grammar_text);
    struct parser *parser;
    MALLOC(parser, sizeof(*parser));
    parser->grammar = grammar;
    return parser;
}

void parser_free(struct parser *parser)
{
    grammar_free(parser->grammar);
    free(parser);
}

bool parser_parse(struct parser *parser, const char *text)
{
    return true;
}