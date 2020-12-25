/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m lexer, m tokenizer
 */
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>

#include "clib/string.h"
#include "clib/util.h"
#include "lexer.h"

const char* token_type_strings[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

#define CUR_CHAR(tokenizer) tokenizer->curr_char[0]

char op_chars[] = {
    '!', '%', '&', '*', '+', '-', '.', '/', '<', '=', '>', '?', '@', '^', '|'
};

bool is_op_char(char op)
{
    for (size_t i = 0; i < ARRAY_SIZE(op_chars); i++) {
        if (op_chars[i] == op)
            return true;
    }
    return false;
}

struct keyword_token {
    char keyword[16];
    enum token_type token;
};

#define TRUE "true"
#define FALSE "false"
static struct keyword_token keyword_tokens[] = {
    { "import", TOKEN_IMPORT },
    { "extern", TOKEN_EXTERN },
    { "type", TOKEN_TYPE },
    { "if", TOKEN_IF },
    { "else", TOKEN_ELSE },
    { "then", TOKEN_THEN },
    { "in", TOKEN_IN },
    { "for", TOKEN_FOR },
    { "unary", TOKEN_UNARY },
    { "binary", TOKEN_BINARY },
    { "||", TOKEN_OR },
    { "&&", TOKEN_AND },
    { "..", TOKEN_RANGE },
    { "...", TOKEN_VARIADIC},
    { TRUE, TOKEN_TRUE },
    { FALSE, TOKEN_FALSE },
};

const char* boolean_values[2] = {
    FALSE,
    TRUE,
};

void log_error(struct file_tokenizer* tokenizer, const char *msg)
{
    char full_msg[512];
    sprintf(full_msg, "%s:%d:%d: %s", tokenizer->filename, tokenizer->tok_loc.line, tokenizer->tok_loc.col, msg);
    log_info(ERROR, full_msg);
}

enum token_type get_token_type(const char* keyword)
{
    for (size_t i = 0; i < ARRAY_SIZE(keyword_tokens); i++) {
        if (strcmp(keyword_tokens[i].keyword, keyword) == 0)
            return keyword_tokens[i].token;
    }
    return TOKEN_UNK;
}

struct char_token {
    char keyword;
    enum token_type token;
};

static struct char_token char_tokens[] = {
    { '(', TOKEN_LPAREN },
    { ')', TOKEN_RPAREN },
    { '[', TOKEN_LBRACKET },
    { ']', TOKEN_RBRACKET },
};

enum token_type get_char_token_type(char keyword)
{
    for (size_t i = 0; i < ARRAY_SIZE(char_tokens); i++) {
        if (char_tokens[i].keyword == keyword)
            return char_tokens[i].token;
    }
    return TOKEN_UNK;
}

static int get_char(struct file_tokenizer* tokenizer)
{
    int last_char = getc(tokenizer->file);
    if (is_new_line(last_char)) {
        tokenizer->loc.line++;
        tokenizer->loc.col = 0;
    } else
        tokenizer->loc.col++;
    return last_char;
}

struct file_tokenizer* create_tokenizer(FILE* file, const char* filename)
{
    struct file_tokenizer* tokenizer = malloc(sizeof(*tokenizer));
    struct source_loc loc = { 1, 0 };
    tokenizer->loc = loc;
    tokenizer->next_token.token_type = TOKEN_UNK;
    tokenizer->curr_char[0] = ' ';
    tokenizer->curr_char[1] = '\0';
    tokenizer->file = file;
    tokenizer->filename = filename;
    string_init(&tokenizer->str_val);
    return tokenizer;
}

void destroy_tokenizer(struct file_tokenizer* tokenizer)
{
    fclose(tokenizer->file);
    string_deinit(&tokenizer->str_val);
    free(tokenizer);
}

struct token* _tokenize_symbol_type(struct file_tokenizer* tokenizer, struct token* t, enum token_type token_type)
{
    t->token_type = token_type;
    t->loc = tokenizer->tok_loc;
    return t;
}

bool _tokenize_symbol(struct file_tokenizer* tokenizer, string* symbol)
{
    bool has_dot = false;
    do {
        if (tokenizer->curr_char[0] == '.') {
            string_add_chars(symbol, tokenizer->curr_char);
            has_dot = true;
        } else
            break;
    } while ((tokenizer->curr_char[0] = get_char(tokenizer)));
    return has_dot;
}

struct token* _tokenize_number(struct file_tokenizer* tokenizer)
{
    string num_str;
    string_init(&num_str);
    bool has_dot = false;
    do {
        string symbol;
        string_init(&symbol);
        bool met_dot = _tokenize_symbol(tokenizer, &symbol);
        enum token_type type = get_token_type(string_get(&symbol));
        if (type) {
            /*valid token like range ..*/
            if (string_eq_chars(&num_str, "")) {
                return _tokenize_symbol_type(tokenizer, &tokenizer->cur_token, type);
            } else {
                _tokenize_symbol_type(tokenizer, &tokenizer->next_token, type);
                break;
            }
        } else {
            string_add(&num_str, &symbol);
        }
        string_add_chars(&num_str, tokenizer->curr_char);
        tokenizer->curr_char[0] = get_char(tokenizer);
        if (!has_dot)
            has_dot = met_dot;
    } while (isdigit(tokenizer->curr_char[0]) || tokenizer->curr_char[0] == '.');
    if (has_dot) {
        tokenizer->cur_token.double_val = strtod(string_get(&num_str), 0);
        tokenizer->cur_token.type = TYPE_DOUBLE;
    } else {
        tokenizer->cur_token.int_val = atoi(string_get(&num_str));
        tokenizer->cur_token.type = TYPE_INT;
    }
    tokenizer->cur_token.token_type = TOKEN_NUM;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    return &tokenizer->cur_token;
}

struct token* _tokenize_id_keyword(struct file_tokenizer* tokenizer)
{
    string_copy_chars(&tokenizer->str_val, tokenizer->curr_char);
    while (isalnum((tokenizer->curr_char[0] = get_char(tokenizer))) || 
        tokenizer->curr_char[0] == '_' || tokenizer->curr_char[0] == '.') {
        string_add_chars(&tokenizer->str_val, tokenizer->curr_char);
    }
    enum token_type token_type = get_token_type(string_get(&tokenizer->str_val));
    tokenizer->cur_token.token_type = token_type != 0 ? token_type : TOKEN_IDENT;
    if (token_type == TOKEN_TRUE || token_type == TOKEN_FALSE) {
        tokenizer->cur_token.int_val = token_type == TOKEN_TRUE ? 1 : 0;
        tokenizer->cur_token.type = TYPE_BOOL;
    } else
        tokenizer->cur_token.str_val = &tokenizer->str_val;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    //log_info(DEBUG, "id: %s, %d", tokenizer->str_val.c_str(), tokenizer->cur_token.token_type);
    return &tokenizer->cur_token;
}

struct token* _tokenize_char(struct file_tokenizer* tokenizer)
{
    char temp = get_char(tokenizer);
    if (temp == '\''){
        log_error(tokenizer, "empty char is not allowed in character literal");
        return 0;
    }
    tokenizer->cur_token.char_val = temp;
    if(get_char(tokenizer) != '\''){
        log_error(tokenizer, "only one char allowed in character literal");
        return 0;
    }
    tokenizer->cur_token.token_type = TOKEN_CHAR;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    tokenizer->curr_char[0] = get_char(tokenizer);
    //log_info(DEBUG, "id: %s, %d", tokenizer->str_val.c_str(), tokenizer->cur_token.token_type);
    return &tokenizer->cur_token;
}

struct token* _tokenize_string(struct file_tokenizer* tokenizer)
{
    string_copy_chars(&tokenizer->str_val, "");
    while (true) {
        char this_char = get_char(tokenizer);
        /*escape sequence*/
        if (tokenizer->curr_char[0] == '\\'){
            if (this_char == 'n')
                this_char = 0x0a;
        }
        tokenizer->curr_char[0] = this_char;
        if(this_char == '\\'){
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

struct token* _tokenize_op(struct file_tokenizer* tokenizer)
{
    string_copy_chars(&tokenizer->str_val, tokenizer->curr_char);
    while (true) {
        tokenizer->curr_char[0] = get_char(tokenizer);
        if (!is_op_char(tokenizer->curr_char[0]))
            break;
        string_add_chars(&tokenizer->str_val, tokenizer->curr_char);
    }
    tokenizer->cur_token.token_type = TOKEN_OP;
    tokenizer->cur_token.str_val = &tokenizer->str_val;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    //log_info(DEBUG, "id: %s, %d", tokenizer->str_val.c_str(), tokenizer->cur_token.token_type);
    return &tokenizer->cur_token;
}

struct token* _tokenize_type(struct file_tokenizer* tokenizer, enum token_type token_type)
{
    string_copy_chars(&tokenizer->str_val, tokenizer->curr_char);
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    tokenizer->cur_token.str_val = &tokenizer->str_val;
    tokenizer->cur_token.token_type = token_type;
    return &tokenizer->cur_token;
}

void _skip_to_line_end(struct file_tokenizer* tokenizer)
{
    do
        tokenizer->curr_char[0] = get_char(tokenizer);
    while (tokenizer->curr_char[0] != EOF && !is_new_line(tokenizer->curr_char[0]));
}

struct token* get_token(struct file_tokenizer* tokenizer)
{
    // skip spaces
    if (tokenizer->next_token.token_type) {
        // cleanup looked ahead tokens
        tokenizer->cur_token = tokenizer->next_token;
        tokenizer->next_token.token_type = TOKEN_UNK;
        return &tokenizer->cur_token;
    }
    while (isspace(tokenizer->curr_char[0])) {
        if (is_new_line(tokenizer->curr_char[0]))
            break;
        tokenizer->curr_char[0] = get_char(tokenizer);
     }

    tokenizer->tok_loc = tokenizer->loc;
    if (tokenizer->curr_char[0] == EOF)
        return _tokenize_type(tokenizer, TOKEN_EOF);
    else if (is_new_line(tokenizer->curr_char[0])) {
        _tokenize_type(tokenizer, TOKEN_EOL);
        tokenizer->curr_char[0] = ' '; // replaced with empty space
        return &tokenizer->cur_token;
    } else if (tokenizer->curr_char[0] == '\'') {
        return _tokenize_char(tokenizer);
    } else if (tokenizer->curr_char[0] == '"') {
        return _tokenize_string(tokenizer);
    } else if (isalpha(tokenizer->curr_char[0])||tokenizer->curr_char[0] == '_') {
        return _tokenize_id_keyword(tokenizer);
    } else if (isdigit(tokenizer->curr_char[0]) || tokenizer->curr_char[0] == '.') {
        return _tokenize_number(tokenizer);
    } else if (is_op_char(tokenizer->curr_char[0])) {
        return _tokenize_op(tokenizer);
    } else if (tokenizer->curr_char[0] == '#') {
        // skip comments
        _skip_to_line_end(tokenizer);
        if (tokenizer->curr_char[0] != EOF)
            return get_token(tokenizer);
        else
            return _tokenize_type(tokenizer, TOKEN_EOF);
    }

    enum token_type token_type = get_char_token_type(tokenizer->curr_char[0]);
    if (!token_type)
        token_type = TOKEN_OP;
    _tokenize_type(tokenizer, token_type);
    tokenizer->curr_char[0] = get_char(tokenizer);
    return &tokenizer->cur_token;
}

void init_token(struct token* token)
{
    token->token_type = TOKEN_UNK;
    token->loc.line = 0;
    token->loc.col = 0;
    token->type = TYPE_UNK;
}