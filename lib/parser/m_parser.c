/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language parser
 */
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "clib/hashtable.h"
#include "clib/util.h"
#include "clib/win/libfmemopen.h"
#include "parser/astdump.h"
#include "parser/m_parser.h"
#include "parser/parser.h"

#define exit_block(parser, parent, base_col) (parent && parser->curr_token.loc.col < base_col && (parser->curr_token.token_type != TOKEN_NEWLINE || parser->is_repl))

char op_chars[] = {
    '!',
    '%',
    '&',
    '*',
    '+',
    '-',
    '/',
    '<',
    '=',
    '>',
    '?',
    '@',
    '^',
    '|',
    ':',
};

bool is_op_char(char op)
{
    for (size_t i = 0; i < ARRAY_SIZE(op_chars); i++) {
        if (op_chars[i] == op)
            return true;
    }
    return false;
}

bool IS_OP(struct m_parser *parser)
{
    return parser->curr_token.token_type == TOKEN_SYMBOL && is_op_char(string_get(parser->curr_token.val.symbol_val)[0]);
}

struct op_prec {
    char op[4];
    int prec;
};

struct op_prec _op_preces[] = {
    { "||", 50 },
    { "&&", 50 },
    { "!", 50 },
    { "<", 100 },
    { ">", 100 },
    { "==", 100 },
    { "!=", 100 },
    { "<=", 100 },
    { ">=", 100 },
    { "+", 200 },
    { "-", 200 },
    { "*", 400 },
    { "/", 400 },
    { "%", 400 },
    { "^", 500 },
};
#define MAX_PRECEDENCE 400

int _get_op_precedence(struct m_parser *parser);
struct exp_node *_parse_number(struct m_parser *parser, struct exp_node *parent);
struct exp_node *_parse_parentheses(struct m_parser *parser, struct exp_node *parent);
struct exp_node *_parse_node(struct m_parser *parser, struct exp_node *parent);
struct exp_node *_parse_binary(struct m_parser *parser, struct exp_node *parent, int exp_prec, struct exp_node *lhs);
struct exp_node *_parse_for(struct m_parser *parser, struct exp_node *parent);
struct exp_node *_parse_type(struct m_parser *parser, struct exp_node *parent);
struct exp_node *_parse_if(struct m_parser *parser, struct exp_node *parent);
struct exp_node *_parse_unary(struct m_parser *parser, struct exp_node *parent);
struct exp_node *_parse_prototype(struct m_parser *parser, struct exp_node *parent, bool is_external);
struct exp_node *_parse_var(struct m_parser *parser, struct exp_node *parent, symbol name, enum type type, symbol ext_type);
struct exp_node *_parse_function_with_prototype(struct m_parser *parser, struct prototype_node *prototype);
struct block_node *_parse_block(struct m_parser *parser, struct exp_node *parent, void (*fun)(void *, struct exp_node *), void *jit);

struct op_type {
    symbol op;
    enum type type;
    symbol type_symbol;
    bool success;
};

struct m_parser *g_parser = 0;

const char *get_ctt(struct m_parser *parser)
{
    return token_type_strings[parser->curr_token.token_type];
}

bool _is_exp(struct exp_node *node)
{
    return node->node_type != VAR_NODE && node->node_type != FUNCTION_NODE && node->node_type != PROTOTYPE_NODE;
}

void _log_error(struct m_parser *parser, struct source_loc loc, const char *msg)
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

void _build_op_precs(struct hashtable *op_precs)
{
    for (size_t i = 0; i < ARRAY_SIZE(_op_preces); i++) {
        symbol op = to_symbol(_op_preces[i].op);
        hashtable_set_int(op_precs, op, _op_preces[i].prec);
    }
}

void _set_op_prec(struct hashtable *op_precs, symbol op, int prec)
{
    hashtable_set(op_precs, string_get(op), &prec);
}

int _get_op_prec(struct hashtable *op_precs, symbol op)
{
    if (!op || !hashtable_in_p(op_precs, op))
        return -1;
    return hashtable_get_int(op_precs, op);
}

