/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic lexer/tokenizer for m, it is designed to be codegened 
 */

#include <ctype.h>
#include "lexer/lexer.h"
#include <assert.h>

void lexer_init(struct lexer *lexer, const char *text)
{
    lexer->text = text;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->col = 1;
    token_init();
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

void _mark_token(struct lexer *lexer, struct tok *tok, enum token_type tok_type, enum op_code opcode)
{
    tok->loc.start = lexer->pos;
    tok->loc.line = lexer->line;
    tok->loc.col = lexer->col;
    tok->tok_type_name = get_symbol_by_token_opcode(tok_type, opcode);
    tok->tok_type = tok_type;
    tok->opcode = opcode;
}

void get_tok(struct lexer *lexer, struct tok *tok)
{
    tok->tok_type_name = 0;
    tok->tok_type = TOKEN_NULL;
    char ch = lexer->text[lexer->pos];
    switch (ch)
    {
    default:
        if(isspace(ch)){
            _scan_until_no_space(lexer);
            get_tok(lexer, tok);
        }
        else if(isdigit(ch) || 
            (ch == '.' && isdigit(lexer->text[lexer->pos + 1])) /*.123*/){
            _mark_token(lexer, tok, ch == '.' ? TOKEN_FLOAT : TOKEN_INT, 0);
            bool has_dot = _scan_until_no_digit(lexer);
            if(has_dot) tok->tok_type = TOKEN_FLOAT;
        }
        else if(isalpha(ch) || ch == '_'){
            _mark_token(lexer, tok, TOKEN_IDENT, 0);
            _scan_until_no_id(lexer);
        }
        else{
            symbol symbol = to_symbol2(&ch, 1);
            struct token_pattern *tp = get_token_pattern_by_symbol(symbol);
            _mark_token(lexer, tok, tp->token_type, tp->opcode);
            _move_ahead(lexer);
        }
        break;
    case '\0':
        break;
    case '#':
        _scan_until(lexer, '\n');
        get_tok(lexer, tok);
        break;
    case '\'':
        _mark_token(lexer, tok, TOKEN_CHAR, 0);
        _scan_until(lexer, '\'');
        _move_ahead(lexer); //skip the single quote
        break;
    case '"':
        _mark_token(lexer, tok, TOKEN_STRING, 0);
        _scan_until(lexer, '"');
        _move_ahead(lexer); // skip the double quote
        break;
    }
    if(tok->tok_type){
        tok->loc.end = lexer->pos;
    }
}
