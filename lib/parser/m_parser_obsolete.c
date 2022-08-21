/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language parser
 */
#include <assert.h>
#include <errno.h>

#include "clib/hashtable.h"
#include "clib/util.h"
#include "clib/win/libfmemopen.h"
#include "parser/astdump.h"
#include "parser/m_parser.h"

#define exit_block(parser, parent, base_col) (parent && parser->curr_token.loc.col < base_col && (parser->curr_token.token_type != TOKEN_NEWLINE || parser->is_repl))

int _op_preces[OP_TOTAL];

#define MAX_PRECEDENCE 400

int _get_op_precedence(struct m_parser *parser);
struct ast_node *_parse_number(struct m_parser *parser, struct ast_node *parent);
struct ast_node *_parse_parentheses(struct m_parser *parser, struct ast_node *parent);
struct ast_node *_parse_node(struct m_parser *parser, struct ast_node *parent);
struct ast_node *_parse_binary(struct m_parser *parser, struct ast_node *parent, int exp_prec, struct ast_node *lhs);
struct ast_node *_parse_for(struct m_parser *parser, struct ast_node *parent);
struct ast_node *_parse_type(struct m_parser *parser, struct ast_node *parent);
struct ast_node *_parse_if(struct m_parser *parser, struct ast_node *parent);
struct ast_node *_parse_unary(struct m_parser *parser, struct ast_node *parent);
struct ast_node *_parse_func_type(struct m_parser *parser, struct ast_node *parent, bool is_external);
struct ast_node *_parse_var(struct m_parser *parser, struct ast_node *parent, symbol name, enum type type, symbol ext_type);
struct ast_node *_parse_function_with_func_type(struct m_parser *parser, struct ast_node *func_type);
struct ast_node *_parse_block(struct m_parser *parser, struct ast_node *parent, void (*fun)(void *, struct ast_node *), void *jit);

struct op_type {
    //symbol op;
    enum op_code op;
    enum type type;
    symbol type_symbol;
    bool success;
};

struct m_parser *g_parser = 0;

const char *get_ctt(struct m_parser *parser)
{
    return token_type_strings[parser->curr_token.token_type];
}

bool _is_exp(struct ast_node *node)
{
    return node->node_type != VAR_NODE && node->node_type != FUNC_NODE && node->node_type != FUNC_TYPE_NODE;
}

void _log_error(struct m_parser *parser, struct source_location loc, const char *msg)
{
    char full_msg[512];
    sprintf_s(full_msg, sizeof(full_msg), "%s:%d:%d: %s", string_get(parser->current_module->name), loc.line, loc.col, msg);
    log_info(ERROR, full_msg);
}

void queue_token(struct m_parser *psr, struct token tkn)
{
    queue_push(&psr->queued_tokens, &tkn);
}

void queue_tokens(struct m_parser *psr, struct array *tokens)
{
    for (size_t i = 0; i < array_size(tokens); i++) {
        struct token *tok = (struct token *)array_get(tokens, i);
        queue_push(&psr->queued_tokens, tok);
    }
}

void _token_init(struct token *token)
{
    token->token_type = TOKEN_EOF;
    token->loc.line = 0;
    token->loc.col = 0;
}

struct m_parser *m_parser_new(bool is_repl)
{
    struct m_parser *parser;
    MALLOC(parser, sizeof(*parser));

    symboltable_init(&parser->vars);
    queue_init(&parser->queued_tokens, sizeof(struct token));
    hashtable_init(&parser->ext_types);
    hashtable_init_with_value_size(&parser->symbol_2_int_types, sizeof(int), 0);
    for (int i = 0; i < TYPE_TYPES; i++) {
        hashtable_set_int(&parser->symbol_2_int_types, to_symbol(type_strings[i]), i);
    }
    for(int i = 0; i < OP_TOTAL; i++){
        _op_preces[i] = -1;
    }
    _op_preces[OP_OR] = 50;
    _op_preces[OP_AND] = 50;
    _op_preces[OP_NOT] = 50;
    _op_preces[OP_LT] = 100;
    _op_preces[OP_GT] = 100;
    _op_preces[OP_EQ] = 100;
    _op_preces[OP_NE] = 100;
    _op_preces[OP_LE] = 100;
    _op_preces[OP_GE] = 100;
    _op_preces[OP_PLUS] = 200;
    _op_preces[OP_MINUS] = 200;
    _op_preces[OP_TIMES] = 400;
    _op_preces[OP_DIVISION] = 400;
    _op_preces[OP_MODULUS] = 400;
    _op_preces[OP_EXPO] = 500;

