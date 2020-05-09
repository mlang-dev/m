/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language parser
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "astdump.h"
#include "builtins.h"
#include "clib/hashtable.h"
#include "clib/util.h"
#include "parser.h"

#define exit_block(parser, parent, col) (parent && parser->curr_token.loc.col < col && (parser->curr_token.token_type != TOKEN_EOS || parser->is_repl))

struct op_prec {
    char op[4];
    unsigned int prec;
};

struct op_prec _op_preces[] = {
    { "<", 10 }, { ">", 10 }, { "==", 10 }, { "!=", 10 }, { "<=", 10 }, { ">=", 10 },
    { "+", 20 }, { "-", 20 },
    { "*", 40 }, { "/", 40 }
};

int _get_op_precedence(struct parser* parser);
struct exp_node* _parse_number(struct parser* parser, struct exp_node* parent);
struct exp_node* _parse_parentheses(struct parser* parser, struct exp_node* parent);
struct exp_node* _parse_node(struct parser* parser, struct exp_node* parent);
struct exp_node* _parse_binary(struct parser* parser, struct exp_node* parent, int exp_prec, struct exp_node* lhs);
struct exp_node* _parse_for(struct parser* parser, struct exp_node* parent);
struct exp_node* _parse_if(struct parser* parser, struct exp_node* parent);
struct exp_node* _parse_unary(struct parser* parser, struct exp_node* parent);
struct exp_node* _parse_prototype(struct parser* parser, struct exp_node* parent);
struct exp_node* _parse_var(struct parser* parser, struct exp_node* parent, const char* name);
struct exp_node* _parse_function_with_prototype(struct parser* parser, struct prototype_node* prototype);

const char* get_ctt(struct parser* parser)
{
    return token_type_strings[parser->curr_token.token_type];
}

bool _is_exp(struct exp_node* node)
{
    return node->node_type != VAR_NODE && node->node_type != FUNCTION_NODE && node->node_type != PROTOTYPE_NODE;
}

void queue_token(struct parser* psr, struct token tkn)
{
    queue_push(&psr->queued_tokens, &tkn);
}

void queue_tokens(struct parser* psr, struct array* tokens)
{
    for (size_t i = 0; i < array_size(tokens); i++) {
        struct token* tok = (struct token*)array_get(tokens, i);
        queue_push(&psr->queued_tokens, tok);
    }
}

void _build_op_precs(struct hashtable* op_precs)
{
    for (size_t i = 0; i < ARRAY_SIZE(_op_preces); i++) {
        hashtable_set(op_precs, _op_preces[i].op, &_op_preces[i].prec);
    }
}

void _set_op_prec(struct hashtable* op_precs, const char* op, unsigned int prec)
{
    hashtable_set(op_precs, op, &prec);
}

int _get_op_prec(struct hashtable* op_precs, const char* op)
{
    if (!op || !op[0])
        return -1;
    unsigned int* prec = (unsigned int*)hashtable_get(op_precs, op);
    if (prec)
        return *prec;
    return -1;
}

struct parser* parser_new(const char* file_name, bool is_repl, FILE* (*open_file)(const char* file_name))
{
    FILE* file;
    if (open_file)
        file = open_file(file_name);
    else
        file = file_name ? fopen(file_name, "r") : stdin;
    const char* mod_name = file_name ? file_name : "intepreter_main";
    struct parser* psr = malloc(sizeof(*psr));
    queue_init(&psr->queued_tokens, sizeof(struct token));
    hashtable_init(&psr->op_precs);
    _build_op_precs(&psr->op_precs);

    struct ast* ast = malloc(sizeof(*ast));
    psr->ast = ast;
    array_init(&psr->ast->modules, sizeof(struct module*));
    psr->allow_id_as_a_func = true;
    psr->is_repl = is_repl;
    psr->current_module = create_module(mod_name, file);
    array_push(&psr->ast->modules, &psr->current_module);
    return psr;
}

void block_deinit(struct block_node* block)
{
    if (!block)
        return;
    // for(struct exp_node *node : block->nodes){

    // }
}

void destroy_module(struct module* module)
{
    destroy_tokenizer(module->tokenizer);
    block_deinit(module->block);
    free(module->block);
    string_deinit(&module->name);
    free(module);
}

