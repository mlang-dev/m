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
#include "clib/win/libfmemopen.h"
#include "error/error.h"


char escape_2_char[128];

void indent_level_stack_init(struct indent_level_stack *stack)
{
    stack->stack_top = 0;
    stack->leading_spaces[stack->stack_top++] = 0;
}

int indent_level_stack_match(struct indent_level_stack *stack, u32 leading_spaces)
{
    //return value: 
    //  0: match the indent level on the top of the stack, no indent level is change
    //  1: bigger than the top indent level, adding one indent level
    //  -n: number of INDENT reduced when current leading spaces is lower than the one on the top
    //      of the stack and previous indent level is matched.
    //  0xFF: INVALID_INDENTS: lower than indent level on top of the stack, but doesn't match any 
    //          existing indent level
    u32 top_spaces = stack->leading_spaces[stack->stack_top - 1];
    if (leading_spaces == top_spaces){
        return 0;
    }else if(leading_spaces > top_spaces){
        stack->leading_spaces[stack->stack_top++] = leading_spaces;
        return 1;
    }else{
        int i;
        for(i = stack->stack_top - 2; i >= 0; i--){
            if(stack->leading_spaces[i] == leading_spaces)
                break;
        }
        if (i < 0){
            return INVALID_INDENTS;
        }
        i -= stack->stack_top - 1; /*pop number of indent levels*/
        stack->stack_top += i;
        return i;
    }
    return 0;
}

struct lexer *lexer_new(FILE *file, const char *filename, const char *code, size_t code_size)
{
    if (code && code_size > CODE_BUFF_SIZE){
        printf("only %d bytes of m code is allowed to parse, but here is the size of the code: %zu.\n",  CODE_BUFF_SIZE, code_size);
        exit(-1);
    }
    for(int i = 0; i < 128; i++){
        escape_2_char[i] = i;
    }
    escape_2_char['n'] = '\n';
    escape_2_char['t'] = '\t';
    escape_2_char['v'] = '\v';
    escape_2_char['b'] = '\b';
    escape_2_char['r'] = '\r';
    escape_2_char['f'] = '\f';
    escape_2_char['a'] = '\a';
    escape_2_char['\\'] = '\\';
    escape_2_char['?'] = '\?';
    escape_2_char['\''] = '\'';
    escape_2_char['\"'] = '\"';
    escape_2_char['0'] = '\0';

    struct lexer *lexer;
    MALLOC(lexer, sizeof(*lexer));
    lexer->buff_base = 0;
    lexer->line = 1;
    lexer->pos = 0;
    lexer->col = 1;
    lexer->file = file;
    lexer->filename = filename;
    memset(lexer->buff, '\0', CODE_BUFF_SIZE + 1);
    lexer->tok.token_type = TOKEN_EOF;
    if(lexer->file){
        //fmemopen in MacOs will open empty string as null file handle
        fgets(lexer->buff, CODE_BUFF_SIZE + 1, lexer->file);
    }else{
        memcpy(lexer->buff, code, code_size);
    }

    //init indent level stack
    indent_level_stack_init(&lexer->indent_stack);
    lexer->pending_dedents = 0;
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
    return lexer;
}

struct lexer *lexer_new_for_string(const char *text)
{
    size_t size = strlen(text);
    FILE *file = fmemopen((void *)text, size, "r");
    return lexer_new(file, "", 0, 0);
}

struct lexer *lexer_new_with_string(const char *text)
{
    return lexer_new(0, 0, text, strlen(text));
}

void lexer_free(struct lexer *lexer)
{
    FREE(lexer);
}

void _move_ahead(struct lexer *lexer)
{
    switch(lexer->buff[lexer->pos]){
        case '\n':
            lexer->line++;
            lexer->col = 1;
            break;
        default:
            lexer->col++;
            break;
    }
    lexer->pos++;
    if(!lexer->buff[lexer->pos]){
        //fgets
        if(lexer->file && fgets(lexer->buff, CODE_BUFF_SIZE + 1, lexer->file)){
            lexer->buff_base += lexer->pos;
            lexer->pos = 0;
        }else{
            //end of file, we've done
        }
    }
}