    parser->allow_id_as_a_func = true;
    parser->id_is_var_decl = false;
    parser->is_repl = is_repl;
    parser->current_module = 0;
    parser->lexer = 0;
    _token_init(&parser->curr_token);
    g_parser = parser;

    return parser;
}

void module_free(struct module *module)
{
    node_free(module->block);
    FREE(module);
}

void m_parser_free(struct m_parser *parser)
{
    hashtable_deinit(&parser->symbol_2_int_types);
    hashtable_deinit(&parser->ext_types);
    symboltable_deinit(&parser->vars);
    FREE(parser);
    g_parser = 0;
}

void parse_next_token(struct m_parser *parser)
{
    if (queue_size(&parser->queued_tokens) > 0) {
        // cleanup queued tokens, to redo parsing
        parser->curr_token = *(struct token *)queue_pop(&parser->queued_tokens);
    } else{
        parser->curr_token = *get_tok(parser->lexer);
    }
}

int _get_op_precedence(struct m_parser *parser)
{
    if (parser->curr_token.token_type != TOKEN_OP || parser->curr_token.opcode > OP_TOTAL || parser->curr_token.opcode < 0)
        return -1;
    return _op_preces[parser->curr_token.opcode];
}

struct ast_node *_parse_bool_value(struct m_parser *parser, struct ast_node *parent)
{
    struct ast_node *result;
    result = bool_node_new(
        parser->curr_token.token_type == TOKEN_TRUE ? 1 : 0, parser->curr_token.loc);
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return result;
}

struct ast_node *_parse_char(struct m_parser *parser, struct ast_node *parent)
{
    struct ast_node *result;
    result = char_node_new(
        parser->curr_token.char_val, parser->curr_token.loc);
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return result;
}

struct ast_node *_parse_string(struct m_parser *parser, struct ast_node *parent)
{
    struct ast_node *result;
    result = string_node_new(
        string_get(parser->curr_token.str_val), parser->curr_token.loc);
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return result;
}

struct ast_node *_parse_number(struct m_parser *parser, struct ast_node *parent)
{
    struct ast_node *result = 0;
    if (parser->curr_token.token_type == TOKEN_INT)
        result = int_node_new(
            parser->curr_token.int_val, parser->curr_token.loc);
    else if (parser->curr_token.token_type == TOKEN_FLOAT)
        result = double_node_new(
            parser->curr_token.double_val, parser->curr_token.loc);
    else
        assert(false);
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return result;
}

struct ast_node *_parse_parentheses(struct m_parser *parser, struct ast_node *parent)
{
    struct ast_node *v;
    parse_next_token(parser);
    if (parser->curr_token.token_type == TOKEN_RPAREN) {
        v = unit_node_new(parser->curr_token.loc);
        parse_next_token(parser);
        return v;
    }
    v = parse_exp(parser, parent, 0);
    if (!v)
        return 0;
    if (parser->curr_token.token_type != TOKEN_RPAREN)
        return log_info(ERROR, "expected ')'");
    parse_next_token(parser);
    return v;
}

struct op_type _parse_op_type(struct m_parser *parser, struct source_location loc)
{
    struct op_type optype;
    optype.type_symbol = 0;
    optype.op = 0;
    optype.type = TYPE_UNK;
    if (parser->curr_token.token_type == TOKEN_OP) {
        optype.op = parser->curr_token.opcode;
    }
    if(parser->curr_token.token_type == TOKEN_ISTYPEOF){
    //if (optype.op == parser->type_of) {
        // type of definition
        parse_next_token(parser); /* skip ':'*/
        symbol type_symbol = parser->curr_token.symbol_val;
        if (!hashtable_in_p(&parser->symbol_2_int_types, type_symbol)) {
            string error;
            string_init_chars(&error, "wrong type: ");
            string_add(&error, parser->curr_token.symbol_val);
            _log_error(parser, loc, string_get(&error));
            optype.success = false;
            return optype;
        }
        optype.type = hashtable_get_int(&parser->symbol_2_int_types, type_symbol);
        optype.type_symbol = type_symbol;
        parse_next_token(parser); /*skip type*/
        if (parser->curr_token.token_type == TOKEN_OP)
            optype.op = parser->curr_token.opcode;
    }
    optype.success = true;
    return optype;
}

