/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic lexer/tokenizer for m, it is designed to be codegened 
 */

#include <ctype.h>
#include "lexer/lexer.h"
#include <assert.h>

symbol IDENT_TOKEN = 0;
symbol NUM_TOKEN = 0;
symbol STRING_TOKEN = 0;
symbol CHAR_TOKEN = 0;
symbol INDENT_TOKEN = 0;
symbol DEDENT_TOKEN = 0;
symbol DEF_TOKEN = 0;
symbol LCBRACKET_TOKEN = 0;
symbol RCBRACKET_TOKEN = 0;
symbol LBRACKET_TOKEN = 0;
symbol RBRACKET_TOKEN = 0;
symbol VBAR_TOKEN = 0;

enum token_type get_token_type(symbol tok_type_name)
{
    if(tok_type_name == IDENT_TOKEN)
        return TOKEN_IDENT;
    else if(tok_type_name == NUM_TOKEN)
        return TOKEN_INT;
    else if(tok_type_name == STRING_TOKEN)
        return TOKEN_STRING;
    else if(tok_type_name == CHAR_TOKEN)
        return TOKEN_CHAR;
    else if(tok_type_name == IDENT_TOKEN)
        return TOKEN_IDENT;
    else if(tok_type_name == DEDENT_TOKEN)
        return TOKEN_DEDENT;
    else if(tok_type_name == DEF_TOKEN)
        return TOKEN_ASSIGN;
    else if(tok_type_name == LCBRACKET_TOKEN)
        return TOKEN_LCBRACKET;
    else if(tok_type_name == RCBRACKET_TOKEN)
        return TOKEN_RCBRACKET;
    else if(tok_type_name == LBRACKET_TOKEN)
        return TOKEN_LBRACKET;
    else if(tok_type_name == RBRACKET_TOKEN)
        return TOKEN_RBRACKET;
    else if(tok_type_name == VBAR_TOKEN)
        return OP_BOR;
    else{
        return TOKEN_OP;
    }
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
    DEF_TOKEN = to_symbol2("=", 1);
    LCBRACKET_TOKEN = to_symbol2_0("{");
    RCBRACKET_TOKEN = to_symbol2_0("}");
    LBRACKET_TOKEN = to_symbol2_0("[");
    RBRACKET_TOKEN = to_symbol2_0("]");
    VBAR_TOKEN = to_symbol2_0("|");
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

void _mark_token(struct lexer *lexer, struct tok *tok, symbol tok_type_name)
{
    tok->loc.start = lexer->pos;
    tok->loc.line = lexer->line;
    tok->loc.col = lexer->col;
    tok->tok_type_name = tok_type_name;
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
    if(tok->tok_type_name){
        tok->loc.end = lexer->pos;
        tok->tok_type = get_token_type(tok->tok_type_name);
    }
}
