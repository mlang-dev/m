/*
 * lexer.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m lexer
 */
#ifndef __MLANG_LEXER_H__
#define __MLANG_LEXER_H__

#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/token.h"
#include "lexer/recognizer.h"
#include "lexer/source_location.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pattern_matches {
    struct token_pattern *patterns[8];
    int pattern_match_count;
};

#define CODE_BUFF_SIZE 4096

struct lexer {
    FILE *file;
    const char *filename;
    char buff[CODE_BUFF_SIZE + 1];
    struct token tok;
    int pos;  //current text position in the buffer
    int buff_base;
    int line;
    int col;
    struct pattern_matches char_matches[128];
};

struct lexer *lexer_new(FILE *file, const char *filename, const char *text);

struct lexer *lexer_new_for_string(const char *text);

void lexer_free(struct lexer *lexer);

struct token *get_tok(struct lexer *lexer);

#ifdef __cplusplus
}
#endif

#endif
