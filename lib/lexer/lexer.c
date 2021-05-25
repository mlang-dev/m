/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m lexer, m tokenizer
 */
#include <assert.h>
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>

#include "clib/hashtable.h"
#include "clib/string.h"
#include "clib/util.h"
#include "lexer/lexer.h"

const char *token_type_strings[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

#define CUR_CHAR(tokenizer) tokenizer->curr_char[0]

char op_chars[] = {
    '!', '%', '&', '*', '+', '-', '/', '<', '=', '>', '?', '@', '^', '|', ':'
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

static struct keyword_token keyword_tokens[] = {
    { "import", TOKEN_KEYWORD },
    { "extern", TOKEN_KEYWORD },
    { "type", TOKEN_KEYWORD },
    { "if", TOKEN_IF },
    { "else", TOKEN_ELSE },
    { "then", TOKEN_THEN },
    { "in", TOKEN_IN },
    { "for", TOKEN_FOR },
    { "unary", TOKEN_KEYWORD },
    { "binary", TOKEN_KEYWORD },
    { "||", TOKEN_KEYWORD },
    { "&&", TOKEN_KEYWORD },
    { "..", TOKEN_RANGE },
    { "...", TOKEN_KEYWORD },
    { "true", TOKEN_TRUE },
    { "false", TOKEN_FALSE },
};

struct hashtable keyword_2_tokens;

void log_error(struct file_tokenizer *tokenizer, const char *msg)
{
    char full_msg[512];
    sprintf(full_msg, "%s:%d:%d: %s", tokenizer->filename, tokenizer->tok_loc.line, tokenizer->tok_loc.col, msg);
    log_info(ERROR, full_msg);
}

enum token_type get_token_type(const char *keyword)
{
    for (size_t i = 0; i < ARRAY_SIZE(keyword_tokens); i++) {
        if (strcmp(keyword_tokens[i].keyword, keyword) == 0)
            return keyword_tokens[i].token;
    }
    return TOKEN_NULL;
}

struct char_token {
    char keyword;
    enum token_type token;
};

static struct char_token char_tokens[] = {
    { '(', TOKEN_KEYWORD },
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
    return TOKEN_NULL;
}

int get_char(struct file_tokenizer *tokenizer)
{
    int last_char;
    if (tokenizer->peek >= 0) {
        last_char = tokenizer->peek;
        tokenizer->peek = -1;
    } else {
        last_char = getc(tokenizer->file);
    }
    if (is_new_line(last_char)) {
        tokenizer->loc.line++;
        tokenizer->loc.col = 0;
    } else
        tokenizer->loc.col++;
    return last_char;
}

int peek_char(struct file_tokenizer *tokenizer)
{
    assert(tokenizer->peek == -1); // only 1 peek is allowed.
    tokenizer->peek = getc(tokenizer->file);
    return tokenizer->peek;
}

void lexer_init()
{
    hashtable_init_with_value_size(&keyword_2_tokens, sizeof(enum token_type), 0);
    for (size_t i = 0; i < ARRAY_SIZE(keyword_tokens); ++i) {
        symbol keyword = to_symbol(keyword_tokens[i].keyword);
        hashtable_set_p(&keyword_2_tokens, keyword, &keyword_tokens[i].token);
    }
}

void lexer_deinit()
{
    hashtable_deinit(&keyword_2_tokens);
}

struct file_tokenizer *create_tokenizer(FILE *file, const char *filename)
{
    struct file_tokenizer *tokenizer = malloc(sizeof(*tokenizer));
    struct source_loc loc = { 1, 0 };
    tokenizer->loc = loc;
    tokenizer->curr_char[0] = ' ';
    tokenizer->curr_char[1] = '\0';
    tokenizer->peek = -1;
    tokenizer->file = file;
    tokenizer->filename = filename;
    string_init(&tokenizer->str_val);
    return tokenizer;
}

void destroy_tokenizer(struct file_tokenizer *tokenizer)
{
    fclose(tokenizer->file);
    string_deinit(&tokenizer->str_val);
    free(tokenizer);
}

void _collect_all_dots(struct file_tokenizer *tokenizer, string *symbol)
{
    while (tokenizer->curr_char[0] == '.') {
        string_add_chars(symbol, tokenizer->curr_char);
        tokenizer->curr_char[0] = get_char(tokenizer);
    }
}

void _collect_all_digits(struct file_tokenizer *tokenizer, string *str)
{
    while (isdigit(tokenizer->curr_char[0])) {
        string_add_chars(str, tokenizer->curr_char);
        tokenizer->curr_char[0] = get_char(tokenizer);
    }
}

struct token *_tokenize_dot(struct file_tokenizer *tokenizer)
{
    assert(tokenizer->curr_char[0] == '.');
    string str;
    string_init_chars(&str, tokenizer->curr_char);
    tokenizer->curr_char[0] = get_char(tokenizer);
    if (tokenizer->curr_char[0] == '.') {
        _collect_all_dots(tokenizer, &str);
        enum token_type type = get_token_type(string_get(&str));
        assert(type);
        if (type == TOKEN_KEYWORD) {
            tokenizer->cur_token.keyword = to_symbol(string_get(&str));
        }
        tokenizer->cur_token.token_type = type;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        return &tokenizer->cur_token;
    } else if (isdigit(tokenizer->curr_char[0])) {
        _collect_all_digits(tokenizer, &str);
        tokenizer->cur_token.double_val = strtod(string_get(&str), 0);
        tokenizer->cur_token.token_type = TOKEN_FLOAT;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        return &tokenizer->cur_token;
    } else {
        tokenizer->cur_token.token_type = TOKEN_KEYWORD;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        tokenizer->cur_token.keyword = to_symbol(".");
        return &tokenizer->cur_token;
    }
}

struct token *_tokenize_number_literal(struct file_tokenizer *tokenizer)
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

struct token *_tokenize_id_keyword(struct file_tokenizer *tokenizer)
{
    string_copy_chars(&tokenizer->str_val, tokenizer->curr_char);
    while (isalnum((tokenizer->curr_char[0] = get_char(tokenizer))) || tokenizer->curr_char[0] == '_' || tokenizer->curr_char[0] == '.') {
        string_add_chars(&tokenizer->str_val, tokenizer->curr_char);
    }
    enum token_type token_type = get_token_type(string_get(&tokenizer->str_val));
    tokenizer->cur_token.token_type = token_type != 0 ? token_type : TOKEN_IDENT;
    if (token_type == TOKEN_TRUE || token_type == TOKEN_FALSE) {
        tokenizer->cur_token.int_val = token_type == TOKEN_TRUE ? 1 : 0;
    } else
        tokenizer->cur_token.str_val = &tokenizer->str_val;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    if (token_type == TOKEN_KEYWORD) {
        tokenizer->cur_token.keyword = to_symbol(string_get(&tokenizer->str_val));
    }
    return &tokenizer->cur_token;
}

struct token *_tokenize_char_literal(struct file_tokenizer *tokenizer)
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

struct token *_tokenize_string_literal(struct file_tokenizer *tokenizer)
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

struct token *_tokenize_op(struct file_tokenizer *tokenizer)
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
    tokenizer->cur_token.keyword = to_symbol(string_get(tokenizer->cur_token.str_val));
    return &tokenizer->cur_token;
}

struct token *_tokenize_type(struct file_tokenizer *tokenizer, enum token_type token_type)
{
    string_copy_chars(&tokenizer->str_val, tokenizer->curr_char);
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    tokenizer->cur_token.str_val = &tokenizer->str_val;
    tokenizer->cur_token.token_type = token_type;
    return &tokenizer->cur_token;
}

void _skip_to_line_end(struct file_tokenizer *tokenizer)
{
    do
        tokenizer->curr_char[0] = get_char(tokenizer);
    while (tokenizer->curr_char[0] != EOF && !is_new_line(tokenizer->curr_char[0]));
}

struct token *get_token(struct file_tokenizer *tokenizer)
{
    // skip spaces
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
        return _tokenize_char_literal(tokenizer);
    } else if (tokenizer->curr_char[0] == '"') {
        return _tokenize_string_literal(tokenizer);
    } else if (isalpha(tokenizer->curr_char[0]) || tokenizer->curr_char[0] == '_') {
        return _tokenize_id_keyword(tokenizer);
    } else if (tokenizer->curr_char[0] == '.') {
        return _tokenize_dot(tokenizer);
    } else if (isdigit(tokenizer->curr_char[0])) {
        return _tokenize_number_literal(tokenizer);
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
    _tokenize_type(tokenizer, token_type);
    tokenizer->cur_token.keyword = to_symbol(tokenizer->curr_char);
    tokenizer->curr_char[0] = get_char(tokenizer);
    return &tokenizer->cur_token;
}

void init_token(struct token *token)
{
    token->token_type = TOKEN_NULL;
    token->loc.line = 0;
    token->loc.col = 0;
}