void parser_free(struct parser* parser)
{
    for (size_t i = 0; i < array_size(&parser->ast->modules); i++) {
        struct module* it = *(struct module**)array_get(&parser->ast->modules, i);
        destroy_module(it);
    }
    free(parser->ast);
    free(parser);
}

void parse_next_token(struct parser* parser)
{
    if (queue_size(&parser->queued_tokens) > 0) {
        // cleanup queued tokens, to redo parsing
        parser->curr_token = *(struct token*)queue_pop(&parser->queued_tokens);
        //log_info(DEBUG, "using queued tokens !");
    } else
        parser->curr_token = *get_token(parser->current_module->tokenizer);
}

int _get_op_precedence(struct parser* parser)
{
    if (parser->curr_token.token_type != TOKEN_OP)
        return -1;
    const char* op = string_get(parser->curr_token.ident_str);
    return _get_op_prec(&parser->op_precs, op);
}

struct exp_node* _parse_bool_value(struct parser* parser, struct exp_node* parent)
{
    struct num_node* result;
    result = create_bool_node(parent, parser->curr_token.loc,
        parser->curr_token.int_val);
    if (parser->curr_token.token_type != TOKEN_EOS)
        parse_next_token(parser);
    return (struct exp_node*)result;
}

struct exp_node* _parse_number(struct parser* parser, struct exp_node* parent)
{
    struct num_node* result;
    if (parser->curr_token.type == TYPE_INT)
        result = create_int_node(parent, parser->curr_token.loc,
            parser->curr_token.int_val);
    else if (parser->curr_token.type == TYPE_DOUBLE)
        result = create_double_node(parent, parser->curr_token.loc,
            parser->curr_token.double_val);
    else
        assert(false);
    if (parser->curr_token.token_type != TOKEN_EOS)
        parse_next_token(parser);
    return (struct exp_node*)result;
}

struct exp_node* _parse_parentheses(struct parser* parser, struct exp_node* parent)
{
    parse_next_token(parser);
    struct exp_node* v = parse_exp(parser, parent, 0);
    if (!v)
        return 0;
    if (parser->curr_token.token_type != TOKEN_RPAREN)
        return (struct exp_node*)log_info(ERROR, "expected ')'");
    parse_next_token(parser);
    return v;
}

struct exp_node* _parse_function_app_or_def(struct parser* parser, struct exp_node* parent, struct source_loc loc, const char* pid_name, bool is_operator, int precedence)
{
    if (parser->curr_token.token_type == TOKEN_LPAREN)
        parse_next_token(parser); // skip '('
    bool func_definition = false;
    struct array args;
    array_init(&args, sizeof(struct exp_node*));
    //doesn't allow function inside parameter or argument
    //will be enhanced later
    //log_info(DEBUG, "function app or def: %s", id_name.c_str());
    string id_name;
    string_init_chars(&id_name, pid_name);
    parser->allow_id_as_a_func = false;
    if (parser->curr_token.token_type != TOKEN_RPAREN) {
        while (true) {
            struct exp_node* arg = parse_exp(parser, parent, 0);
            if (arg) {
                array_push(&args, &arg);
            }
            if (string_eq_chars(parser->curr_token.ident_str, "=")) {
                func_definition = true;
                break;
            } else if (parser->curr_token.token_type == TOKEN_RPAREN || parser->curr_token.token_type == TOKEN_EOS || parser->curr_token.token_type == TOKEN_EOF)
                break;
            else if (string_eq_chars(parser->curr_token.ident_str, ","))
                parse_next_token(parser);
        }
    }
    parser->allow_id_as_a_func = true;
    //log_info(DEBUG, "is %s a function def: %d, %d", id_name.c_str(), func_definition, is_operator);
    if (func_definition) {
        ARRAY_FUN_PARAM(fun_params);
        struct var_node fun_param;
        for (size_t i = 0; i < array_size(&args); i++) {
            struct ident_node* id = *(struct ident_node**)array_get(&args, i);
            string_copy(&fun_param.var_name, &id->name);
            array_push(&fun_params, &fun_param);
        }
        if (is_operator) {
            if (precedence && array_size(&fun_params) != BINARY_PARAM_SIZE) {
                return (struct exp_node*)log_info(ERROR, "precedence only apply for binary operator");
            } else if (array_size(&fun_params) != UNARY_PARAM_SIZE && array_size(&fun_params) != BINARY_PARAM_SIZE)
                return (struct exp_node*)log_info(ERROR, "operator overloading only for unary or binary operator");
            if (array_size(&fun_params) == UNARY_PARAM_SIZE) {
                string_copy_chars(&id_name, "unary");
                string_add_chars(&id_name, pid_name);
                //id_name = "unary" + id_name;
            } else {
                string_copy_chars(&id_name, "binary");
                string_add_chars(&id_name, pid_name);
            }
        }
        struct prototype_node* prototype = create_prototype_node(parent, loc, string_get(&id_name), &fun_params, 0,
        is_operator, precedence, is_operator ? string_get(&id_name) : "");
        //log_info(DEBUG, "prototype: %s", id_name.c_str());
        return _parse_function_with_prototype(parser, prototype);
        //log_info(DEBUG, "func: %s", id_name.c_str());
        //array_deinit(&argNames);
    }
    // function application
    //log_info(DEBUG, "function application: %s", id_name.c_str());
    struct exp_node* call_node = (struct exp_node*)create_call_node(parent, loc, string_get(&id_name), &args);
    return parse_exp(parser, parent, call_node);
}