struct ast_node *_parse_type_value_node(struct m_parser *parser, struct ast_node *parent, symbol ext_type_symbol)
{
    assert(ext_type_symbol);
    struct ast_node *type = hashtable_get_p(&parser->ext_types, ext_type_symbol);
    assert(type);
    struct ast_node *block = _parse_block(parser, parent, 0, 0);
    if (block) {
        return type_value_node_new(block, ext_type_symbol, parser->curr_token.loc);
    }
    return 0;
}

struct ast_node *_parse_function_app_or_def(struct m_parser *parser, struct ast_node *parent, struct source_location loc, symbol pid_name, bool is_operator, int precedence)
{
    if (parser->curr_token.token_type == TOKEN_LPAREN) {
        parse_next_token(parser); // skip '('
    }
    bool func_definition = false;
    bool is_variadic = false;
    struct array args;
    array_init(&args, sizeof(struct ast_node *));
    // doesn't allow function inside parameter or argument
    // will be enhanced later
    string id_name;
    string_init_chars(&id_name, string_get(pid_name));
    parser->allow_id_as_a_func = false;
    struct type_exp *ret_type = 0;
    if (hashtable_in_p(&parser->ext_types, pid_name)) {
        return _parse_type_value_node(parser, parent, pid_name);
    }

    if (parser->curr_token.token_type != TOKEN_RPAREN) {
        while (true) {
            if (parser->curr_token.token_type == TOKEN_VARIADIC) {
                is_variadic = true;
                parse_next_token(parser);
            } else {
                struct ast_node *arg = parse_exp(parser, parent, 0);
                if (arg) {
                    if (is_variadic) {
                        _log_error(parser, arg->loc, "no parameter allowed after variadic");
                        return 0;
                    }
                    if (arg->node_type == IDENT_NODE) {
                        struct op_type optype = _parse_op_type(parser, arg->loc);
                        if (optype.type) {
                            arg->annotated_type_name = optype.type_symbol;
                            arg->annotated_type_enum = optype.type;
                        }
                    }
                    array_push(&args, &arg);
                }
            }
            if (parser->curr_token.token_type == TOKEN_RPAREN) {
                parse_next_token(parser);
                struct op_type optype = _parse_op_type(parser, parser->curr_token.loc);
                if (optype.type) {
                    ret_type = (struct type_exp *)create_nullary_type(optype.type, optype.type_symbol);
                }
            }
            if (parser->curr_token.token_type == TOKEN_ASSIGN) {
                func_definition = true;
                break;
            } else if (parser->curr_token.token_type == TOKEN_NEWLINE || parser->curr_token.token_type == TOKEN_EOF)
                break;
            else if (parser->curr_token.token_type == TOKEN_COMMA)
                parse_next_token(parser);
        }
    } else {
        /*looks we got (), if next one is = then it's definition*/
        parse_next_token(parser);
        if (parser->curr_token.token_type == TOKEN_ASSIGN)
            func_definition = true;
    }
    parser->allow_id_as_a_func = true;
    // log_info(DEBUG, "is %s a function def: %d, %d, %zu", string_get(&id_name), func_definition, is_variadic, array_size(&args));
    if (func_definition) {
        ARRAY_FUN_PARAM(fun_params);
        for (size_t i = 0; i < array_size(&args); i++) {
            struct ast_node *id = *(struct ast_node **)array_get(&args, i);
            struct ast_node *fun_param = var_node_new(id->ident->name, id->annotated_type_enum, id->annotated_type_name, 0, !parent, parser->curr_token.loc);
            array_push(&fun_params, &fun_param);
        }
        if (is_operator) {
            if (precedence && array_size(&fun_params) != BINARY_PARAM_SIZE) {
                return log_info(ERROR, "precedence only apply for binary operator");
            } else if (array_size(&fun_params) != UNARY_PARAM_SIZE && array_size(&fun_params) != BINARY_PARAM_SIZE)
                return log_info(ERROR, "operator overloading only for unary or binary operator");
            if (array_size(&fun_params) == UNARY_PARAM_SIZE) {
                string_copy_chars(&id_name, "unary");
                string_add_chars(&id_name, string_get(pid_name));
                // id_name = "unary" + id_name;
            } else {
                string_copy_chars(&id_name, "binary");
                string_add_chars(&id_name, string_get(pid_name));
            }
        }
        symbol id_symbol = string_2_symbol(&id_name);
        struct ast_node *params = block_node_new(&fun_params);
        struct ast_node *func_type = func_type_node_new(id_symbol, params, ret_type && ret_type->type ? to_symbol(type_strings[ret_type->type]) : 0,
            is_operator, precedence, is_operator ? id_symbol : EmptySymbol, is_variadic, false, loc);
        return _parse_function_with_func_type(parser, func_type);
    }
    // function application
    symbol name_symbol = string_2_symbol(&id_name);
    struct ast_node *arg_block = block_node_new(&args);
    array_copy(&arg_block->block->nodes, &args); //? we have to copy array
    struct ast_node *call = call_node_new(name_symbol, arg_block, loc);
    return parse_exp(parser, parent, call);
}

