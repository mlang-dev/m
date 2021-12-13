/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m lexer, m tokenizer
 */
#include <assert.h>
#include <ctype.h>
#include <memory.h>

#include "clib/hashtable.h"
#include "clib/string.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include "clib/win/libfmemopen.h"
#include "parser/m_grammar.h"


#define CUR_CHAR(tokenizer) tokenizer->curr_char[0]

void log_error(struct tokenizer *tokenizer, const char *msg)
{
    char full_msg[512];
    sprintf_s(full_msg, sizeof(full_msg), "%s:%d:%d: %s", tokenizer->filename, tokenizer->tok_loc.line, tokenizer->tok_loc.col, msg);
    log_info(ERROR, full_msg);
}

char get_char(struct tokenizer *tokenizer)
{
    char last_char;
    if (tokenizer->peek >= 0) {
        last_char = tokenizer->peek;
        tokenizer->peek = -1;
    } else {
        last_char = (char)getc(tokenizer->file);
    }
    if (is_new_line(last_char)) {
        tokenizer->loc.line++;
        tokenizer->loc.col = 0;
    } else
        tokenizer->loc.col++;
    return last_char;
}

char peek_char(struct tokenizer *tokenizer)
{
    assert(tokenizer->peek == -1); // only 1 peek is allowed.
    tokenizer->peek = (char)getc(tokenizer->file);
    return tokenizer->peek;
}

void _lexer_init(struct tokenizer *tokenizer, struct keyword_token *keyword_tokens, int keyword_count)
{
    hashtable_init_with_value_size(&tokenizer->keyword_2_tokens, sizeof(enum token_type), 0);
    // for (size_t i = 0; i < ARRAY_SIZE(keyword_symbols); ++i) {
    //     symbol keyword = to_symbol(keyword_symbols[i].keyword);
    //     hashtable_set_p(&keyword_2_tokens, keyword, &keyword_symbols[i].token);
    // }
    rcg_init(&tokenizer->rcg_states);
    //char ch;
    //struct rcg_state *ks;
    //struct rcg_state *next_ks;
    for (int i = 0; i < keyword_count; ++i) {
        rcg_add_exact_match(&tokenizer->rcg_states, keyword_tokens[i].keyword, keyword_tokens[i].token_type);
    }
}

void _lexer_deinit(struct tokenizer *tokenizer)
{
    hashtable_deinit(&tokenizer->keyword_2_tokens);
    rcg_deinit(&tokenizer->rcg_states);
}

struct tokenizer *create_tokenizer(FILE *file, const char *filename, struct keyword_token *keyword_tokens, int keyword_count)
{
    struct tokenizer *tokenizer;
    MALLOC(tokenizer, sizeof(*tokenizer));
    _lexer_init(tokenizer, keyword_tokens, keyword_count);
    struct source_location loc = { 1, 0, 0, 0 };
    tokenizer->loc = loc;
    tokenizer->curr_char[0] = ' ';
    tokenizer->curr_char[1] = '\0';
    tokenizer->peek = -1;
    tokenizer->file = file;
    tokenizer->filename = filename;
    string_init(&tokenizer->str_val);
    return tokenizer;
}

struct tokenizer *create_tokenizer_for_string(const char *content, struct keyword_token *keyword_tokens, int keyword_count)
{
    FILE *file = fmemopen((void *)content, strlen(content), "r");
    struct tokenizer *tokenizer = create_tokenizer(file, "", keyword_tokens, keyword_count);
    return tokenizer;
}

void destroy_tokenizer(struct tokenizer *tokenizer)
{
    if(tokenizer->file) //memory file for string, could be closed earlier for each string parsing.
        fclose(tokenizer->file);
    string_deinit(&tokenizer->str_val);
    _lexer_deinit(tokenizer);
    FREE(tokenizer);
}

void _collect_all_dots(struct tokenizer *tokenizer, string *symbol)
{
    while (tokenizer->curr_char[0] == '.') {
        string_add_chars(symbol, tokenizer->curr_char);
        tokenizer->curr_char[0] = get_char(tokenizer);
    }
}

