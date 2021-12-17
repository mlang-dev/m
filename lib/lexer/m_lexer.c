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
#include "lexer/m_lexer.h"
#include "clib/win/libfmemopen.h"
#include "parser/m_grammar.h"
#include "lexer/token.h"


#define CUR_CHAR(tokenizer) tokenizer->curr_char[0]
symbol VARIADIC = 0;
symbol RANGE = 0;
symbol TYPEOF = 0;
symbol ASSIGN = 0;
symbol COMMA = 0;
symbol BINOPDEF = 0;
symbol UNOPDEF = 0;
symbol IMPORT = 0;
symbol EXTERN = 0;
symbol LPAREN = 0;
symbol RPAREN = 0;
symbol IF = 0;
symbol ELSE = 0;
symbol THEN = 0;
symbol FOR = 0;
symbol IN = 0;
symbol TYPE = 0;

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

void _lexer_init(struct tokenizer *tokenizer)
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
    struct token_patterns tps = get_token_patterns();
    for (size_t i = 0; i < tps.pattern_count; ++i) {
        if (tps.patterns[i].pattern && tps.patterns[i].token_type > TOKEN_IDENT)
            rcg_add_exact_match(&tokenizer->rcg_states, tps.patterns[i].pattern, tps.patterns[i].token_type, tps.patterns[i].opcode);
    }

    token_init();
    VARIADIC = to_symbol("...");
    RANGE = to_symbol("..");
    TYPEOF = to_symbol(":");
    ASSIGN = to_symbol("=");
    COMMA = to_symbol(",");
    BINOPDEF = to_symbol("binary");
    UNOPDEF = to_symbol("unary");
    IMPORT = to_symbol("import");
    EXTERN = to_symbol("extern");
    LPAREN = to_symbol("(");
    RPAREN = to_symbol(")");
    IF = to_symbol("if");
    ELSE = to_symbol("else");
    THEN = to_symbol("then");
    FOR = to_symbol("for");
    IN = to_symbol("in");
    TYPE = to_symbol("type");
}

void _lexer_deinit(struct tokenizer *tokenizer)
{
    hashtable_deinit(&tokenizer->keyword_2_tokens);
    rcg_deinit(&tokenizer->rcg_states);
    token_deinit();
}

struct tokenizer *create_tokenizer(FILE *file, const char *filename)
{
    struct tokenizer *tokenizer;
    MALLOC(tokenizer, sizeof(*tokenizer));
    _lexer_init(tokenizer);
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

struct tokenizer *create_tokenizer_for_string(const char *content)
{
    FILE *file = fmemopen((void *)content, strlen(content), "r");
    struct tokenizer *tokenizer = create_tokenizer(file, "");
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
        if(tokenizer->cur_token.symbol_val == VARIADIC)
            tokenizer->cur_token.token_type = TOKEN_VARIADIC;
        else if(tokenizer->cur_token.symbol_val == RANGE)
            tokenizer->cur_token.token_type = TOKEN_RANGE;
        else
            tokenizer->cur_token.token_type = TOKEN_OP;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        return &tokenizer->cur_token;
    } else if (isdigit(tokenizer->curr_char[0])) {
        _collect_all_digits(tokenizer, &str);
        tokenizer->cur_token.double_val = strtod(string_get(&str), 0);
        tokenizer->cur_token.token_type = TOKEN_FLOAT;
        tokenizer->cur_token.loc = tokenizer->tok_loc;
        return &tokenizer->cur_token;
    } else {
        tokenizer->cur_token.token_type = TOKEN_OP;
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
        if (ks && ks->accepted_token_type && !ks->identifiable && !ks_next)
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
        ks->accepted_token_type == TOKEN_TRUE || 
        ks->accepted_token_type == TOKEN_FALSE){
        tokenizer->cur_token.token_type = ks->accepted_token_type;
    }else{
        tokenizer->cur_token.token_type = ks->accepted_token_type ? TOKEN_OP : TOKEN_IDENT;
    }
    tokenizer->cur_token.symbol_val = to_symbol(string_get(&tokenizer->str_val));
    if (tokenizer->cur_token.symbol_val == TYPEOF)
        tokenizer->cur_token.token_type = TOKEN_ISTYPEOF;
    else if (tokenizer->cur_token.symbol_val == ASSIGN)
        tokenizer->cur_token.token_type = TOKEN_ASSIGN;
    else if (tokenizer->cur_token.symbol_val == COMMA)
        tokenizer->cur_token.token_type = TOKEN_COMMA;
    else if (tokenizer->cur_token.symbol_val == IMPORT)
        tokenizer->cur_token.token_type = TOKEN_IMPORT;
    else if (tokenizer->cur_token.symbol_val == EXTERN)
        tokenizer->cur_token.token_type = TOKEN_EXTERN;
    else if (tokenizer->cur_token.symbol_val == LPAREN)
        tokenizer->cur_token.token_type = TOKEN_LPAREN;
    else if (tokenizer->cur_token.symbol_val == RPAREN)
        tokenizer->cur_token.token_type = TOKEN_RPAREN;
    else if (tokenizer->cur_token.symbol_val == IF)
        tokenizer->cur_token.token_type = TOKEN_IF;
    else if (tokenizer->cur_token.symbol_val == ELSE)
        tokenizer->cur_token.token_type = TOKEN_ELSE;
    else if (tokenizer->cur_token.symbol_val == THEN)
        tokenizer->cur_token.token_type = TOKEN_THEN;
    else if (tokenizer->cur_token.symbol_val == FOR)
        tokenizer->cur_token.token_type = TOKEN_FOR;
    else if (tokenizer->cur_token.symbol_val == IN)
        tokenizer->cur_token.token_type = TOKEN_IN;
    else if (tokenizer->cur_token.symbol_val == TYPE)
        tokenizer->cur_token.token_type = TOKEN_TYPE;
    tokenizer->cur_token.loc = tokenizer->tok_loc;
    if(ks&&ks->accepted_token_type == TOKEN_OP){
        tokenizer->cur_token.opcode = ks->accepted_opcode;
    }
    // }else{
    //     tokenizer->cur_token.opcode = 0;
    // }
        
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