void _move_ahead_n(struct lexer *lexer, int n)
{
    for(int i=0; i < n; i++) _move_ahead(lexer);
}

void _scan_until(struct lexer *lexer, char until)
{
    do{
        _move_ahead(lexer);
        if (!lexer->buff[lexer->pos] || (lexer->buff[lexer->pos] == until && lexer->buff[lexer->pos-1] != '\\')){
            break;
        }
    } while (true);
}

bool _scan_until_no_digit(struct lexer *lexer)
{
    const char *p = &lexer->buff[lexer->pos];
    bool has_dot = false;
    do {
        if(!has_dot && *p == '.') has_dot = true;
        p++;
        _move_ahead(lexer); 
    } while (isdigit(*p) || *p == '.');
    return has_dot;
}

u32 _scan_until_no_space(struct lexer *lexer)
{
    u32 spaces = 0;
    while (isspace(lexer->buff[lexer->pos]) && lexer->buff[lexer->pos] != '\n') {
        _move_ahead(lexer);
        spaces++;
    }
    return spaces;
}

void _scan_until_no_id(struct lexer *lexer)
{
    char ch;
    do
    {
        _move_ahead(lexer);
        ch = lexer->buff[lexer->pos];
    }
    while (ch == '_' || isalpha(ch) || isdigit(ch));
}

void _mark_token(struct lexer *lexer, enum token_type token_type, enum op_code opcode)
{
    lexer->tok.loc.start = lexer->buff_base + lexer->pos;
    lexer->tok.loc.line = lexer->line;
    lexer->tok.loc.col = lexer->col;
    lexer->tok.token_type = token_type;
    lexer->tok.opcode = opcode;
}

void _mark_regex_tok(struct lexer *lexer)
{
    struct token *tok = &lexer->tok;
    char ch = lexer->buff[lexer->pos];
    struct pattern_matches *pm = &lexer->char_matches[(int)ch];
    if(!pm->pattern_match_count){
        report_error(lexer, EC_UNRECOGNIZED_CHAR, tok->loc);
        return;
    }
    int max_matched = 0;
    struct token_pattern *used_tp = 0;
    for(int i = 0; i < pm->pattern_match_count; i++){
        int matched = 0;
        struct token_pattern *tp = 0;
        tp = pm->patterns[i];
        matched = regex_match(tp->re, &lexer->buff[lexer->pos], 0);
        if(matched > max_matched){
            max_matched = matched;
            used_tp = tp;
        }
    }
    if(max_matched){
        _mark_token(lexer, used_tp->token_type, used_tp->opcode);
        _move_ahead_n(lexer, max_matched);
        if(used_tp->token_type == TOKEN_IDENT)
            tok->symbol_val = to_symbol2(&lexer->buff[tok->loc.start - lexer->buff_base], max_matched);
        else if(used_tp->token_type == TOKEN_INT){
            int base = 10;
            char hex = lexer->buff[tok->loc.start - lexer->buff_base + 1];
            if (hex == 'x' || hex == 'X'){
                base = 16;
            }
            tok->int_val = (int)strtoul(&lexer->buff[tok->loc.start - lexer->buff_base], 0, base);
        } else if (used_tp->token_type == TOKEN_DOUBLE)
            tok->double_val = strtod(&lexer->buff[tok->loc.start - lexer->buff_base], 0);
    }else{
        report_error(lexer, EC_UNRECOGNIZED_TOKEN, tok->loc);
    }
}

const char * _copy_string_strip_escape(const char *text, size_t len, size_t *out_size)
{
    char *dst = malloc(len+1);
    size_t i = 0;
    size_t j = 0;
    for(i=0; i<len; i++){
        if(text[i] == '\\'){
            i++;
            dst[j++] = escape_2_char[(int)text[i]];
        }else{
            dst[j++] = text[i];
        }
    }
    *out_size = j;
    return dst;
}