void _collect_all_digits(struct tokenizer *tokenizer, string *str)
{
    while (isdigit(tokenizer->curr_char[0])) {
        string_add_chars(str, tokenizer->curr_char);
        tokenizer->curr_char[0] = get_char(tokenizer);
    }
}

struct token *_tokenize_dot(struct tokenizer *tokenizer)
{
    assert(tokenizer->curr_char[0] == '.');
    string str;
    string_init_chars(&str, tokenizer->curr_char);
    tokenizer->curr_char[0] = get_char(tokenizer);
    if (tokenizer->curr_char[0] == '.') {
        _collect_all_dots(tokenizer, &str);
        tokenizer->cur_token.symbol_val = to_symbol(string_get(&str));
        tokenizer->cur_token.token_type = TOKEN_SYMBOL;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        return &tokenizer->cur_token;
    } else if (isdigit(tokenizer->curr_char[0])) {
        _collect_all_digits(tokenizer, &str);
        tokenizer->cur_token.double_val = strtod(string_get(&str), 0);
        tokenizer->cur_token.token_type = TOKEN_FLOAT;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        return &tokenizer->cur_token;
    } else {
        tokenizer->cur_token.token_type = TOKEN_SYMBOL;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        tokenizer->cur_token.symbol_val = to_symbol(".");
        return &tokenizer->cur_token;
    }
}

struct token *_tokenize_number_literal(struct tokenizer *tokenizer)
{
    string num_str;
    string_init(&num_str);
    bool has_dot = false;
    while (isdigit(tokenizer->curr_char[0])) {
        string_add_chars(&num_str, tokenizer->curr_char);
        tokenizer->curr_char[0] = get_char(tokenizer);
        if (tokenizer->curr_char[0] == '.') {
            // TODO: this is second dot, might need to produce diagnosis
            if (has_dot)
                break;
            int nextc = peek_char(tokenizer);
            if (nextc == '.') // two dots in a row
                break;
            has_dot = true;
            // take the dot
            string_add_chars(&num_str, tokenizer->curr_char);
            tokenizer->curr_char[0] = get_char(tokenizer);
        }
    }
    if (has_dot) {
        tokenizer->cur_token.double_val = strtod(string_get(&num_str), 0);
        tokenizer->cur_token.token_type = TOKEN_FLOAT;
    } else {
        tokenizer->cur_token.int_val = atoi(string_get(&num_str));
        tokenizer->cur_token.token_type = TOKEN_INT;
    }

    tokenizer->cur_token.loc = tokenizer->tok_loc;
    return &tokenizer->cur_token;
}

struct token *_tokenize_id_keyword(struct tokenizer *tokenizer)
{
    string_copy_chars(&tokenizer->str_val, tokenizer->curr_char);
    struct rcg_state *ks = tokenizer->rcg_states.states[(int)tokenizer->curr_char[0]];
    struct rcg_state *ks_next = 0;
    while (true) {
        char ch = tokenizer->curr_char[0] = get_char(tokenizer);
        ks_next = rcg_find_next_state(ks, ch);
        if (ks && ks->accepted_token_or_opcode && !ks->identifiable && !ks_next)
            break;
        else if (isalnum(ch) || ch == '_' || ch == '.' || ks_next) {
            string_add_chars(&tokenizer->str_val, tokenizer->curr_char);
            ks = ks_next;
        } else {
            break;
        }
    }
    if (!ks){
        tokenizer->cur_token.token_type = TOKEN_IDENT;
    }else if(
        ks->accepted_token_or_opcode == TOKEN_TRUE || 
        ks->accepted_token_or_opcode == TOKEN_FALSE){
        tokenizer->cur_token.token_type = ks->accepted_token_or_opcode;
    }else{
        tokenizer->cur_token.token_type = ks->accepted_token_or_opcode ? TOKEN_SYMBOL : TOKEN_IDENT;
    }
    //tokenizer->cur_token.token_type = (ks && ks->accepted_token_or_opcode) ? TOKEN_SYMBOL : TOKEN_IDENT;
    tokenizer->cur_token.symbol_val = to_symbol(string_get(&tokenizer->str_val));
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    if(ks&&ks->accepted_token_or_opcode > TOKEN_TOTAL){
        tokenizer->cur_token.opcode = ks->accepted_token_or_opcode;
    }else{
        tokenizer->cur_token.opcode = 0;
    }
        