struct m_parser *m_parser_new(bool is_repl)
{
    struct m_parser *parser = malloc(sizeof(*parser));
    parser->type_of = to_symbol(":");
    parser->assignment = to_symbol("=");
    parser->comma = to_symbol(",");
    parser->binary = to_symbol("binary");
    parser->unary = to_symbol("unary");
    parser->import = to_symbol("import");
    parser->extern_symbol = to_symbol("extern");
    parser->type = to_symbol("type");
    parser->variadic = to_symbol("...");
    parser->lparen = to_symbol("(");
    parser->rparen = to_symbol(")");
    parser->lbracket = to_symbol("[");
    parser->rbracket = to_symbol("]");
    parser->if_symbol = to_symbol("if");
    parser->else_symbol = to_symbol("else");
    parser->then_symbol = to_symbol("then");
    parser->for_symbol = to_symbol("for");
    parser->in_symbol = to_symbol("in");
    parser->range_symbol = to_symbol("..");
    parser->true_symbol = to_symbol("true");
    parser->false_symbol = to_symbol("false");

    parser->plus_op = to_symbol("+");
    parser->minus_op = to_symbol("-");
    parser->multiply_op = to_symbol("*");
    parser->division_op = to_symbol("/");
    parser->modulo_op = to_symbol("%");
    parser->lessthan_op = to_symbol("<");
    parser->greaterthan_op = to_symbol(">");
    parser->lessthanequal_op = to_symbol("<=");
    parser->greaterthanequal_op = to_symbol(">=");
    parser->equal_op = to_symbol("==");
    parser->notequal_op = to_symbol("!=");
    parser->or_op = to_symbol("||");
    parser->and_op = to_symbol("&&");
    parser->not_op = to_symbol("!");

    symboltable_init(&parser->vars);
    queue_init(&parser->queued_tokens, sizeof(struct token));
    hashtable_init(&parser->ext_types);
    hashtable_init_with_value_size(&parser->symbol_2_int_types, sizeof(int), 0);
    for (int i = 0; i < TYPE_TYPES; i++) {
        hashtable_set_int(&parser->symbol_2_int_types, to_symbol(type_strings[i]), i);
    }
    hashtable_init_with_value_size(&parser->op_precs, sizeof(int), 0);
    _build_op_precs(&parser->op_precs);

    struct ast *ast = malloc(sizeof(*ast));
    parser->ast = ast;
    array_init(&parser->ast->modules, sizeof(struct module *));
    parser->allow_id_as_a_func = true;
    parser->id_is_var_decl = false;
    parser->is_repl = is_repl;
    parser->current_module = 0;
    init_token(&parser->curr_token);
    g_parser = parser;

    return parser;
}

void module_free(struct module *module)
{
    destroy_tokenizer(module->tokenizer);
    node_free((struct exp_node *)module->block);
    free(module);
}

void m_parser_free(struct m_parser *parser)
{
    hashtable_deinit(&parser->symbol_2_int_types);
    hashtable_deinit(&parser->ext_types);
    for (size_t i = 0; i < array_size(&parser->ast->modules); i++) {
        struct module *it = *(struct module **)array_get(&parser->ast->modules, i);
        module_free(it);
    }
    free(parser->ast);
    symboltable_deinit(&parser->vars);
    free(parser);
    g_parser = 0;
}

void parse_next_token(struct m_parser *parser)
{
    if (queue_size(&parser->queued_tokens) > 0) {
        // cleanup queued tokens, to redo parsing
        parser->curr_token = *(struct token *)queue_pop(&parser->queued_tokens);
    } else
        parser->curr_token = *get_token(parser->current_module->tokenizer);
}

int _get_op_precedence(struct m_parser *parser)
{
    if (!IS_OP(parser))
        return -1;
    return _get_op_prec(&parser->op_precs, parser->curr_token.val.symbol_val);
}

struct exp_node *_parse_bool_value(struct m_parser *parser, struct exp_node *parent)
{
    struct literal_node *result;
    result = bool_node_new(parent, parser->curr_token.loc,
        parser->curr_token.val.symbol_val == parser->true_symbol ? 1 : 0);
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return (struct exp_node *)result;
}

struct exp_node *_parse_char(struct m_parser *parser, struct exp_node *parent)
{
    struct literal_node *result;
    result = char_node_new(parent, parser->curr_token.loc,
        parser->curr_token.val.char_val);
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return (struct exp_node *)result;
}

struct exp_node *_parse_string(struct m_parser *parser, struct exp_node *parent)
{
    struct literal_node *result;
    result = string_node_new(parent, parser->curr_token.loc,
        string_get(parser->curr_token.val.str_val));
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return (struct exp_node *)result;
}

struct exp_node *_parse_number(struct m_parser *parser, struct exp_node *parent)
{
    struct literal_node *result = 0;
    if (parser->curr_token.token_type == TOKEN_INT)
        result = int_node_new(parent, parser->curr_token.loc,
            parser->curr_token.val.int_val);
    else if (parser->curr_token.token_type == TOKEN_FLOAT)
        result = double_node_new(parent, parser->curr_token.loc,
            parser->curr_token.val.double_val);
    else
        assert(false);
    if (parser->curr_token.token_type != TOKEN_NEWLINE)
        parse_next_token(parser);
    return (struct exp_node *)result;
}