struct exp_node* parse_statement(struct parser* parser, struct exp_node* parent)
{
    //log_info(DEBUG, "parsing statement:%d, %s", parent, token_type_strings[parser->curr_token.token_type]);
    struct exp_node* node;
    struct source_loc loc = parser->curr_token.loc;
    if (parser->curr_token.token_type == TOKEN_EOF)
        return 0;
    else if (parser->curr_token.token_type == TOKEN_IMPORT)
        node = parse_import(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_UNARY || parser->curr_token.token_type == TOKEN_BINARY) {
        //function def
        struct exp_node* proto = _parse_prototype(parser, parent);
        node = _parse_function_with_prototype(parser, (struct prototype_node*)proto);
    } else if (parser->curr_token.token_type == TOKEN_IDENT) {
        string id_name;
        string_copy(&id_name, parser->curr_token.ident_str);
        struct source_loc loc = parser->curr_token.loc;
        parse_next_token(parser); // skip identifier
        string op;
        string_init(&op);
        if (parser->curr_token.token_type == TOKEN_OP)
            string_copy(&op, parser->curr_token.ident_str);
        //log_info(DEBUG, "id token: %s, %s, %d", id_name.c_str(), op.c_str(), parent);
        if (string_eq_chars(&op, "=")) {
            // variable definition
            node = _parse_var(parser, parent, string_get(&id_name));
        } else if (parser->curr_token.token_type == TOKEN_EOS || parser->curr_token.token_type == TOKEN_EOF || _get_op_prec(&parser->op_precs, string_get(&op)) > 0) {
            // just id expression evaluation
            struct exp_node* lhs = (struct exp_node*)create_ident_node(parent, parser->curr_token.loc, string_get(&id_name));
            node = parse_exp(parser, parent, lhs);
            //log_info(DEBUG, "parsed exp: id exp: %d", node->node_type);
        } else {
            // function definition or application
            node = _parse_function_app_or_def(parser, parent, loc, string_get(&id_name), false, 0);
        }
    } else {
        if (parser->curr_token.token_type == TOKEN_LPAREN) {
            struct array queued;
            array_init(&queued, sizeof(struct token));
            array_push(&queued, &parser->curr_token);
            parse_next_token(parser); //skip (
            array_push(&queued, &parser->curr_token);
            if (parser->curr_token.token_type == TOKEN_OP) { // && op_chars.count(string_get(parser->curr_token.ident_str)[0])
                //it is operator overloading
                //log_info(DEBUG, "it is operator overloading: %c: loc: %d, %d", parser->curr_token.op_val, parser->curr_token.loc.line, parser->curr_token.loc.col);
                string op;
                string_copy(&op, parser->curr_token.ident_str);
                parse_next_token(parser);
                if (parser->curr_token.token_type != TOKEN_RPAREN)
                    return (struct exp_node*)log_info(ERROR, "expected ')'");
                parse_next_token(parser);
                int precedence = 0;
                if (parser->curr_token.token_type == TOKEN_NUM) {
                    //precedence: parse binary
                    precedence = parser->curr_token.double_val;
                    parse_next_token(parser); //skip it
                    //log_info(DEBUG, "got precedence: %d", precedence);
                }
                node = _parse_function_app_or_def(parser, parent, loc, string_get(&op), true, precedence);
            } else { //normal exp
                queue_tokens(parser, &queued);
                parse_next_token(parser); //retrieving (
                //log_info(DEBUG, "normal exp: %c", parser->curr_token.op_val);
                node = parse_exp(parser, parent, 0);
            }
            array_deinit(&queued);
        } else {
            //log_info(DEBUG, "starting to parse exp: %s", token_type_strings[parser->curr_token.token_type]);
            node = parse_exp(parser, parent, 0);
            //log_info(DEBUG, "it's exp: %s", node_type_strings[node->node_type]);
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

bool _id_is_a_function_call(struct parser* parser)
{
    return parser->allow_id_as_a_func && (parser->curr_token.token_type == TOKEN_IDENT || parser->curr_token.token_type == TOKEN_NUM || parser->curr_token.token_type == TOKEN_IF || parser->curr_token.token_type == TOKEN_UNARY || parser->curr_token.token_type == TOKEN_LPAREN);
}

struct exp_node* _parse_ident(struct parser* parser, struct exp_node* parent)
{
    string id_name;
    string_copy(&id_name, parser->curr_token.ident_str);
    struct source_loc loc = parser->curr_token.loc;

    parse_next_token(parser); // take identifier
    //log_info(DEBUG, "parsed id: %s, curtoken: %s", id_name.c_str(), token_type_strings[parser->curr_token.token_type]);
    if (_id_is_a_function_call(parser)) { // pure variable
        // fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
        //(
        struct array args;
        array_init(&args, sizeof(struct exp_node*));
        while (true) {
            struct exp_node* arg = parse_exp(parser, parent, 0);
            if (arg)
                array_push(&args, &arg);
            else
                return 0;
            if (!_id_is_a_function_call(parser))
                break;

            parse_next_token(parser);
        }
        parse_next_token(parser);
        struct exp_node* exp = (struct exp_node*)create_call_node(parent, loc, string_get(&id_name), &args);
        array_deinit(&args);
        return exp;
    }
    return (struct exp_node*)create_ident_node(parent, parser->curr_token.loc, string_get(&id_name));
}

struct exp_node* _parse_node(struct parser* parser, struct exp_node* parent)
{
    if (parser->curr_token.token_type == TOKEN_IDENT)
        return _parse_ident(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_TRUE || parser->curr_token.token_type == TOKEN_FALSE)
        return _parse_bool_value(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_NUM)
        return _parse_number(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_IF)
        return _parse_if(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_FOR)
        return _parse_for(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_LPAREN)
        return _parse_parentheses(parser, parent);
    else {
        string error;
        string_init_chars(&error, "unknown token: ");
        string_add_chars(&error, token_type_strings[parser->curr_token.token_type]);
        if (parser->curr_token.token_type == TOKEN_OP) {
            string_add_chars(&error, " op: ");
            string_add(&error, parser->curr_token.ident_str);
        }
        return (struct exp_node*)log_info(ERROR, string_get(&error));
    }
}

struct exp_node* _parse_binary(struct parser* parser, struct exp_node* parent, int exp_prec, struct exp_node* lhs)
{
    while (true) {
        if (parser->curr_token.token_type == TOKEN_EOS)
            return lhs;
        int tok_prec = _get_op_precedence(parser);
        if (tok_prec < exp_prec)
            return lhs;
        //log_info(DEBUG, "bin exp: [%s, %c], %d, %s", token_type_strings[parser->curr_token.token_type], parser->curr_token.op_val, tok_prec, map_to_string(g_op_precedences).c_str());
        string binary_op;
        string_copy(&binary_op, parser->curr_token.ident_str);
        parse_next_token(parser);
        struct exp_node* rhs = _parse_unary(parser, parent);
        if (!rhs)
            return lhs;
        //log_info(DEBUG, "bin exp: rhs: %s", node_type_strings[rhs->node_type]);
        int next_prec = _get_op_precedence(parser);
        if (tok_prec < next_prec) {
            //log_info(DEBUG, "right first %s, %d, %d", token_type_strings[parser->curr_token.token_type], tok_prec, next_prec);
            rhs = _parse_binary(parser, parent, tok_prec + 1, rhs);
            if (!rhs)
                return 0;
        }
        //log_info(DEBUG, "left first: %s, %d, %d", token_type_strings[parser->curr_token.token_type], tok_prec, next_prec);
        lhs = (struct exp_node*)create_binary_node(parent, lhs->loc, string_get(&binary_op), lhs, rhs);
    }
}

struct exp_node* parse_exp(struct parser* parser, struct exp_node* parent, struct exp_node* lhs)
{
    if (parser->curr_token.token_type == TOKEN_EOS)
        return lhs;
    if (!lhs)
        lhs = _parse_unary(parser, parent);
    //log_info(DEBUG, "got lhs: %s, %s", node_type_strings[lhs->node_type], parser->curr_token.ident_str->c_str());
    if (!lhs || parser->curr_token.token_type == TOKEN_EOS)
        return lhs;
    return _parse_binary(parser, parent, 0, lhs);
}

/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
struct exp_node* _parse_prototype(struct parser* parser, struct exp_node* parent)
{
    string fun_name;
    struct source_loc loc = parser->curr_token.loc;
    // SourceLocation FnLoc = CurLoc;
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    switch (parser->curr_token.token_type) {
    case TOKEN_IDENT:
        string_copy(&fun_name, parser->curr_token.ident_str);
        proto_type = 0;
        // fprintf(stderr, "ident token in parse prototype: %s\n",
        // fun_name.c_str());
        parse_next_token(parser);
        break;
    case TOKEN_UNARY:
        parse_next_token(parser);
        if (parser->curr_token.token_type != TOKEN_OP)
            return (struct exp_node*)log_info(ERROR, "Expected unary operator");
        string_init_chars(&fun_name, "unary");
        string_add(&fun_name, parser->curr_token.ident_str);
        //log_info(DEBUG, "finding unary operator: %s", fun_name.c_str());
        proto_type = 1;
        parse_next_token(parser);
        break;
    case TOKEN_BINARY:
        parse_next_token(parser);
        if (parser->curr_token.token_type != TOKEN_OP)
            return (struct exp_node*)log_info(ERROR, "Expected binary operator");
        string_init_chars(&fun_name, "binary");
        string_add(&fun_name, parser->curr_token.ident_str);
        proto_type = 2;
        parse_next_token(parser);
        // Read the precedence if present.
        if (parser->curr_token.token_type == TOKEN_NUM) {
            if (parser->curr_token.double_val < 1 || parser->curr_token.double_val > 100)
                return (struct exp_node*)log_info(ERROR, "Invalid precedecnce: must be 1..100");
            bin_prec = (unsigned)parser->curr_token.double_val;
            //log_info(DEBUG, "finding binary operator: %s, prec: %d", fun_name.c_str(), bin_prec);
            parse_next_token(parser);
        }
        break;
    default:
        return (struct exp_node*)log_info(ERROR, "Expected function name in prototype");
    }
    bool has_parenthese = parser->curr_token.token_type == TOKEN_LPAREN;
    if (has_parenthese)
        parse_next_token(parser); // skip '('
    ARRAY_FUN_PARAM(fun_params);
    struct var_node fun_param;
    while (parser->curr_token.token_type == TOKEN_IDENT) {
        // fprintf(stderr, "arg names: %s",
        // (*parser->curr_token.ident_str).c_str());
        string_copy(&fun_param.var_name, parser->curr_token.ident_str);
        array_push(&fun_params, &fun_param);
        parse_next_token(parser);
    }
    if (has_parenthese && parser->curr_token.token_type != TOKEN_RPAREN)
        return (struct exp_node*)log_info(ERROR, "Expected ')' to match '('");
    // success.
    if (has_parenthese)
        parse_next_token(parser); // eat ')'.
    // Verify right number of names for operator.
    if (proto_type && array_size(&fun_params) != proto_type)
        return (struct exp_node*)log_info(ERROR, "Invalid number of operands for operator");
    struct exp_node* ret = (struct exp_node*)create_prototype_node(parent, loc, string_get(&fun_name), &fun_params, 
        0, proto_type != 0, bin_prec, "");
    //array_deinit(&arg_names);
    return ret;
}

struct exp_node* _create_fun_node(struct parser* parser, struct prototype_node* prototype, struct block_node* block)
{
    if (is_binary_op(prototype)) {
        _set_op_prec(&parser->op_precs, string_get(&prototype->op), prototype->precedence);
    }
    return (struct exp_node*)create_function_node(prototype, block);
}

struct exp_node* _parse_function_with_prototype(struct parser* parser,
    struct prototype_node* prototype)
{
    struct block_node* block = parse_block(parser, (struct exp_node*)prototype, 0, 0);
    if (block) {
        return _create_fun_node(parser, prototype, block);
    }
    return 0;
}

struct exp_node* _parse_var(struct parser* parser, struct exp_node* parent, const char* name)
{
    if (string_eq_chars(parser->curr_token.ident_str, "="))
        parse_next_token(parser); // skip '='
            // token
    struct exp_node* exp = parse_exp(parser, parent, 0);
    if (exp) {
        // not a function but a value
        //log_info(INFO, "_parse_variable:  %lu!", var_names.size());
        return (struct exp_node*)create_var_node(parent, parser->curr_token.loc, name, exp);
    }
    return 0;
}

struct exp_node* parse_exp_to_function(struct parser* parser, struct exp_node* exp, const char* fn)
{
    if (!exp)
        exp = parse_exp(parser, 0, 0);
    if (exp) {
        ARRAY_FUN_PARAM(fun_params);
        struct prototype_node* prototype = create_prototype_node_default(0, exp->loc, fn, &fun_params, 0);
        struct array nodes;
        array_init(&nodes, sizeof(struct exp_node*));
        array_push(&nodes, &exp);
        struct block_node* block = create_block_node((struct exp_node*)prototype, &nodes);
        return _create_fun_node(parser, prototype, block);
    }
    return 0;
}

struct exp_node* parse_import(struct parser* parser, struct exp_node* parent)
{
    parse_next_token(parser);
    return _parse_prototype(parser, parent);
}

/// unary
///   ::= primary
///   ::= '!' unary
struct exp_node* _parse_unary(struct parser* parser, struct exp_node* parent)
{
    // If the current token is not an operator, it must be a primary expr.
    if (parser->curr_token.token_type == TOKEN_RANGE
        || parser->curr_token.token_type == TOKEN_EOS
        || parser->curr_token.token_type == TOKEN_EOF)
        return 0;
    struct source_loc loc = parser->curr_token.loc;
    if (parser->curr_token.token_type != TOKEN_OP || parser->curr_token.token_type == TOKEN_LPAREN
        || string_eq_chars(parser->curr_token.ident_str, ",")) {
        return _parse_node(parser, parent);
    }
    //log_info(DEBUG, "unary: %c", parser->curr_token.op_val);
    // If this is a unary operator, read it.
    string opc;
    string_copy(&opc, parser->curr_token.ident_str);
    parse_next_token(parser);
    struct exp_node* operand = _parse_unary(parser, parent);
    if (operand) {
        //log_info(DEBUG, "unary node:%c: %s", opc, node_type_strings[operand->node_type]);
        return (struct exp_node*)create_unary_node(parent, loc, string_get(&opc), operand);
    }
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
struct exp_node* _parse_for(struct parser* parser, struct exp_node* parent)
{
    struct source_loc loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the for.

    if (parser->curr_token.token_type != TOKEN_IDENT)
        return (struct exp_node*)log_info(ERROR, "expected identifier after for, got %s", token_type_strings[parser->curr_token.token_type]);

    string id_name;
    string_copy(&id_name, parser->curr_token.ident_str);
    parse_next_token(parser); // eat identifier.

    if (parser->curr_token.token_type != TOKEN_IN)
        return (struct exp_node*)log_info(ERROR, "expected 'in' after for %s", parser->curr_token.ident_str);
    parse_next_token(parser); // eat 'in'.

    struct exp_node* start = parse_exp(parser, parent, 0);
    if (start == 0)
        return 0;
    if (parser->curr_token.token_type != TOKEN_RANGE)
        return (struct exp_node*)log_info(ERROR, "expected '..' after for start value got token: %s: %s",
            token_type_strings[parser->curr_token.token_type], parser->curr_token.ident_str);
    parse_next_token(parser);

    //step or end
    struct exp_node* end_val = parse_exp(parser, parent, 0);
    if (end_val == 0)
        return 0;

    // The step value is optional.
    struct exp_node* step = 0;
    if (parser->curr_token.token_type == TOKEN_RANGE) {
        step = end_val;
        parse_next_token(parser);
        end_val = parse_exp(parser, parent, 0);
        if (end_val == 0)
            return 0;
    } else { //default 1
        step = (struct exp_node*)create_double_node(parent, parser->curr_token.loc, 1.0);
    }
    //convert end variable to a logic
    struct exp_node* id_node = (struct exp_node*)create_ident_node(parent, start->loc, string_get(&id_name));
    //printf("end node: %p, %p\n", (void*)id_node, (void*)end_val);
    struct exp_node* end = (struct exp_node*)create_binary_node(parent, end_val->loc, "<", id_node, end_val);
    while (parser->curr_token.token_type == TOKEN_EOS)
        parse_next_token(parser);

    struct exp_node* body = parse_exp(parser, parent, 0);
    if (body == 0)
        return 0;
    return (struct exp_node*)create_for_node(parent, loc, string_get(&id_name), start, end, step, body);
}

struct exp_node* _parse_if(struct parser* parser, struct exp_node* parent)
{
    struct source_loc loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the if.

    // condition.
    //log_info(DEBUG, "parsing if exp");
    struct exp_node* cond = parse_exp(parser, parent, 0);
    if (!cond)
        return 0;
    //log_info(DEBUG, "conf: %s, %s", node_type_strings[cond->node_type], dump(cond).c_str());
    if (parser->curr_token.token_type == TOKEN_THEN) {
        parse_next_token(parser); // eat the then
    }
    while (parser->curr_token.token_type == TOKEN_EOS)
        parse_next_token(parser);
    //log_info(DEBUG, "parsing then exp");
    struct exp_node* then = parse_exp(parser, parent, 0);
    if (then == 0)
        return 0;

    while (parser->curr_token.token_type == TOKEN_EOS)
        parse_next_token(parser);
    if (parser->curr_token.token_type != TOKEN_ELSE)
        return (struct exp_node*)log_info(ERROR, "expected else, got type: %s", node_type_strings[parser->curr_token.token_type]);

    parse_next_token(parser);

    //log_info(DEBUG, "parsing else exp");
    struct exp_node* else_exp = parse_exp(parser, parent, 0);
    if (!else_exp)
        return 0;

    //log_info(DEBUG, "creating if nodes");
    return (struct exp_node*)create_if_node(parent, loc, cond, then, else_exp);
}

struct block_node* parse_block(struct parser* parser, struct exp_node* parent, void (*fun)(void*, struct exp_node*), void* jit)
{
    int col = parent ? parent->loc.col : 1;
    struct array nodes;
    array_init(&nodes, sizeof(struct exp_node*));
    while (true) {
        parse_next_token(parser);
        if (exit_block(parser, parent, col)) {
            //repeat the token
            queue_token(parser, parser->curr_token);
            break;
        }
        while (parser->curr_token.token_type == TOKEN_EOS)
            parse_next_token(parser);
        if (exit_block(parser, parent, col)) {
            queue_token(parser, parser->curr_token);
            break;
        }
        //log_info(DEBUG, "got token in block: (%d, %d), %s", parser->curr_token.loc.line, parser->curr_token.loc.col, token_type_strings[parser->curr_token.token_type]);
        //struct source_loc loc = parser->curr_token.loc;
        //log_info(DEBUG, "parsing statement in block--: (%d, %d), %d", loc.line, loc.col, parent);
        struct exp_node* node = parse_statement(parser, parent);
        if (node) {
            col = node->loc.col;
            array_push(&nodes, &node);
            if (fun) {
                (*fun)(jit, node);
            }
        }
        // if (exit_block(parser, parent, col)) {
        //   break;
        // }
        if (parser->curr_token.token_type == TOKEN_EOF)
            break;
        //   //log_info(DEBUG, "is exp: %d", _is_exp(node));
        //   if(_is_exp(node)) {
        //     //if exp returning it
        //     break;
        //   }
        // }
    }
    return array_size(&nodes) ? create_block_node(parent, &nodes) : 0;
}
