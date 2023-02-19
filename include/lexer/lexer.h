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
#include "clib/array.h"
#include "clib/util.h"
#include "lexer/terminal.h"
#include "lexer/source_location.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PATTERNS_PER_CHAR   16
struct pattern_matches {
    struct token_pattern *patterns[MAX_PATTERNS_PER_CHAR];
    int pattern_match_count;
};

#define CODE_BUFF_SIZE 40960

#define MAX_INDENTS     254
#define INVALID_INDENTS 255

struct indent_level_stack{
    u32 leading_spaces[MAX_INDENTS];
    u8 stack_top;
};

struct lexer {
    FILE *file;
    const char *filename;
    char buff[CODE_BUFF_SIZE + 1];
    struct indent_level_stack indent_stack;
    struct token tok;
    enum token_type last_token_type; //last effective token type, excluding comments token
    int pos;  //current text position in the buffer
    int buff_base;
    int line;
    int col;
    struct pattern_matches char_matches[128];
    int pending_dedents;
    struct array open_closes; //group match 
};

struct lexer *lexer_new(FILE *file, const char *filename, const char *code, size_t code_size);
struct lexer *lexer_new_for_string(const char *text);
struct lexer *lexer_new_with_string(const char *text);
const char *highlight(struct lexer *lexer, const char *text);
void lexer_free(struct lexer *lexer);

struct token *get_tok(struct lexer *lexer);
struct token *get_tok_with_comments(struct lexer *lexer);

#ifdef __cplusplus
}
#endif

#endif