struct exp_node *_parse_parentheses(struct m_parser *parser, struct exp_node *parent)
{
    struct exp_node *v;
    parse_next_token(parser);
    if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->rparen) {
        v = (struct exp_node *)unit_node_new(parent, parser->curr_token.loc);
        parse_next_token(parser);
        return v;
    }
    v = parse_exp(parser, parent, 0);
    if (!v)
        return 0;
    if (parser->curr_token.token_type != TOKEN_SYMBOL || parser->curr_token.val.symbol_val != parser->rparen)
        return (struct exp_node *)log_info(ERROR, "expected ')'");
    parse_next_token(parser);
    return v;
}

struct op_type _parse_op_type(struct m_parser *parser, struct source_loc loc)
{
    struct op_type optype;
    optype.type_symbol = 0;
    optype.op = 0;
    optype.type = TYPE_UNK;
    if (IS_OP(parser)) {
        optype.op = parser->curr_token.val.symbol_val;
    }
    if (optype.op == parser->type_of) {
        // type of definition
        parse_next_token(parser); /* skip ':'*/
        symbol type_symbol = parser->curr_token.val.symbol_val;
        if (!hashtable_in_p(&parser->symbol_2_int_types, type_symbol)) {
            string error;
            string_init_chars(&error, "wrong type: ");
            string_add(&error, parser->curr_token.val.symbol_val);
            _log_error(parser, loc, string_get(&error));
            optype.success = false;
            return optype;
        }
        optype.type = hashtable_get_int(&parser->symbol_2_int_types, type_symbol);
        optype.type_symbol = type_symbol;
        parse_next_token(parser); /*skip type*/
        if (IS_OP(parser))
            optype.op = parser->curr_token.val.symbol_val;
    }
    optype.success = true;
    return optype;
}

struct exp_node *_parse_type_value_node(struct m_parser *parser, struct exp_node *parent, symbol ext_type_symbol)
{
    assert(ext_type_symbol);
    struct type_node *type = (struct type_node *)hashtable_get_p(&parser->ext_types, ext_type_symbol);
    assert(type);
    struct block_node *block = _parse_block(parser, (struct exp_node *)parent, 0, 0);
    if (block) {
        assert(array_size(&type->body->nodes) == array_size(&block->nodes));
        return (struct exp_node *)type_value_node_new(parent, parser->curr_token.loc, block, ext_type_symbol);
    }
    return 0;
}

struct exp_node *_parse_function_app_or_def(struct m_parser *parser, struct exp_node *parent, struct source_loc loc, symbol pid_name, bool is_operator, int precedence)
{
    if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->lparen) {
        parse_next_token(parser); // skip '('
    }
    bool func_definition = false;
    bool is_variadic = false;
    struct array args;
    array_init(&args, sizeof(struct exp_node *));
    // doesn't allow function inside parameter or argument
    // will be enhanced later
    string id_name;
    string_init_chars(&id_name, string_get(pid_name));
    parser->allow_id_as_a_func = false;
    struct type_exp *ret_type = 0;
    if (hashtable_in_p(&parser->ext_types, pid_name)) {
        return _parse_type_value_node(parser, parent, pid_name);
    }

    if (parser->curr_token.token_type != TOKEN_SYMBOL || parser->curr_token.val.symbol_val != parser->rparen) {
        while (true) {
            if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->variadic) {
                is_variadic = true;
                parse_next_token(parser);
            } else {
                struct exp_node *arg = parse_exp(parser, parent, 0);
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
            if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->rparen) {
                parse_next_token(parser);
                struct op_type optype = _parse_op_type(parser, parser->curr_token.loc);
                if (optype.type) {
                    ret_type = (struct type_exp *)create_nullary_type(optype.type, optype.type_symbol);
                }
            }
            if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->assignment) {
                func_definition = true;
                break;
            } else if (parser->curr_token.token_type == TOKEN_NEWLINE || parser->curr_token.token_type == TOKEN_EOF)
                break;
            else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->comma)
                parse_next_token(parser);
        }
    } else {
        /*looks we got (), if next one is = then it's definition*/
        parse_next_token(parser);
        if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->assignment)
            func_definition = true;
    }
    parser->allow_id_as_a_func = true;
    // log_info(DEBUG, "is %s a function def: %d, %d, %zu", string_get(&id_name), func_definition, is_variadic, array_size(&args));
    if (func_definition) {
        ARRAY_FUN_PARAM(fun_params);
        struct var_node fun_param;
        fun_param.base.annotated_type_enum = 0;
        fun_param.base.annotated_type_name = 0;
        for (size_t i = 0; i < array_size(&args); i++) {
            struct ident_node *id = *(struct ident_node **)array_get(&args, i);
            fun_param.base.annotated_type_enum = id->base.annotated_type_enum;
            fun_param.base.annotated_type_name = id->base.annotated_type_name;
            fun_param.var_name = id->name;
            array_push(&fun_params, &fun_param);
        }
        if (is_operator) {
            if (precedence && array_size(&fun_params) != BINARY_PARAM_SIZE) {
                return (struct exp_node *)log_info(ERROR, "precedence only apply for binary operator");
            } else if (array_size(&fun_params) != UNARY_PARAM_SIZE && array_size(&fun_params) != BINARY_PARAM_SIZE)
                return (struct exp_node *)log_info(ERROR, "operator overloading only for unary or binary operator");
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
        struct prototype_node *prototype = prototype_node_new(parent, loc, id_symbol, &fun_params, ret_type,
            is_operator, precedence, is_operator ? id_symbol : EmptySymbol, is_variadic, false);
        return _parse_function_with_prototype(parser, prototype);
    }
    // function application
    symbol name_symbol = string_2_symbol(&id_name);
    struct exp_node *call_node = (struct exp_node *)call_node_new(parent, loc, name_symbol, &args);
    return parse_exp(parser, parent, call_node);
}

