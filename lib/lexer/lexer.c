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
        for(int j = tps.pattern_count - 1; j >= 0; j--){
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
    do {
        _move_ahead(lexer);
    } while (isspace(lexer->text[lexer->pos]));
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

struct token *get_tok(struct lexer *lexer)
{
    struct token *tok = &lexer->tok;
    tok->token_type = TOKEN_NULL;
    char ch = lexer->text[lexer->pos];
    switch (ch)
    {
    default:
        if(isspace(ch)){
            _scan_until_no_space(lexer);
            get_tok(lexer);
        }
        else if(isdigit(ch) || 
            (ch == '.' && isdigit(lexer->text[lexer->pos + 1])) /*.123*/){
            _mark_token(lexer, ch == '.' ? TOKEN_FLOAT : TOKEN_INT, 0);
            bool has_dot = _scan_until_no_digit(lexer);
            if(has_dot) {
                tok->token_type = TOKEN_FLOAT;
                tok->double_val = strtod(&lexer->text[lexer->tok.loc.start], 0);
            }else{
                tok->int_val = (int)strtol(&lexer->text[lexer->tok.loc.start], 0, 10);
            }
        }
        else if(isalpha(ch) || ch == '_'){
            _mark_token(lexer, TOKEN_IDENT, 0);
            _scan_until_no_id(lexer);
            lexer->tok.symbol_val = to_symbol2(&lexer->text[tok->loc.start], lexer->pos - tok->loc.start);
        }
        else{
            symbol symbol = to_symbol2(&ch, 1);
            struct token_pattern *tp = get_token_pattern_by_symbol(symbol);
            _mark_token(lexer, tp->token_type, tp->opcode);
            _move_ahead(lexer);
        }
        break;
    case '\0':
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