struct ast_node *parse_statement(struct m_parser *parser, struct ast_node *parent)
{
    struct ast_node *node = 0;
    struct source_location loc = parser->curr_token.loc;
    if (parser->curr_token.token_type == TOKEN_LET)
        parse_next_token(parser);
    if (parser->curr_token.token_type == TOKEN_EOF)
        return 0;
    else if (parser->curr_token.token_type == TOKEN_IMPORT)
        node = parse_import(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_TYPE)
        node = _parse_type(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_EXTERN) {
        parse_next_token(parser);
        node = _parse_func_type(parser, parent, true);
    /*TODO: not supported
    } else if (parser->curr_token.token_type == TOKEN_BINOPDEF || parser->curr_token.token_type == TOKEN_UNOPDEF) {
        // function def
        struct ast_node *func_type = _parse_func_type(parser, parent, false);
        node = _parse_function_with_func_type(parser, func_type);
    */
    } else if (parser->curr_token.token_type == TOKEN_IDENT) {
        symbol id_symbol = parser->curr_token.symbol_val;
        struct source_location current_loc = parser->curr_token.loc;
        parse_next_token(parser); // skip identifier
        struct op_type optype = _parse_op_type(parser, current_loc);
        if (!optype.success)
            return 0;
        if (parser->curr_token.token_type == TOKEN_OP && parser->curr_token.opcode == OP_DOT){
            //collect more id: so far only two dots supported
            string ids;
            string_init_chars(&ids, string_get(id_symbol));
            string_add_chars(&ids, ".");
            parse_next_token(parser);
            assert(parser->curr_token.token_type == TOKEN_IDENT);
            string_add(&ids, parser->curr_token.symbol_val);
            id_symbol = string_2_symbol(&ids);
            parser->curr_token.symbol_val = id_symbol;
            string_deinit(&ids);
            parse_next_token(parser);
        }
        if (parser->id_is_var_decl) {
            /*id is var decl*/
            node = var_node_new2(id_symbol, optype.type_symbol, 0, !parent, current_loc);
        } else if (parser->curr_token.token_type == TOKEN_ASSIGN || optype.type) { //|| !has_symbol(&parser->vars, id_symbol)
            // variable definition
            node = _parse_var(parser, parent, id_symbol, optype.type, optype.type_symbol);
        } else if (parser->curr_token.token_type == TOKEN_NEWLINE || parser->curr_token.token_type == TOKEN_EOF || (parser->curr_token.token_type == TOKEN_OP &&_op_preces[optype.op] > 0)) {
            // just id expression evaluation
            struct ast_node *lhs = ident_node_new(id_symbol, parser->curr_token.loc);
            node = parse_exp(parser, parent, lhs);
        } else {
            // function definition or application
            node = _parse_function_app_or_def(parser, parent, current_loc, id_symbol, false, 0);
        }
    } else {
        if (parser->curr_token.token_type == TOKEN_LPAREN) {
            struct array queued;
            array_init(&queued, sizeof(struct token));
            array_push(&queued, &parser->curr_token);
            parse_next_token(parser); // skip (
            array_push(&queued, &parser->curr_token);
            if (parser->curr_token.token_type == TOKEN_OP) {
                // it is operator overloading
                symbol op = get_symbol_by_token_opcode(parser->curr_token.token_type, parser->curr_token.opcode);
                parse_next_token(parser);
                if (parser->curr_token.token_type != TOKEN_RPAREN)
                    return log_info(ERROR, "expected ')'");
                parse_next_token(parser);
                int precedence = 0;
                if (parser->curr_token.token_type == TOKEN_INT) {
                    // precedence: parse binary
                    precedence = parser->curr_token.int_val;
                    parse_next_token(parser); // skip it
                }
                node = _parse_function_app_or_def(parser, parent, loc, op, true, precedence);
            } else { // normal exp
                queue_tokens(parser, &queued);
                parse_next_token(parser); // retrieving (
                node = parse_exp(parser, parent, 0);
            }
            array_deinit(&queued);
        } else {
            node = parse_exp(parser, parent, 0);
        }
    }
    if (node) {
        node->loc = loc;
    }
    return node;
}