struct exp_node *parse_statement(struct m_parser *parser, struct exp_node *parent)
{
    struct exp_node *node = 0;
    struct source_loc loc = parser->curr_token.loc;
    if (parser->curr_token.token_type == TOKEN_EOF)
        return 0;
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->import)
        node = parse_import(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->type)
        node = _parse_type(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->extern_symbol) {
        parse_next_token(parser);
        node = _parse_prototype(parser, parent, true);
    } else if (parser->curr_token.token_type == TOKEN_SYMBOL && (parser->curr_token.val.symbol_val == parser->binary || parser->curr_token.val.symbol_val == parser->unary)) {
        // function def
        struct exp_node *proto = _parse_prototype(parser, parent, false);
        node = _parse_function_with_prototype(parser, (struct prototype_node *)proto);
    } else if (parser->curr_token.token_type == TOKEN_IDENT) {
        symbol id_symbol = parser->curr_token.val.symbol_val;
        struct source_loc current_loc = parser->curr_token.loc;
        parse_next_token(parser); // skip identifier
        struct op_type optype = _parse_op_type(parser, current_loc);
        if (!optype.success)
            return 0;
        if (parser->id_is_var_decl) {
            /*id is var decl*/
            node = (struct exp_node *)var_node_new(parent, current_loc, id_symbol, optype.type, optype.type_symbol, 0);
        } else if (optype.op == parser->assignment || optype.type) { //|| !has_symbol(&parser->vars, id_symbol)
            // variable definition
            node = _parse_var(parser, parent, id_symbol, optype.type, optype.type_symbol);
        } else if (parser->curr_token.token_type == TOKEN_NEWLINE || parser->curr_token.token_type == TOKEN_EOF || _get_op_prec(&parser->op_precs, optype.op) > 0) {
            // just id expression evaluation
            struct exp_node *lhs = (struct exp_node *)ident_node_new(parent, parser->curr_token.loc, id_symbol);
            node = parse_exp(parser, parent, lhs);
        } else {
            // function definition or application
            node = _parse_function_app_or_def(parser, parent, current_loc, id_symbol, false, 0);
        }
    } else {
        if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->lparen) {
            struct array queued;
            array_init(&queued, sizeof(struct token));
            array_push(&queued, &parser->curr_token);
            parse_next_token(parser); // skip (
            array_push(&queued, &parser->curr_token);
            if (IS_OP(parser)) {
                // it is operator overloading
                symbol op = parser->curr_token.val.symbol_val;
                parse_next_token(parser);
                if (parser->curr_token.token_type != TOKEN_SYMBOL || parser->curr_token.val.symbol_val != parser->rparen)
                    return (struct exp_node *)log_info(ERROR, "expected ')'");
                parse_next_token(parser);
                int precedence = 0;
                if (parser->curr_token.token_type == TOKEN_INT) {
                    // precedence: parse binary
                    precedence = parser->curr_token.val.int_val;
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
        node->parent = parent;
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
    return parser->allow_id_as_a_func && (parser->curr_token.token_type == TOKEN_IDENT || parser->curr_token.token_type == TOKEN_INT || parser->curr_token.token_type == TOKEN_FLOAT || (parser->curr_token.token_type == TOKEN_SYMBOL && (parser->curr_token.val.symbol_val == parser->unary || parser->curr_token.val.symbol_val == parser->lparen || parser->curr_token.val.symbol_val == parser->if_symbol)));
}

struct exp_node *_parse_ident(struct m_parser *parser, struct exp_node *parent)
{
    symbol id_symbol = parser->curr_token.val.symbol_val;
    struct source_loc loc = parser->curr_token.loc;

    parse_next_token(parser); // take identifier
    if (_id_is_a_function_call(parser)) {
        struct exp_node *exp = 0;
        if (hashtable_in_p(&parser->ext_types, id_symbol)) {
            exp = _parse_type_value_node(parser, parent, id_symbol);
        } else {
            struct array args;
            array_init(&args, sizeof(struct exp_node *));
            while (true) {
                struct exp_node *arg = parse_exp(parser, parent, 0);
                assert(arg);
                if (!(arg->node_type == LITERAL_NODE && arg->annotated_type_enum == TYPE_UNIT))
                    array_push(&args, &arg);
                if (!_id_is_a_function_call(parser))
                    break;
            }
            parse_next_token(parser);
            exp = (struct exp_node *)call_node_new(parent, loc, id_symbol, &args);
            array_deinit(&args);
        }
        return exp;
    }
    return (struct exp_node *)ident_node_new(parent, loc, id_symbol);
}

struct exp_node *_parse_node(struct m_parser *parser, struct exp_node *parent)
{
    if (parser->curr_token.token_type == TOKEN_IDENT)
        return _parse_ident(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && (parser->curr_token.val.symbol_val == parser->true_symbol || parser->curr_token.val.symbol_val == parser->false_symbol))
        return _parse_bool_value(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_INT || parser->curr_token.token_type == TOKEN_FLOAT)
        return _parse_number(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_CHAR)
        return _parse_char(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_STRING)
        return _parse_string(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->if_symbol)
        return _parse_if(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->for_symbol)
        return _parse_for(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->lparen)
        return _parse_parentheses(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->not_op)
        return _parse_unary(parser, parent);
    else {
        string error;
        string_init_chars(&error, "unknown token: ");
        string_add_chars(&error, token_type_strings[parser->curr_token.token_type]);
        if (IS_OP(parser)) {
            string_add_chars(&error, " op: ");
            string_add(&error, parser->curr_token.val.symbol_val);
        }
        parse_next_token(parser);
        return (struct exp_node *)log_info(ERROR, string_get(&error));
    }
}

struct exp_node *_parse_binary(struct m_parser *parser, struct exp_node *parent, int exp_prec, struct exp_node *lhs)
{
    while (true) {
        if (parser->curr_token.token_type == TOKEN_NEWLINE)
            return lhs;
        int tok_prec = _get_op_precedence(parser);
        if (tok_prec < exp_prec)
            return lhs;
        symbol binary_op = parser->curr_token.val.symbol_val;
        parse_next_token(parser);
        struct exp_node *rhs = _parse_unary(parser, parent);
        if (!rhs)
            return lhs;
        int next_prec = _get_op_precedence(parser);
        if (tok_prec < next_prec) {
            rhs = _parse_binary(parser, parent, tok_prec + 1, rhs);
            if (!rhs)
                return 0;
        }
        lhs = (struct exp_node *)binary_node_new(parent, lhs->loc, binary_op, lhs, rhs);
    }
}

struct exp_node *parse_exp(struct m_parser *parser, struct exp_node *parent, struct exp_node *lhs)
{
    if (parser->curr_token.token_type == TOKEN_NEWLINE)
        return lhs;
    if (!lhs)
        lhs = _parse_unary(parser, parent);
    if (!lhs || parser->curr_token.token_type == TOKEN_NEWLINE)
        return lhs;
    return _parse_binary(parser, parent, 0, lhs);
}

/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
struct exp_node *_parse_prototype(struct m_parser *parser, struct exp_node *parent, bool is_external)
{
    string fun_name;
    struct source_loc loc = parser->curr_token.loc;
    // SourceLocation FnLoc = CurLoc;
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    if (parser->curr_token.token_type == TOKEN_IDENT) {
        string_copy(&fun_name, parser->curr_token.val.symbol_val);
        proto_type = 0;
        parse_next_token(parser);
    } else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->unary) {
        parse_next_token(parser);
        if (!IS_OP(parser))
            return (struct exp_node *)log_info(ERROR, "Expected unary operator");
        string_init_chars(&fun_name, "unary");
        string_add(&fun_name, parser->curr_token.val.symbol_val);
        proto_type = 1;
        parse_next_token(parser);
    } else if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->binary) {
        parse_next_token(parser);
        if (!IS_OP(parser))
            return (struct exp_node *)log_info(ERROR, "Expected binary operator");
        string_init_chars(&fun_name, "binary");
        string_add(&fun_name, parser->curr_token.val.symbol_val);
        proto_type = 2;
        parse_next_token(parser);
        // Read the precedence if present.
        if (parser->curr_token.token_type == TOKEN_INT) {
            if (parser->curr_token.val.int_val < 1 || parser->curr_token.val.int_val > MAX_PRECEDENCE) {
                return (struct exp_node *)log_info(ERROR, "Invalid precedecnce: must be 1..100");
            }
            parse_next_token(parser);
        }
    } else {
        return (struct exp_node *)log_info(ERROR, "Expected function name in prototype");
    }
    bool has_parenthese = parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->lparen;
    if (has_parenthese)
        parse_next_token(parser); // skip '('
    ARRAY_FUN_PARAM(fun_params);
    struct var_node fun_param;
    struct op_type optype;
    while (parser->curr_token.token_type == TOKEN_IDENT) {
        fun_param.var_name = parser->curr_token.val.symbol_val;
        parse_next_token(parser);
        optype = _parse_op_type(parser, parser->curr_token.loc);
        fun_param.base.annotated_type_name = 0;
        fun_param.base.annotated_type_enum = TYPE_UNK;
        fun_param.base.type = 0;
        fun_param.base.annotated_type_name = 0;
        if (optype.success && optype.type) {
            fun_param.base.annotated_type_name = optype.type_symbol;
            fun_param.base.annotated_type_enum = optype.type;
        }
        array_push(&fun_params, &fun_param);
        if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->comma)
            parse_next_token(parser);
    }
    bool is_variadic = parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->variadic;
    if (is_variadic)
        parse_next_token(parser);
    if (has_parenthese && parser->curr_token.val.symbol_val != parser->rparen)
        return (struct exp_node *)log_info(ERROR, "Expected ')' to match '('");
    // success.
    if (has_parenthese)
        parse_next_token(parser); // eat ')'.
    struct type_exp *ret_type = 0;
    optype = _parse_op_type(parser, parser->curr_token.loc);
    if (optype.type)
        ret_type = (struct type_exp *)create_nullary_type(optype.type, optype.type_symbol);
    // Verify right number of names for operator.
    if (proto_type && array_size(&fun_params) != proto_type)
        return (struct exp_node *)log_info(ERROR, "Invalid number of operands for operator");
    symbol fun_name_symbol = string_2_symbol(&fun_name);
    struct exp_node *ret = (struct exp_node *)prototype_node_new(parent, loc, fun_name_symbol, &fun_params,
        ret_type, proto_type != 0, bin_prec, EmptySymbol, is_variadic, is_external);
    return ret;
}