bool _is_valid_char(struct lexer *lexer)
{
    return lexer->buff_base + lexer->pos - lexer->tok.loc.start != 3 && lexer->buff_base + lexer->pos - lexer->tok.loc.start != 4;
}

struct token *get_tok(struct lexer *lexer)
{
    struct token *tok = &lexer->tok;
    if (lexer->pending_dedents < 0){
        assert(tok->token_type == TOKEN_DEDENT);
        lexer->pending_dedents ++;
        return tok;
    }
    u32 spaces;
    char ch;
    do {
        spaces = _scan_until_no_space(lexer);
        ch = lexer->buff[lexer->pos];
        if (ch=='\n' && (tok->token_type == TOKEN_EOF || tok->token_type == TOKEN_NEWLINE)){
            //skip the empty line
            _move_ahead(lexer);
        }else{
            break;
        }
    } while(true);
    if (ch == '\0'){
        int match = indent_level_stack_match(&lexer->indent_stack, 0);
        if(match < 0){
            //dedent
            tok->token_type = TOKEN_DEDENT;
            lexer->pending_dedents = match + 1;
            return tok;
        }
    } else if ((tok->token_type == TOKEN_EOF || tok->token_type == TOKEN_NEWLINE) && ch != '\n') {
        //if last token is new line and this is not an empty line
        //then we're going to check indent/dedent levels
        int match = indent_level_stack_match(&lexer->indent_stack, spaces);
        if(match == 1){
            tok->token_type = TOKEN_INDENT;
            return tok;
        }
        else if(match == INVALID_INDENTS){
            tok->token_type = TOKEN_ERROR;
            report_error(lexer, EC_INCONSISTENT_INDENT_LEVEL, tok->loc);
            goto mark_end;
        }
        else if(match < 0){
            //dedent
            tok->token_type = TOKEN_DEDENT;
            lexer->pending_dedents = match + 1;
            return tok;
        }
    }

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
        _move_ahead(lexer); // skip the new line
        break;
    case '#': //comments
        _scan_until(lexer, '\n');
        get_tok(lexer);
        break;
    case '\'':
        _mark_token(lexer, TOKEN_CHAR, 0);
        _scan_until(lexer, '\'');
        if(lexer->buff[lexer->pos] != '\''){
            tok->token_type = TOKEN_ERROR;
            report_error(lexer, EC_CHAR_MISS_END_QUOTE, tok->loc);
            goto mark_end;
        }
        _move_ahead(lexer); //skip the single quote
        if(_is_valid_char(lexer)){
            tok->token_type = TOKEN_ERROR;
            report_error(lexer, EC_CHAR_LEN_TOO_LONG, tok->loc);
            goto mark_end;
        }
        if(lexer->buff[tok->loc.start - lexer->buff_base + 1] == '\\'){
            lexer->tok.int_val = escape_2_char[(int)lexer->buff[tok->loc.start - lexer->buff_base + 2]];
        }else{
            lexer->tok.int_val = lexer->buff[tok->loc.start - lexer->buff_base + 1];
        }
        break;
    case '"':
        _mark_token(lexer, TOKEN_STRING, 0);
        _scan_until(lexer, '"');
        if(lexer->buff[lexer->pos] != '"'){
            tok->token_type = TOKEN_ERROR;
            report_error(lexer, EC_STR_MISS_END_QUOTE, tok->loc);
            goto mark_end;
        }
        _move_ahead(lexer); // skip the double quote
        size_t char_len = 0;
        const char *code = _copy_string_strip_escape(&lexer->buff[tok->loc.start - lexer->buff_base + 1], lexer->buff_base + lexer->pos - tok->loc.start - 2, &char_len);
        lexer->tok.str_val = string_new2(code, char_len);
        free((void*)code);
        break;
    }
mark_end:
    tok->loc.end = lexer->buff_base + lexer->pos;
    return tok;
}