bool _is_new_line(int cha)
{
    return cha == '\r' || cha == '\n';
}

bool _id_is_a_function_call(struct m_parser *parser)
{
    return parser->allow_id_as_a_func && (parser->curr_token.token_type == TOKEN_IDENT || parser->curr_token.token_type == TOKEN_INT || parser->curr_token.token_type == TOKEN_FLOAT || /*TODO NOT SUPPORTED parser->curr_token.token_type == TOKEN_UNOPDEF ||*/ parser->curr_token.token_type == TOKEN_LPAREN || parser->curr_token.token_type == TOKEN_IF);
}

struct ast_node *_parse_ident(struct m_parser *parser, struct ast_node *parent)
{
    symbol id_symbol = parser->curr_token.symbol_val;
    struct source_location loc = parser->curr_token.loc;

    parse_next_token(parser); // take identifier
    if (_id_is_a_function_call(parser)) {
        struct ast_node *exp = 0;
        if (hashtable_in_p(&parser->ext_types, id_symbol)) {
            exp = _parse_type_value_node(parser, parent, id_symbol);
        } else {
            struct array args;
            array_init(&args, sizeof(struct ast_node *));
            while (true) {
                struct ast_node *arg = parse_exp(parser, parent, 0);
                assert(arg);
                if (!(arg->node_type == LITERAL_NODE && arg->annotated_type_enum == TYPE_UNIT))
                    array_push(&args, &arg);
                if (!_id_is_a_function_call(parser))
                    break;
            }
            parse_next_token(parser);
            struct ast_node *arg_block = block_node_new(&args);
            array_copy(&arg_block->block->nodes, &args); //? we have to copy array
            exp = call_node_new(id_symbol, arg_block, loc);
            array_deinit(&args);
        }
        return exp;
    }
    return ident_node_new(id_symbol, loc);
}

struct ast_node *_parse_node(struct m_parser *parser, struct ast_node *parent)
{
    switch(parser->curr_token.token_type){
        case TOKEN_TRUE:
        case TOKEN_FALSE:
            return _parse_bool_value(parser, parent);
        default:
            break;
    }
    if (parser->curr_token.token_type == TOKEN_IDENT)
        return _parse_ident(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_INT || parser->curr_token.token_type == TOKEN_FLOAT)
        return _parse_number(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_CHAR)
        return _parse_char(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_STRING)
        return _parse_string(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_IF)
        return _parse_if(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_FOR)
        return _parse_for(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_LPAREN)
        return _parse_parentheses(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_OP && parser->curr_token.opcode == OP_NOT)
        return _parse_unary(parser, parent);
    else {
        string error;
        string_init_chars(&error, "unknown token: ");
        string_add_chars(&error, token_type_strings[parser->curr_token.token_type]);
        if (parser->curr_token.token_type == TOKEN_OP) {
            symbol sym_op = get_symbol_by_token_opcode(parser->curr_token.token_type, parser->curr_token.opcode);
            string_add_chars(&error, " op: ");
            string_add(&error, sym_op);
        }
        parse_next_token(parser);
        return log_info(ERROR, string_get(&error));
    }
}

struct ast_node *_parse_binary(struct m_parser *parser, struct ast_node *parent, int exp_prec, struct ast_node *lhs)
{
    while (true) {
        if (parser->curr_token.token_type == TOKEN_NEWLINE || parser->curr_token.token_type == TOKEN_EOF)
            return lhs;
        int tok_prec = _get_op_precedence(parser);
        if (tok_prec < exp_prec)
            return lhs;
        enum op_code opcode = parser->curr_token.opcode;
        parse_next_token(parser);
        struct ast_node *rhs = _parse_unary(parser, parent);
        if (!rhs)
            return lhs;
        int next_prec = _get_op_precedence(parser);
        if (tok_prec < next_prec) {
            rhs = _parse_binary(parser, parent, tok_prec + 1, rhs);
            if (!rhs)
                return 0;
        }
        lhs = binary_node_new(opcode, lhs, rhs, lhs->loc);
    }
}

