/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic lexer/tokenizer for m, it is designed to be codegened 
 */

#include <ctype.h>
#include "lexer/lexer.h"
#include <assert.h>
#include <string.h>
#include "clib/regex.h"
#include "clib/win/libfmemopen.h"
#include "app/error.h"


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
        return 0;
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
    lexer->last_token_type = TOKEN_EOF;
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
        struct pattern_matches *pm = &lexer->char_matches[i];
        pm->pattern_match_count = 0;
        for(size_t j = 0; j < tps.pattern_count; j++){
            size_t matched_len;
            if(tps.patterns[j].re){
                regex_match(tps.patterns[j].re, test, &matched_len);
                if(!matched_len) continue;
                assert(pm->pattern_match_count < MAX_PATTERNS_PER_CHAR);
                pm->patterns[pm->pattern_match_count++] = &tps.patterns[j];

                //switch ident token to be the last one
                if (pm->pattern_match_count > 1){
                    struct token_pattern *ident_tp = pm->patterns[pm->pattern_match_count-2];
                    if(ident_tp->token_type == TOKEN_IDENT){
                        pm->patterns[pm->pattern_match_count-2] = pm->patterns[pm->pattern_match_count-1];
                        pm->patterns[pm->pattern_match_count-1] = ident_tp;
                    }
                }
            }
        }
    }
    array_init(&lexer->open_closes, sizeof(enum token_type));
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
    tok_clean(&lexer->tok);
    if(lexer->file){
        fclose(lexer->file);
    }
    array_deinit(&lexer->open_closes);
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

