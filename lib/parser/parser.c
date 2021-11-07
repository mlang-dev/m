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
    parser->text = 0;
    parser->text_pos = 0;
    parser->row_no = 1;
    parser->col_no = 1;
    parser->ID_TOKEN = to_symbol2("ID", 2);
    parser->NUM_TOKEN = to_symbol2("NUM", 3);
    parser->STRING_TOKEN = to_symbol2("STRING", 6);
    parser->CHAR_TOKEN = to_symbol2("CHAR", 4);
    return parser;
}

void parser_free(struct parser *parser)
{
    grammar_free(parser->grammar);
    free(parser);
}

struct tok{
    int start_pos;
    int len;
    int row_no; //1-based
    int col_no; //1-based
    symbol tok_type; //ID, NUM, STRING keywords like 'if', 'for' etc   
};

//write non-null-terminated string: fwrite(string, sizeof(char), length, stdout);
void get_tok(struct parser *parser, struct tok *tok)
{
    symbol s = 0;
    const char *p0 = &parser->text[parser->text_pos];
    const char *p = p0;
    while (*p) {
        switch (*p) {
        default:
            p++;
            break;
        case '#':
            while (*p++ != '\n')
                ;
            break;
        case '\r':
        case '\n':
        case ' ':
            if (p > p0) {
                s = to_symbol2(p0, p-p0);
                tok->tok_type = s;
                tok->len = p-p0;
                tok->start_pos = parser->text_pos;
                tok->row_no = parser->row_no;
                tok->col_no = parser->col_no;
                
                parser->text_pos += tok->len;
            }
            while (*p == '\r' || *p == '\n' || *p == ' ')
                p++;
            if(s){
                return;
            }
            break;
        case '\'':
            assert(p0==p);
            while (*++p != '\'');
            // term_char_no = 0;
            // expr_add_symbol(expr, to_symbol(term), ATOM_EXACT_MATCH);
            tok->
            p++;
            p0 = p;
            break;
        case '"':
            // assert(term_char_no == 0);
            // while (*++p != ']') {
            //     term[term_char_no++] = *p;
            // }
            // term[term_char_no] = 0;
            // term_char_no = 0;
            // expr_add_symbol(expr, to_symbol(term), ATOM_IN_MATCH);
            p++;
            break;
        case '{':
        case '}':
            p++;
            break;
        }
    }
}

bool parser_parse(struct parser *parser, const char *text)
{
    struct tok tok;
    get_tok(parser, &tok);
    // parser->text = text;
    // parser->text_pos = 0;
    // struct tok tok = get_tok(parser);
    // while (tok.tok_type){
    //     tok = get_tok(parser);
    // } 
    return true;
}