struct ast_node *parse_exp(struct m_parser *parser, struct ast_node *parent, struct ast_node *lhs)
{
    if (parser->curr_token.token_type == TOKEN_NEWLINE)
        return lhs;
    if (!lhs)
        lhs = _parse_unary(parser, parent);
    if (!lhs || parser->curr_token.token_type == TOKEN_NEWLINE)
        return lhs;
    return _parse_binary(parser, parent, 0, lhs);
}

/// func_type
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
struct ast_node *_parse_func_type(struct m_parser *parser, struct ast_node *parent, bool is_external)
{
    string fun_name;
    struct source_location loc = parser->curr_token.loc;
    // SourceLocation FnLoc = CurLoc;
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    if (parser->curr_token.token_type == TOKEN_IDENT) {
        string_copy(&fun_name, parser->curr_token.symbol_val);
        proto_type = 0;
        parse_next_token(parser);
    /*TODO NOT SUPPORTED
    } else if (parser->curr_token.token_type == TOKEN_UNOPDEF) {
        parse_next_token(parser);
        if (parser->curr_token.token_type != TOKEN_OP)
            return log_info(ERROR, "Expected unary operator");
        string_init_chars(&fun_name, "unary");
        string_add(&fun_name, parser->curr_token.symbol_val);
        proto_type = 1;
        parse_next_token(parser);
    } else if (parser->curr_token.token_type == TOKEN_BINOPDEF) {
        parse_next_token(parser);
        if (parser->curr_token.token_type != TOKEN_OP)
            return log_info(ERROR, "Expected binary operator");
        string_init_chars(&fun_name, "binary");
        string_add(&fun_name, parser->curr_token.symbol_val);
        proto_type = 2;
        parse_next_token(parser);
        // Read the precedence if present.
        if (parser->curr_token.token_type == TOKEN_INT) {
            if (parser->curr_token.int_val < 1 || parser->curr_token.int_val > MAX_PRECEDENCE) {
                return log_info(ERROR, "Invalid precedecnce: must be 1..100");
            }
            parse_next_token(parser);
        }
    */
    } else {
        return log_info(ERROR, "Expected function name in func_type");
    }
    bool has_parenthese = parser->curr_token.token_type == TOKEN_LPAREN;
    if (has_parenthese)
        parse_next_token(parser); // skip '('
    ARRAY_FUN_PARAM(fun_params);
    struct op_type optype;
    while (parser->curr_token.token_type == TOKEN_IDENT) {
        symbol var_name = parser->curr_token.symbol_val;
        parse_next_token(parser);
        optype = _parse_op_type(parser, parser->curr_token.loc);
        struct ast_node *fun_param = var_node_new2(var_name, 0, 0, true, parser->curr_token.loc);
        if (optype.success && optype.type) {
            fun_param->annotated_type_name = optype.type_symbol;
            fun_param->annotated_type_enum = optype.type;
        }
        array_push(&fun_params, &fun_param);
        if (parser->curr_token.token_type == TOKEN_COMMA)
            parse_next_token(parser);
    }
    bool is_variadic = parser->curr_token.token_type == TOKEN_VARIADIC;
    if (is_variadic)
        parse_next_token(parser);
    if (has_parenthese && parser->curr_token.token_type != TOKEN_RPAREN)
        return log_info(ERROR, "Expected ')' to match '('");
    // success.
    if (has_parenthese)
        parse_next_token(parser); // eat ')'.
    struct type_exp *ret_type = 0;
    optype = _parse_op_type(parser, parser->curr_token.loc);
    if (optype.type)
        ret_type = (struct type_exp *)create_nullary_type(optype.type, optype.type_symbol);
    // Verify right number of names for operator.
    if (proto_type && array_size(&fun_params) != proto_type)
        return log_info(ERROR, "Invalid number of operands for operator");
    symbol fun_name_symbol = string_2_symbol(&fun_name);
    struct ast_node *params = block_node_new(&fun_params);
    struct ast_node *ret = func_type_node_new(fun_name_symbol, params,
        ret_type && ret_type->type ? to_symbol(type_strings[ret_type->type]) : 0, proto_type != 0, bin_prec, EmptySymbol, is_variadic, is_external, loc);
    return ret;
}

struct ast_node *_create_fun_node(struct m_parser *parser, struct ast_node *func_type, struct ast_node *block)
{
    hashtable_set_int(&parser->symbol_2_int_types, func_type->ft->name, TYPE_FUNCTION);
    return function_node_new(func_type, block, block->loc);
}

