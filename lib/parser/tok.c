/*
 * tok.c
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic lexer implementation, it handles char/string/number literal and identifer
 */
#include <ctype.h>

#include "parser/tok.h"

//write non-null-terminated string: fwrite(string, sizeof(char), length, stdout);

void _move_ahead(struct lexer *lexer)
{
    if (!lexer->text[lexer->pos]) return;
    if (lexer->text[lexer->pos] == '\n') {
        lexer->row++;
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

void _scan_until_no_digit(struct lexer *lexer)
{
    const char *p = &lexer->text[lexer->pos];
    do {
        p++;
        _move_ahead(lexer);
    } while (isdigit(*p) || *p == '.');
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

void _mark_token(struct lexer *lexer, struct tok *tok, symbol tok_type)
{
    tok->loc.start = lexer->pos;
    tok->loc.row = lexer->row;
    tok->loc.col = lexer->col;
    tok->tok_type = tok_type;
}

void lexer_init(struct lexer *lexer, const char *text)
{
    lexer->text = text;
    lexer->pos = 0;
    lexer->row = 1;
    lexer->col = 1;

    lexer->IDENT_TOKEN = to_symbol2("IDENT", 5);
    lexer->NUM_TOKEN = to_symbol2("NUM", 3);
    lexer->STRING_TOKEN = to_symbol2("STRING", 6);
    lexer->CHAR_TOKEN = to_symbol2("CHAR", 4);
}

void get_tok(struct lexer *lexer, struct tok *tok)
{
    tok->tok_type = 0;
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
            _mark_token(lexer, tok, lexer->NUM_TOKEN);
            _scan_until_no_digit(lexer);
        }
        else if(isalpha(ch) || ch == '_'){
            _mark_token(lexer, tok, lexer->IDENT_TOKEN);
            _scan_until_no_id(lexer);
        }
        else{
            _mark_token(lexer, tok, 0);
            tok->char_type = ch;
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
        _mark_token(lexer, tok, lexer->CHAR_TOKEN);
        _scan_until(lexer, '\'');
        _move_ahead(lexer); //skip the single quote
        break;
    case '"':
        _mark_token(lexer, tok, lexer->STRING_TOKEN);
        _scan_until(lexer, '"');
        _move_ahead(lexer); // skip the double quote
        break;
    }
    if(tok->tok_type){
        tok->loc.end = lexer->pos;
    }
}