struct exp_node *_create_fun_node(struct m_parser *parser, struct prototype_node *prototype, struct block_node *block)
{
    if (is_binary_op(prototype)) {
        _set_op_prec(&parser->op_precs, prototype->op, prototype->precedence);
    }
    hashtable_set_int(&parser->symbol_2_int_types, prototype->name, TYPE_FUNCTION);
    return (struct exp_node *)function_node_new(prototype, block);
}

struct exp_node *_parse_function_with_prototype(struct m_parser *parser,
    struct prototype_node *prototype)
{
    assert(parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->assignment);
    parse_next_token(parser);
    struct block_node *block = _parse_block(parser, (struct exp_node *)prototype, 0, 0);
    if (block) {
        return _create_fun_node(parser, prototype, block);
    }
    return 0;
}

struct exp_node *_parse_var(struct m_parser *parser, struct exp_node *parent, symbol name,
    enum type type, symbol ext_type)
{
    if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->assignment)
        parse_next_token(parser); // skip '='
            // token
    struct exp_node *exp = 0;
    struct var_node *var = (struct var_node *)var_node_new(parent, parser->curr_token.loc, name, type, ext_type, 0);
    if (type == TYPE_EXT) {
        exp = _parse_type_value_node(parser, &var->base, ext_type);
    } else {
        exp = parse_exp(parser, (struct exp_node *)var, 0);
    }
    var->init_value = exp;
    symboltable_push(&parser->vars, var->var_name, var);
    return (struct exp_node *)var;
}