struct ast_node *_parse_function_with_func_type(struct m_parser *parser,
    struct ast_node *func_type)
{
    assert(parser->curr_token.token_type == TOKEN_ASSIGN);
    parse_next_token(parser);
    struct ast_node *block = _parse_block(parser, func_type, 0, 0);
    if (block) {
        return _create_fun_node(parser, func_type, block);
    }
    return 0;
}

struct ast_node *_parse_var(struct m_parser *parser, struct ast_node *parent, symbol name,
    enum type type, symbol ext_type)
{
    if (parser->curr_token.token_type == TOKEN_ASSIGN)
        parse_next_token(parser); // skip '='
            // token
    struct ast_node *exp = 0;
    struct ast_node *var = var_node_new(name, type, ext_type, 0, !parent, parser->curr_token.loc);
    if (type == TYPE_EXT) {
        exp = _parse_type_value_node(parser, var, ext_type);
    } else {
        exp = parse_exp(parser, var, 0);
    }
    var->var->init_value = exp;
    symboltable_push(&parser->vars, var->var->var_name, var);
    return var;
}

struct ast_node *parse_import(struct m_parser *parser, struct ast_node *parent)
{
    parse_next_token(parser);
    return _parse_func_type(parser, parent, true);
}

struct ast_node *_parse_type(struct m_parser *parser, struct ast_node *parent)
{
    struct source_location loc = parser->curr_token.loc;
    parse_next_token(parser); /*eat type keyword*/
    symbol name = parser->curr_token.symbol_val;
    parser->id_is_var_decl = true;
    parse_next_token(parser); /*pointing to '='*/
    assert(parser->curr_token.token_type == TOKEN_ASSIGN);
    parse_next_token(parser);
    struct ast_node *type = type_node_new(name, 0, loc);
    struct ast_node *body = _parse_block(parser, type, 0, 0);
    type->type_def->body = body;
    assert(body);
    parser->id_is_var_decl = false;
    hashtable_set_int(&parser->symbol_2_int_types, name, TYPE_EXT);
    hashtable_set_p(&parser->ext_types, name, type);
    return type;
}

struct ast_node *_parse_unary(struct m_parser *parser, struct ast_node *parent)
{
    // If the current token is not an operator, it must be a primary expr.
    if ((parser->curr_token.token_type == TOKEN_RANGE)
        || parser->curr_token.token_type == TOKEN_NEWLINE
        || parser->curr_token.token_type == TOKEN_EOF)
        return 0;
    struct source_location loc = parser->curr_token.loc;
    if (parser->curr_token.token_type != TOKEN_OP || parser->curr_token.token_type == TOKEN_COMMA || parser->curr_token.token_type == TOKEN_LPAREN) {
        return _parse_node(parser, parent);
    }
    // If this is a unary operator, read it.
    enum op_code opcode = parser->curr_token.opcode;
    parse_next_token(parser);
    struct ast_node *operand = _parse_unary(parser, parent);
    if (operand) {
        return unary_node_new(opcode, operand, false, loc);
    }
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
struct ast_node *_parse_for(struct m_parser *parser, struct ast_node *parent)
{
    struct source_location loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the for.

    if (parser->curr_token.token_type != TOKEN_IDENT)
        return log_info(ERROR, "expected identifier after for, got %s", token_type_strings[parser->curr_token.token_type]);

    symbol id_symbol = parser->curr_token.symbol_val;
    parse_next_token(parser); // eat identifier.

    if (parser->curr_token.token_type != TOKEN_IN)
        return log_info(ERROR, "expected 'in' after for %s", parser->curr_token.symbol_val);
    parse_next_token(parser); // eat 'in'.

    struct ast_node *start = parse_exp(parser, parent, 0);
    if (start == 0)
        return 0;
    if (parser->curr_token.token_type != TOKEN_RANGE)
        return log_info(ERROR, "expected '..' after for start value got token: %s: %s",
            token_type_strings[parser->curr_token.token_type], parser->curr_token.symbol_val);
    parse_next_token(parser);

    // step or end
    struct ast_node *end_val = parse_exp(parser, parent, 0);
    if (end_val == 0)
        return 0;