    return &tokenizer->cur_token;
}

struct token *_tokenize_char_token(struct tokenizer *tokenizer, enum token_type token_type)
{
    string_copy_chars(&tokenizer->str_val, tokenizer->curr_char);
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    tokenizer->cur_token.token_type = token_type;
    tokenizer->cur_token.symbol_val = to_symbol(tokenizer->curr_char);
    return &tokenizer->cur_token;
}

struct token *_tokenize_char_literal(struct tokenizer *tokenizer)
{
    char temp = get_char(tokenizer);
    if (temp == '\'') {
        log_error(tokenizer, "empty char is not allowed in character literal");
        return 0;
    }
    tokenizer->cur_token.char_val = temp;
    if (get_char(tokenizer) != '\'') {
        log_error(tokenizer, "only one char allowed in character literal");
        return 0;
    }
    tokenizer->cur_token.token_type = TOKEN_CHAR;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    tokenizer->curr_char[0] = get_char(tokenizer);
    return &tokenizer->cur_token;
}

struct token *_tokenize_string_literal(struct tokenizer *tokenizer)
{
    string_copy_chars(&tokenizer->str_val, "");
    while (true) {
        char this_char = get_char(tokenizer);
        /*escape sequence*/
        if (tokenizer->curr_char[0] == '\\') {
            if (this_char == 'n')
                this_char = 0x0a;
        }
        tokenizer->curr_char[0] = this_char;
        if (this_char == '\\') {
            /*starting escape sequence*/
            continue;
        }
        if (tokenizer->curr_char[0] == '"')
            break;
        string_add_chars(&tokenizer->str_val, tokenizer->curr_char);
    }
    tokenizer->cur_token.token_type = TOKEN_STRING;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    tokenizer->cur_token.str_val = &tokenizer->str_val;
    tokenizer->curr_char[0] = get_char(tokenizer);
    return &tokenizer->cur_token;
}

void _skip_to_line_end(struct tokenizer *tokenizer)
{
    do
        tokenizer->curr_char[0] = get_char(tokenizer);
    while (tokenizer->curr_char[0] != EOF && !is_new_line(tokenizer->curr_char[0]));
}

struct token *get_token(struct tokenizer *tokenizer)
{
    // skip spaces
    while (isspace(tokenizer->curr_char[0])) {
        if (is_new_line(tokenizer->curr_char[0]))
            break;
        tokenizer->curr_char[0] = get_char(tokenizer);
    }
    tokenizer->tok_loc = tokenizer->loc;
    if (tokenizer->curr_char[0] == EOF)
        return _tokenize_char_token(tokenizer, TOKEN_EOF);
    else if (is_new_line(tokenizer->curr_char[0])) {
        _tokenize_char_token(tokenizer, TOKEN_NEWLINE);
        tokenizer->curr_char[0] = ' '; // replaced with empty space
        return &tokenizer->cur_token;
    } else if (tokenizer->curr_char[0] == '\'') {
        return _tokenize_char_literal(tokenizer);
    } else if (tokenizer->curr_char[0] == '"') {
        return _tokenize_string_literal(tokenizer);
    } else if (tokenizer->curr_char[0] == '.') {
        return _tokenize_dot(tokenizer);
    } else if (isdigit(tokenizer->curr_char[0])) {
        return _tokenize_number_literal(tokenizer);
    } else if (isalpha(tokenizer->curr_char[0]) || tokenizer->curr_char[0] == '_' || tokenizer->rcg_states.states[(int)tokenizer->curr_char[0]]) {
        return _tokenize_id_keyword(tokenizer);
    } else if (tokenizer->curr_char[0] == '#') {
        // skip comments
        _skip_to_line_end(tokenizer);
        if (tokenizer->curr_char[0] != EOF)
            return get_token(tokenizer);
        else
            return _tokenize_char_token(tokenizer, TOKEN_EOF);
    }else{
        assert(false);
    }
    return 0;
}