struct exp_node *parse_exp_to_function(struct m_parser *parser, struct exp_node *exp, symbol fn)
{
    if (!exp)
        exp = parse_exp(parser, 0, 0);
    if (exp) {
        ARRAY_FUN_PARAM(fun_params);
        struct prototype_node *prototype = prototype_node_default_new(0, exp->loc, fn, &fun_params, 0, false, false);
        struct array nodes;
        array_init(&nodes, sizeof(struct exp_node *));
        array_push(&nodes, &exp);
        struct block_node *block = block_node_new((struct exp_node *)prototype, &nodes);
        return _create_fun_node(parser, prototype, block);
    }
    return 0;
}

struct exp_node *parse_import(struct m_parser *parser, struct exp_node *parent)
{
    parse_next_token(parser);
    return _parse_prototype(parser, parent, true);
}

struct exp_node *_parse_type(struct m_parser *parser, struct exp_node *parent)
{
    struct source_loc loc = parser->curr_token.loc;
    parse_next_token(parser); /*eat type keyword*/
    symbol name = parser->curr_token.val.symbol_val;
    parser->id_is_var_decl = true;
    parse_next_token(parser); /*pointing to '='*/
    assert(parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->assignment);
    parse_next_token(parser);
    struct type_node *type = type_node_new(parent, loc, name, 0);
    struct block_node *body = _parse_block(parser, &type->base, 0, 0);
    type->body = body;
    assert(body);
    parser->id_is_var_decl = false;
    hashtable_set_int(&parser->symbol_2_int_types, name, TYPE_EXT);
    hashtable_set_p(&parser->ext_types, name, type);
    return (struct exp_node *)type;
}