    // The step value is optional.
    struct ast_node *step = 0;
    if (parser->curr_token.token_type == TOKEN_RANGE) {
        step = end_val;
        parse_next_token(parser);
        end_val = parse_exp(parser, parent, 0);
        if (end_val == 0)
            return 0;
    } else { // default 1
        step = int_node_new(1, parser->curr_token.loc);
    }
    // convert end variable to a logic
    struct ast_node *id_node = ident_node_new(id_symbol, start->loc);
    struct ast_node *end = binary_node_new(OP_LT, id_node, end_val, end_val->loc);
    struct ast_node *body = _parse_block(parser, parent, 0, 0);
    if (body == 0)
        return 0;
    return for_node_new(id_symbol, start, end, step, body, loc);
}

struct ast_node *_parse_if(struct m_parser *parser, struct ast_node *parent)
{
    struct source_location loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the if.

    // condition.
    struct ast_node *cond = parse_exp(parser, parent, 0);
    if (!cond)
        return 0;
    if (parser->curr_token.token_type == TOKEN_THEN) {
        parse_next_token(parser); // eat the then
    }
    while (parser->curr_token.token_type == TOKEN_NEWLINE)
        parse_next_token(parser);
    struct ast_node *then = parse_exp(parser, parent, 0);
    if (then == 0)
        return 0;

    while (parser->curr_token.token_type == TOKEN_NEWLINE)
        parse_next_token(parser);
    if (parser->curr_token.token_type != TOKEN_ELSE)
        return log_info(ERROR, "expected else, got type: %s", node_type_strings[parser->curr_token.token_type]);

    parse_next_token(parser);

    struct ast_node *else_exp = parse_exp(parser, parent, 0);
    if (!else_exp)
        return 0;

    return if_node_new(cond, then, else_exp, loc);
}

struct ast_node *_parse_block(struct m_parser *parser, struct ast_node *parent, void (*fun)(void *, struct ast_node *), void *jit)
{
    int base_col = parent ? parent->loc.col : 1;
    struct array nodes;
    array_init(&nodes, sizeof(struct ast_node *));
    while (true) {
        assert(parser->curr_token.token_type>=0 && parser->curr_token.token_type <= TOKEN_OP);
        if (exit_block(parser, parent, base_col))
            break;
        while (parser->curr_token.token_type == TOKEN_NEWLINE)
            parse_next_token(parser);
        if (exit_block(parser, parent, base_col))
            break;
        struct ast_node *node = parse_statement(parser, parent);
        if (node) {
            base_col = node->loc.col;
            array_push(&nodes, &node);
            if (fun) {
                (*fun)(jit, node);
            }
        }
        if (parser->curr_token.token_type == TOKEN_EOF)
            break;
    }
    return array_size(&nodes) ? block_node_new(&nodes) : 0;
}

struct ast_node *parse_block(struct m_parser *parser, struct ast_node *parent, void (*fun)(void *, struct ast_node *), void *jit)
{
    parse_next_token(parser); /*start parsing*/
    parser->current_module->block = _parse_block(parser, parent, fun, jit);
    return parser->current_module->block;
}

struct ast_node *parse_file(struct m_parser *parser, const char *file_name)
{
    FILE *file= fopen(file_name, "r");
    if (!file) {
        printf("can't open the file: %s errno: %d\n", file_name, errno);
        return 0;
    }
    const char *mod_name = file_name;
    parser->current_module = module_new(mod_name, file);
    parser->lexer = lexer_new(file, mod_name);
    struct ast_node * ast = parse_block(parser, 0, 0, 0);
    lexer_free(parser->lexer);
    parser->lexer = 0;
    return ast;
}


struct ast_node *parse_file_object(struct m_parser *parser, const char *mod_name, FILE *file)
{
    parser->current_module = module_new(mod_name, file);
    parser->lexer = lexer_new(file, mod_name);
    struct ast_node *ast = parse_block(parser, 0, 0, 0);
    lexer_free(parser->lexer);
    parser->lexer = 0;
    return ast;
}

struct ast_node *parse_string(struct m_parser *parser, const char *mod_name, const char *code)
{
    FILE *file = fmemopen((void *)code, strlen(code), "r");
    struct ast_node *node = parse_file_object(parser, mod_name, file);
    fclose(file);
    return node;
}

struct ast_node *parse_repl(struct m_parser *parser, void (*fun)(void *, struct ast_node *), void *jit)
{
    const char *mod_name = "intepreter_main";
    parser->current_module = module_new(mod_name, stdin);
    parser->lexer = lexer_new(stdin, mod_name);
    return parse_block(parser, 0, fun, jit);
}