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
symbol IDENT_TOKEN = 0;
symbol NUM_TOKEN = 0;
symbol STRING_TOKEN = 0;
symbol CHAR_TOKEN = 0;
symbol INDENT_TOKEN = 0;
symbol DEDENT_TOKEN = 0;

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
    tok->loc.line = lexer->line;
    tok->loc.col = lexer->col;
    tok->tok_type = tok_type;
}

void lexer_init(struct lexer *lexer, const char *text)
{
    lexer->text = text;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->col = 1;

    IDENT_TOKEN = to_symbol2("IDENT", 5);
    NUM_TOKEN = to_symbol2("NUM", 3);
    STRING_TOKEN = to_symbol2("STRING", 6);
    CHAR_TOKEN = to_symbol2("CHAR", 4);
    INDENT_TOKEN = to_symbol2("INDENT", 6);
    DEDENT_TOKEN = to_symbol2("DEDENT", 6);
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
            _mark_token(lexer, tok, NUM_TOKEN);
            _scan_until_no_digit(lexer);
        }
        else if(isalpha(ch) || ch == '_'){
            _mark_token(lexer, tok, IDENT_TOKEN);
            _scan_until_no_id(lexer);
        }
        else{
            _mark_token(lexer, tok, to_symbol2(&ch, 1));
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
        _mark_token(lexer, tok, CHAR_TOKEN);
        _scan_until(lexer, '\'');
        _move_ahead(lexer); //skip the single quote
        break;
    case '"':
        _mark_token(lexer, tok, STRING_TOKEN);
        _scan_until(lexer, '"');
        _move_ahead(lexer); // skip the double quote
        break;
    }
    if(tok->tok_type){
        tok->loc.end = lexer->pos;
    }
}
