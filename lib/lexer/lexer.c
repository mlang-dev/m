/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic lexer/tokenizer for m, it is designed to be codegened 
 */

#include <ctype.h>
#include "lexer/lexer.h"
#include <assert.h>
#include "lexer/token.h"
#include "clib/regex.h"

void lexer_init(struct lexer *lexer, const char *text)
{
    lexer->text = text;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->col = 1;
    token_init();
    //register pattern matcher for each character
    char test[2]; test[1] = 0;
    struct token_patterns tps = get_token_patterns();
    for(int i=0; i < 128; i++){
        test[0] = (char)i;
        lexer->char_matches[i].pattern_match_count = 0;
        for(size_t j = 0; j < tps.pattern_count; j++){
            size_t matched_len;
            if(tps.patterns[j].re){
                regex_match(tps.patterns[j].re, test, &matched_len);
                if(!matched_len) continue;
                if(lexer->char_matches[i].pattern_match_count == 8){
                    printf("maximum number of pattern matcher is 8.\n");
                    exit(-1);
                }
                lexer->char_matches[i].patterns[lexer->char_matches[i].pattern_match_count++] = &tps.patterns[j];
            }
        }
    }
}

void lexer_deinit()
{
    token_deinit();
}

void _move_ahead(struct lexer *lexer)
{
    if (!lexer->text[lexer->pos]) return;
    if (lexer->text[lexer->pos] == '\n') {
        lexer->line++;
        lexer->col = 1;
    } else {
        lexer->col++;
    }
    lexer->pos++;
}

void _move_ahead_n(struct lexer *lexer, int n)
{
    for(int i=0; i < n; i++) _move_ahead(lexer);
}

void _scan_until(struct lexer *lexer, char until)
{
    do{
        _move_ahead(lexer);

    } while (lexer->text[lexer->pos] != until);
}

bool _scan_until_no_digit(struct lexer *lexer)
{
    const char *p = &lexer->text[lexer->pos];
    bool has_dot = false;
    do {
        if(!has_dot && *p == '.') has_dot = true;
        p++;
        _move_ahead(lexer);
    } while (isdigit(*p) || *p == '.');
    return has_dot;
}

void _scan_until_no_space(struct lexer *lexer)
{
    while (isspace(lexer->text[lexer->pos]) && lexer->text[lexer->pos] != '\n') {
        _move_ahead(lexer);
    }
}

void _scan_until_no_id(struct lexer *lexer)
{
    char ch;
    do
    {
        _move_ahead(lexer);
        ch = lexer->text[lexer->pos];
    }
    while (ch == '_' || isalpha(ch) || isdigit(ch));
}

void _mark_token(struct lexer *lexer, enum token_type token_type, enum op_code opcode)
{
    lexer->tok.loc.start = lexer->pos;
    lexer->tok.loc.line = lexer->line;
    lexer->tok.loc.col = lexer->col;
    lexer->tok.token_type = token_type;
    lexer->tok.opcode = opcode;
}

void _mark_regex_tok(struct lexer *lexer)
{
    struct token *tok = &lexer->tok;
    char ch = lexer->text[lexer->pos];
    struct pattern_matches *pm = &lexer->char_matches[(int)ch];
    if(!pm->pattern_match_count){
        printf("invalid char : %c\n", ch);
        exit(-1);
    }
    int max_matched = 0;
    struct token_pattern *used_tp = 0;
    for(int i = 0; i < pm->pattern_match_count; i++){
        int matched = 0;
        struct token_pattern *tp = 0;
        tp = pm->patterns[i];
        matched = regex_match(tp->re, &lexer->text[lexer->pos], 0);
        if(matched > max_matched){
            max_matched = matched;
            used_tp = tp;
        }
    }
    if(max_matched){
        _mark_token(lexer, used_tp->token_type, used_tp->opcode);
        _move_ahead_n(lexer, max_matched);
        if(used_tp->token_type == TOKEN_IDENT)
            tok->symbol_val = to_symbol2(&lexer->text[tok->loc.start], max_matched);
        else if(used_tp->token_type == TOKEN_INT)
            tok->int_val = (int)strtol(&lexer->text[lexer->tok.loc.start], 0, 10);
        else if(used_tp->token_type == TOKEN_FLOAT)
            tok->double_val = strtod(&lexer->text[lexer->tok.loc.start], 0);
    }else{
        printf("no valid token found for %c\n", ch);
        exit(-1);
    }
}

struct token *get_tok(struct lexer *lexer)
{
    struct token *tok = &lexer->tok;
    _scan_until_no_space(lexer);
    char ch = lexer->text[lexer->pos];
    tok->token_type = TOKEN_EOF;
    switch (ch)
    {
    default:
        _mark_regex_tok(lexer);        
        break;
    case '\0':
        _mark_token(lexer, TOKEN_EOF, OP_NULL);
        break;
    case '\n':
        _mark_token(lexer, TOKEN_NEWLINE, OP_NULL);
        _move_ahead(lexer); // skip the double quote
        break;
    case '#': //comments
        _scan_until(lexer, '\n');
        get_tok(lexer);
        break;
    case '\'':
        _mark_token(lexer, TOKEN_CHAR, 0);
        _scan_until(lexer, '\'');
        _move_ahead(lexer); //skip the single quote
        if(lexer->pos - lexer->tok.loc.start != 3){
            printf("character is supposed to be 1 char long.\n");
            exit(-1);
            return 0;
        }
        lexer->tok.char_val = lexer->text[lexer->tok.loc.start + 1];
        break;
    case '"':
        _mark_token(lexer, TOKEN_STRING, 0);
        _scan_until(lexer, '"');
        _move_ahead(lexer); // skip the double quote
        lexer->tok.str_val = string_new2(&lexer->text[lexer->tok.loc.start + 1], lexer->pos - lexer->tok.loc.start - 2);
        break;
    }
    if(tok->token_type){
        tok->loc.end = lexer->pos;
    }
    return tok;
}