struct exp_node *_parse_unary(struct m_parser *parser, struct exp_node *parent)
{
    // If the current token is not an operator, it must be a primary expr.
    if ((parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->range_symbol)
        || parser->curr_token.token_type == TOKEN_NEWLINE
        || parser->curr_token.token_type == TOKEN_EOF)
        return 0;
    struct source_loc loc = parser->curr_token.loc;
    if (!IS_OP(parser) || (parser->curr_token.token_type == TOKEN_SYMBOL && (parser->curr_token.val.symbol_val == parser->lparen || parser->curr_token.val.symbol_val == parser->comma))) {
        return _parse_node(parser, parent);
    }
    // If this is a unary operator, read it.
    symbol op = parser->curr_token.val.symbol_val;
    parse_next_token(parser);
    struct exp_node *operand = _parse_unary(parser, parent);
    if (operand) {
        return (struct exp_node *)unary_node_new(parent, loc, op, operand);
    }
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
struct exp_node *_parse_for(struct m_parser *parser, struct exp_node *parent)
{
    struct source_loc loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the for.

    if (parser->curr_token.token_type != TOKEN_IDENT)
        return (struct exp_node *)log_info(ERROR, "expected identifier after for, got %s", token_type_strings[parser->curr_token.token_type]);

    symbol id_symbol = parser->curr_token.val.symbol_val;
    parse_next_token(parser); // eat identifier.

    if (parser->curr_token.token_type != TOKEN_SYMBOL || parser->curr_token.val.symbol_val != parser->in_symbol)
        return (struct exp_node *)log_info(ERROR, "expected 'in' after for %s", parser->curr_token.val.symbol_val);
    parse_next_token(parser); // eat 'in'.

    struct exp_node *start = parse_exp(parser, parent, 0);
    if (start == 0)
        return 0;
    if (parser->curr_token.token_type != TOKEN_SYMBOL || parser->curr_token.val.symbol_val != parser->range_symbol)
        return (struct exp_node *)log_info(ERROR, "expected '..' after for start value got token: %s: %s",
            token_type_strings[parser->curr_token.token_type], parser->curr_token.val.symbol_val);
    parse_next_token(parser);

    // step or end
    struct exp_node *end_val = parse_exp(parser, parent, 0);
    if (end_val == 0)
        return 0;