void _scan_untils(struct lexer *lexer, char until0, char until1)
{
    do{
        _move_ahead(lexer);
        if (!lexer->buff[lexer->pos] || (lexer->buff[lexer->pos] == until0 && lexer->buff[lexer->pos-1] != '\\' && lexer->buff[lexer->pos+1] == until1)){
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

bool _is_in_group(struct lexer *lexer)
{
    return array_size(&lexer->open_closes);
}

u32 _scan_until_no_space(struct lexer *lexer)
{
    u32 spaces = 0;
    while (isspace(lexer->buff[lexer->pos]) && (lexer->buff[lexer->pos] != '\n' || _is_in_group(lexer))) {
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
        else if(used_tp->token_type == TOKEN_LITERAL_INT){
            int base = 10;
            char hex = lexer->buff[tok->loc.start - lexer->buff_base + 1];
            if (hex == 'x' || hex == 'X'){
                base = 16;
            }
            tok->int_val = (int)strtoul(&lexer->buff[tok->loc.start - lexer->buff_base], 0, base);
        } else if (used_tp->token_type == TOKEN_LITERAL_FLOAT)
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
    dst[j] = 0;
    return dst;
}

bool _is_valid_char(struct lexer *lexer)
{
    int len = lexer->buff_base + lexer->pos - lexer->tok.loc.start;
    if (len == 3) return true;
    if ((len == 4) && lexer->buff[lexer->tok.loc.start + 1] == '\\') return true;
    return false;
}

u32 _skip_empty_lines(struct lexer *lexer, enum token_type last_token_type)
{
    u32 spaces;
    char ch;
    do {
        spaces = _scan_until_no_space(lexer);
        ch = lexer->buff[lexer->pos];
        if (ch=='\n' && (last_token_type == TOKEN_EOF || last_token_type == TOKEN_NEWLINE)){
            //skip the empty line
            _move_ahead(lexer);
        }else{
            break;
        }
    } while(true);
    return spaces;
}

struct token *get_tok(struct lexer *lexer)
{
    struct token *tok = 0;
    do{
        tok = get_tok_with_comments(lexer);
    }while(is_comment_token(tok->token_type));
    return tok;
}

struct token *get_tok_with_comments(struct lexer *lexer)
{
    struct token *tok = &lexer->tok;
    if (lexer->pending_dedents < 0){
        assert(tok->token_type == TOKEN_DEDENT);
        lexer->pending_dedents ++;
        return tok;
    }
    _mark_token(lexer, TOKEN_NULL, OP_NULL);
    u32 spaces = _skip_empty_lines(lexer, lexer->last_token_type);
    char ch = lexer->buff[lexer->pos];
    if (ch == '\0'){
        int match = indent_level_stack_match(&lexer->indent_stack, 0);
        if(match < 0){
            //dedent
            tok->token_type = TOKEN_DEDENT;
            lexer->pending_dedents = match + 1;
            goto mark_end;
        }
    } else if ((lexer->last_token_type == TOKEN_EOF || lexer->last_token_type == TOKEN_NEWLINE) && ch != '\n') {
        //if last token is new line and this is not an empty line
        //then we're going to check indent/dedent levels
        int match = indent_level_stack_match(&lexer->indent_stack, spaces);
        if(match == 1){
            tok->token_type = TOKEN_INDENT;
            goto mark_end;
        }
        else if(match == INVALID_INDENTS){
            tok->token_type = TOKEN_NULL;
            tok->loc.col = lexer->col; //we need location of end of token here
            report_error(lexer, EC_INCONSISTENT_INDENT_LEVEL, tok->loc);
            goto mark_end;
        }
        else if(match < 0){
            //dedent
            tok->token_type = TOKEN_DEDENT;
            lexer->pending_dedents = match + 1;
            goto mark_end;
        }
    }
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
    case '\'':
        _mark_token(lexer, TOKEN_LITERAL_CHAR, 0);
        _scan_until(lexer, '\'');
        if(lexer->buff[lexer->pos] != '\''){
            tok->token_type = TOKEN_NULL;
            report_error(lexer, EC_CHAR_MISS_END_QUOTE, tok->loc);
            goto mark_end;
        }
        _move_ahead(lexer); //skip the single quote
        if(!_is_valid_char(lexer)){
            tok->token_type = TOKEN_NULL;
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
        _mark_token(lexer, TOKEN_LITERAL_STRING, 0);
        _scan_until(lexer, '"');
        if(lexer->buff[lexer->pos] != '"'){
            tok->token_type = TOKEN_NULL;
            report_error(lexer, EC_STR_MISS_END_QUOTE, tok->loc);
            goto mark_end;
        }
        _move_ahead(lexer); // skip the double quote
        size_t char_len = 0;
        const char *code = _copy_string_strip_escape(&lexer->buff[tok->loc.start - lexer->buff_base + 1], lexer->buff_base + lexer->pos - tok->loc.start - 2, &char_len);
        lexer->tok.str_val = code;
        break;
    }
    //
    if(is_linecomment_token(tok->token_type)){
        _scan_until(lexer, '\n');
    } else if (tok->token_type == TOKEN_BLOCKCOMMENT){
        _scan_untils(lexer, '*', '/');
        if(lexer->buff[lexer->pos] == '*')
            _move_ahead(lexer);
        if(lexer->buff[lexer->pos] == '/')
            _move_ahead(lexer);
    }
mark_end:
    tok->loc.end = lexer->buff_base + lexer->pos;
    if(is_open_group(tok->token_type)){
        array_push(&lexer->open_closes, &tok->token_type);
    }
    else if(is_close_group(tok->token_type)){
        if(!array_size(&lexer->open_closes)){
            //TODO: print not matched open symbol error
            assert(false);
        }
        enum token_type open = *(enum token_type*)array_back(&lexer->open_closes);
        if(!is_open_group(open)){
            //TODO: print error message
            assert(false);
        }
        else if(!is_match_open(open, tok->token_type)){
            //TODO print error message 2
            assert(false);
        }
        array_pop(&lexer->open_closes);
    }
    if(!is_comment_token(tok->token_type))
        lexer->last_token_type = tok->token_type;
    return tok;
}

const char *highlight(struct lexer *lexer, const char *text)
{
    string str;
    string_init_chars(&str, "");
    struct token *tok = get_tok_with_comments(lexer);
    int last_end = 0;
    while(tok->token_type != TOKEN_NULL && tok->token_type != TOKEN_EOF){
        if (tok->token_type == TOKEN_DEDENT) {
            tok = get_tok_with_comments(lexer);
            continue;
        }
        if(tok->loc.start - last_end > 0){
            string_add_chars2(&str, &text[last_end], tok->loc.start - last_end);
        }
        const char *token_content = &text[tok->loc.start];
        size_t token_content_len = tok->loc.end-tok->loc.start;
        struct token_pattern *tp = get_token_pattern_by_token_type(tok->token_type);
        if(tp->style_class_name){
            string tok_str;
            char span_class[128];
            sprintf(span_class, "<span class=\"token %s\">", tp->style_class_name);
            string_init_chars(&tok_str, span_class);
            
            if(is_comment_token(tp->token_type)){
                //encode 
                string *dst = string_replace(token_content, token_content_len, '&', "&amp;");
                string *dst1 = string_replace(string_get(dst), string_size(dst), '<', "&lt;");
                string *dst2 = string_replace(string_get(dst1), string_size(dst1), '>', "&gt;");
                string_add_chars2(&tok_str, string_get(dst2), string_size(dst2));
                string_free(dst);
                string_free(dst1);
                string_free(dst2);
            } else {
                string_add_chars2(&tok_str, token_content, token_content_len);
            }
            string_add_chars(&tok_str, "</span>");
            string_add(&str, &tok_str);
            string_deinit(&tok_str);
        }else{
            string_add_chars2(&str, token_content, token_content_len);
        }
        last_end = tok->loc.end;
        tok = get_tok_with_comments(lexer);
    }
    int len = strlen(text);
    if(len-last_end > 0){
        string_add_chars2(&str, &text[last_end], len - last_end);
    }
    return string_get_owned(&str);
}
