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
#include <ctype.h>

struct parser *parser_new(const char *grammar_text)
{
    struct grammar *grammar = grammar_new(grammar_text);
    struct parser *parser;
    MALLOC(parser, sizeof(*parser));
    
    parser->grammar = grammar;
    parser->ID_TOKEN = to_symbol2("ID", 2);
    parser->NUM_TOKEN = to_symbol2("NUM", 3);
    parser->STRING_TOKEN = to_symbol2("STRING", 6);
    parser->CHAR_TOKEN = to_symbol2("CHAR", 4);
    return parser;
}


void parser_set_text(struct parser *parser, const char *text)
{
    parser->text = text;
    parser->text_pos = 0;
    parser->row_no = 1;
    parser->col_no = 1;
}

void parser_free(struct parser *parser)
{
    grammar_free(parser->grammar);
    free(parser);
}


//write non-null-terminated string: fwrite(string, sizeof(char), length, stdout);

void _move_ahead(struct parser *parser)
{
    if (!parser->text[parser->text_pos]) return;
    if (parser->text[parser->text_pos] == '\n') {
        parser->row_no++;
        parser->col_no = 1;
    } else {
        parser->col_no++;
    }
    parser->text_pos++;
}

void _scan_until(struct parser *parser, char until)
{
    do{
        _move_ahead(parser);

    } while (parser->text[parser->text_pos] != until);
}

void _scan_until_no_digit(struct parser *parser)
{
    const char *p = &parser->text[parser->text_pos];
    do {
        p++;
        _move_ahead(parser);
    } while (isdigit(*p) || *p == '.');
}

void _scan_until_no_space(struct parser *parser)
{
    do {
        _move_ahead(parser);
    } while (isspace(parser->text[parser->text_pos]));
}

void _scan_until_no_id(struct parser *parser)
{
    char ch;
    do
    {
        _move_ahead(parser);
        ch = parser->text[parser->text_pos];
    }
    while (ch == '_' || isalpha(ch) || isdigit(ch));
}

void _mark_token(struct parser *parser, struct tok *tok, symbol tok_type)
{
    tok->start_pos = parser->text_pos;
    tok->row_no = parser->row_no;
    tok->col_no = parser->col_no;
    tok->tok_type = tok_type;
}

void get_tok(struct parser *parser, struct tok *tok)
{
    tok->tok_type = 0;
    char ch = parser->text[parser->text_pos];
    switch (ch)
    {
    default:
        if(isspace(ch)){
            _scan_until_no_space(parser);
            get_tok(parser, tok);
        }
        else if(isdigit(ch) || 
            (ch == '.' && isdigit(parser->text[parser->text_pos + 1])) /*.123*/){
            _mark_token(parser, tok, parser->NUM_TOKEN);
            _scan_until_no_digit(parser);
        }
        else if(isalpha(ch) || ch == '_'){
            _mark_token(parser, tok, parser->ID_TOKEN);
            _scan_until_no_id(parser);
        }
        break;
    case '\0':
        break;
    case '#':
        _scan_until(parser, '\n');
        get_tok(parser, tok);
        break;
    case '\'':
        _mark_token(parser, tok, parser->CHAR_TOKEN);
        _scan_until(parser, '\'');
        _move_ahead(parser); //skip the single quote
        break;
    case '"':
        _mark_token(parser, tok, parser->STRING_TOKEN);
        _scan_until(parser, '"');
        _move_ahead(parser); // skip the double quote
        break;
    }
    if(tok->tok_type){
        tok->end_pos = parser->text_pos;
    }
}

bool parser_parse(struct parser *parser, const char *text)
{
    struct tok tok;
    parser_set_text(parser, text);
    get_tok(parser, &tok);
    // parser->text = text;
    // parser->text_pos = 0;
    // struct tok tok = get_tok(parser);
    // while (tok.tok_type){
    //     tok = get_tok(parser);
    // } 
    return true;
}