    // The step value is optional.
    struct exp_node *step = 0;
    if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->range_symbol) {
        step = end_val;
        parse_next_token(parser);
        end_val = parse_exp(parser, parent, 0);
        if (end_val == 0)
            return 0;
    } else { // default 1
        step = (struct exp_node *)int_node_new(parent, parser->curr_token.loc, 1);
    }
    // convert end variable to a logic
    struct exp_node *id_node = (struct exp_node *)ident_node_new(parent, start->loc, id_symbol);
    struct exp_node *end = (struct exp_node *)binary_node_new(parent, end_val->loc, parser->lessthan_op, id_node, end_val);
    struct block_node *body = _parse_block(parser, parent, 0, 0);
    if (body == 0)
        return 0;
    return (struct exp_node *)for_node_new(parent, loc, id_symbol, start, end, step, (struct exp_node *)body);
}

struct exp_node *_parse_if(struct m_parser *parser, struct exp_node *parent)
{
    struct source_loc loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the if.

    // condition.
    struct exp_node *cond = parse_exp(parser, parent, 0);
    if (!cond)
        return 0;
    if (parser->curr_token.token_type == TOKEN_SYMBOL && parser->curr_token.val.symbol_val == parser->then_symbol) {
        parse_next_token(parser); // eat the then
    }
    while (parser->curr_token.token_type == TOKEN_NEWLINE)
        parse_next_token(parser);
    struct exp_node *then = parse_exp(parser, parent, 0);
    if (then == 0)
        return 0;

    while (parser->curr_token.token_type == TOKEN_NEWLINE)
        parse_next_token(parser);
    if (parser->curr_token.token_type != TOKEN_SYMBOL || parser->curr_token.val.symbol_val != parser->else_symbol)
        return (struct exp_node *)log_info(ERROR, "expected else, got type: %s", node_type_strings[parser->curr_token.token_type]);

    parse_next_token(parser);

    struct exp_node *else_exp = parse_exp(parser, parent, 0);
    if (!else_exp)
        return 0;

    return (struct exp_node *)if_node_new(parent, loc, cond, then, else_exp);
}

struct block_node *_parse_block(struct m_parser *parser, struct exp_node *parent, void (*fun)(void *, struct exp_node *), void *jit)
{
    int base_col = parent ? parent->loc.col : 1;
    struct array nodes;
    array_init(&nodes, sizeof(struct exp_node *));
    while (true) {
        assert(parser->curr_token.token_type && parser->curr_token.token_type < TOKEN_TOTAL);
        if (exit_block(parser, parent, base_col))
            break;
        while (parser->curr_token.token_type == TOKEN_NEWLINE)
            parse_next_token(parser);
        if (exit_block(parser, parent, base_col))
            break;
        struct exp_node *node = parse_statement(parser, parent);
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
    return array_size(&nodes) ? block_node_new(parent, &nodes) : 0;
}

struct block_node *parse_block(struct m_parser *parser, struct exp_node *parent, void (*fun)(void *, struct exp_node *), void *jit)
{
    parse_next_token(parser); /*start parsing*/
    parser->current_module->block = _parse_block(parser, parent, fun, jit);
    return parser->current_module->block;
}

struct block_node *parse_file(struct m_parser *parser, const char *file_name)
{
    FILE *file= fopen(file_name, "r");
    if (!file) {
        printf("can't open the file: %s errno: %d\n", file_name, errno);
        return 0;
    }
    const char *mod_name = file_name;
    parser->current_module = module_new(mod_name, file);
    array_push(&parser->ast->modules, &parser->current_module);
    return parse_block(parser, 0, 0, 0);
}

struct block_node *parse_file_object(struct m_parser *parser, const char *mod_name, FILE *file)
{
    parser->current_module = module_new(mod_name, file);
    array_push(&parser->ast->modules, &parser->current_module);
    return parse_block(parser, 0, 0, 0);
}

struct block_node *parse_string(struct m_parser *parser, const char *mod_name, const char *code)
{
    FILE *file = fmemopen((void *)code, strlen(code), "r");
    struct block_node *node = parse_file_object(parser, mod_name, file);
    fclose(file);
    parser->current_module->tokenizer->file = 0;
    return node;
}

struct block_node *parse_repl(struct m_parser *parser, void (*fun)(void *, struct exp_node *), void *jit)
{
    const char *mod_name = "intepreter_main";
    parser->current_module = module_new(mod_name, stdin);
    array_push(&parser->ast->modules, &parser->current_module);
    return parse_block(parser, 0, fun, jit);
}

enum type get_type_enum(symbol type_symbol)
{
    assert(g_parser);
    return hashtable_get_int(&g_parser->symbol_2_int_types, type_symbol);
}

symbol get_type_symbol(enum type type_enum)
{
    //assert(g_parser);
    return to_symbol(type_strings[type_enum]);
}
