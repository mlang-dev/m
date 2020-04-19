/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language parser
 */
#include "parser.h"
#include "astdump.h"
#include "builtins.h"
#include "clib/util.h"
#include <map>
#include <memory>
#include <set>
#include <sstream>

#define exit_block(parser, parent, col) (parent && parser->curr_token.loc.col < col && (parser->curr_token.token_type != TOKEN_EOS || parser->is_repl))


std::map<std::string, int> g_op_precedences = {
    { "<", 10 }, { ">", 10 }, { "==", 10 }, { "!=", 10 }, { "<=", 10 }, { ">=", 10 },
    { "+", 20 }, { "-", 20 },
    { "*", 40 }, { "/", 40 }
};

int _get_op_precedence(parser* parser);
exp_node* _parse_number(parser* parser, exp_node* parent);
exp_node* _parse_parentheses(parser* parser, exp_node* parent);
exp_node* _parse_node(parser* parser, exp_node* parent);
exp_node* _parse_binary(parser* parser, exp_node* parent, int exp_prec, exp_node* lhs);
exp_node* _parse_for(parser* parser, exp_node* parent);
exp_node* _parse_if(parser* parser, exp_node* parent);
exp_node* _parse_unary(parser* parser, exp_node* parent);
exp_node* _parse_prototype(parser* parser, exp_node* parent);
exp_node* _parse_var(parser* parser, exp_node* parent, const char *name);
exp_node* _parse_function_with_prototype(parser* parser,
    prototype_node* prototype);

const char* get_ctt(parser* parser)
{
    return TokenTypeString[parser->curr_token.token_type];
}
bool _is_exp(exp_node* node)
{
    return node->node_type != VAR_NODE && node->node_type != FUNCTION_NODE && node->node_type != PROTOTYPE_NODE;
}

void queue_token(parser* psr, token tkn)
{
    psr->queued_tokens.push(tkn);
}

void queue_tokens(parser* psr, std::vector<token> tokens)
{
    for (auto tkn : tokens)
        psr->queued_tokens.push(tkn);
}

parser* parser_new(const char* file_name, bool is_repl, FILE* (*open_file)(const char* file_name))
{
    FILE* file;
    if (open_file)
        file = open_file(file_name);
    else
        file = file_name ? fopen(file_name, "r") : stdin;
    const char* mod_name = file_name ? file_name : "intepreter_main";
    auto psr = new parser();
    psr->op_precedences = &g_op_precedences;
    psr->ast = new ast();
    psr->allow_id_as_a_func = true;
    psr->is_repl = is_repl;
    psr->current_module = create_module(mod_name, file);
    psr->ast->modules.push_back(psr->current_module);
    return psr;
}

void create_builtins(parser* parser, void* context)
{
    get_builtins(context, parser->ast->builtins);
}

void block_deinit(block_node *block)
{
    for(exp_node *node : block->nodes){

    }
}

void destroy_module(module* module)
{
    destroy_tokenizer(module->tokenizer);
    block_deinit(module->block);
    delete module->block;
    string_deinit(&module->name);
    delete module;
}

void parser_free(parser* parser)
{
    for (auto it : parser->ast->modules)
        destroy_module(it);
    delete parser->ast;
    delete parser;
}

void parse_next_token(parser* parser)
{
    if (!parser->queued_tokens.empty()) {
        // cleanup queued tokens, to redo parsing
        parser->curr_token = parser->queued_tokens.front();
        parser->queued_tokens.pop();
        //log_info(DEBUG, "using queued tokens !");
    } else
        parser->curr_token = *get_token(parser->current_module->tokenizer);
}

int _get_op_precedence(parser* parser)
{
    if (parser->curr_token.token_type != TOKEN_OP)
        return -1;
    int op_precedence = g_op_precedences[std::string(string_get(parser->curr_token.ident_str))];
    // fprintf(stderr, "op %d: pre: %d\n", op, op_precedence);
    if (op_precedence <= 0)
        return -1;
    return op_precedence;
}

exp_node* _parse_number(parser* parser, exp_node* parent)
{
    num_node* result;
    if (parser->curr_token.type == TYPE_INT)
        result = create_num_node(parent, parser->curr_token.loc,
            parser->curr_token.int_val);
    else
        result = create_num_node(parent, parser->curr_token.loc,
            parser->curr_token.double_val);
    if (parser->curr_token.token_type != TOKEN_EOS)
        parse_next_token(parser);
    return (exp_node*)result;
}

exp_node* _parse_parentheses(parser* parser, exp_node* parent)
{
    parse_next_token(parser);
    auto v = parse_exp(parser, parent);
    if (!v)
        return 0;
    if (parser->curr_token.token_type != TOKEN_RPAREN)
        return (exp_node*)log_info(ERROR, "expected ')'");
    parse_next_token(parser);
    return v;
}

exp_node* _parse_function_app_or_def(parser* parser, exp_node* parent, source_loc loc, std::string id_name, bool is_operator = false, int precedence = 0)
{
    if (parser->curr_token.token_type == TOKEN_LPAREN)
        parse_next_token(parser); // skip '('
    auto func_definition = false;
    std::vector<exp_node*> args;
    //doesn't allow function inside parameter or argument
    //will be enhanced later
    //log_info(DEBUG, "function app or def: %s", id_name.c_str());
    parser->allow_id_as_a_func = false;
    if (parser->curr_token.token_type != TOKEN_RPAREN) {
        while (true) {
            if (auto arg = parse_exp(parser, parent)) {
                args.push_back(arg);
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
        std::vector<std::string> argNames;
        for (auto exp : args) {
            auto id = (ident_node*)exp;
            std::string idname(string_get(&id->name));
            argNames.push_back(idname);
        }
        if (is_operator) {
            if (precedence && argNames.size() != 2){
                return (exp_node*)log_info(ERROR, "precedence only apply for binary operator");
            }
            else if (argNames.size() != 1 && argNames.size() != 2)
                return (exp_node*)log_info(ERROR, "operator overloading only for unary or binary operator");
            if (argNames.size() == 1) {
                id_name = "unary" + id_name;
            } else {
                id_name = "binary" + id_name;
            }
        }
        prototype_node* prototype = create_prototype_node(parent, loc, id_name.c_str(), argNames, is_operator, precedence, is_operator ? id_name.c_str() : "");
        //log_info(DEBUG, "prototype: %s", id_name.c_str());
        auto func = _parse_function_with_prototype(parser, prototype);
        //log_info(DEBUG, "func: %s", id_name.c_str());
        return func;
    }
    // function application
    //log_info(DEBUG, "function application: %s", id_name.c_str());
    exp_node* call_node = (exp_node*)create_call_node(parent, loc, id_name.c_str(), args);
    return parse_exp(parser, parent, call_node);
}

extern std::set<char> op_chars;
exp_node* parse_statement(parser* parser, exp_node* parent)
{
    //log_info(DEBUG, "parsing statement:%d, %s", parent, TokenTypeString[parser->curr_token.token_type]);
    exp_node* node;
    source_loc loc = parser->curr_token.loc;
    if (parser->curr_token.token_type == TOKEN_EOF)
        return nullptr;
    else if (parser->curr_token.token_type == TOKEN_IMPORT)
        node = parse_import(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_UNARY || parser->curr_token.token_type == TOKEN_BINARY) {
        //function def
        auto proto = _parse_prototype(parser, parent);
        node = _parse_function_with_prototype(parser, (prototype_node*)proto);
    } else if (parser->curr_token.token_type == TOKEN_IDENT) {
        std::string id_name(string_get(parser->curr_token.ident_str));
        source_loc loc = parser->curr_token.loc;
        parse_next_token(parser); // skip identifier
        std::string op = parser->curr_token.token_type == TOKEN_OP ? std::string(string_get(parser->curr_token.ident_str)) : "";
        //log_info(DEBUG, "id token: %s, %s, %d", id_name.c_str(), op.c_str(), parent);
        if (op == "=") {
            // variable definition
            node = _parse_var(parser, parent, id_name.c_str());
        } else if (parser->curr_token.token_type == TOKEN_EOS || parser->curr_token.token_type == TOKEN_EOF || g_op_precedences[op]) {
            // just id expression evaluation
            auto lhs = (exp_node*)create_ident_node(parent, parser->curr_token.loc, id_name.c_str());
            node = parse_exp(parser, parent, lhs);
            //log_info(DEBUG, "parsed exp: id exp: %d", node->node_type);
        } else {
            // function definition or application
            node = _parse_function_app_or_def(parser, parent, loc, id_name.c_str());
        }
    } else {
        if (parser->curr_token.token_type == TOKEN_LPAREN) {
            std::vector<token> queued;
            queued.push_back(parser->curr_token);
            parse_next_token(parser); //skip (
            queued.push_back(parser->curr_token);
            if (parser->curr_token.token_type == TOKEN_OP && op_chars.count(string_get(parser->curr_token.ident_str)[0])) {
                //it is operator overloading
                //log_info(DEBUG, "it is operator overloading: %c: loc: %d, %d", parser->curr_token.op_val, parser->curr_token.loc.line, parser->curr_token.loc.col);
                std::string op(string_get(parser->curr_token.ident_str));
                parse_next_token(parser);
                if (parser->curr_token.token_type != TOKEN_RPAREN)
                    return (exp_node*)log_info(ERROR, "expected ')'");
                parse_next_token(parser);
                int precedence = 0;
                if (parser->curr_token.token_type == TOKEN_NUM) {
                    //precedence: parse binary
                    precedence = parser->curr_token.double_val;
                    parse_next_token(parser); //skip it
                    //log_info(DEBUG, "got precedence: %d", precedence);
                }
                node = _parse_function_app_or_def(parser, parent, loc, op.c_str(), true, precedence);
            } else { //normal exp
                queue_tokens(parser, queued);
                parse_next_token(parser); //retrieving (
                //log_info(DEBUG, "normal exp: %c", parser->curr_token.op_val);
                node = parse_exp(parser, parent);
            }
        } else {
            //log_info(DEBUG, "starting to parse exp: %s", TokenTypeString[parser->curr_token.token_type]);
            node = parse_exp(parser, parent);
            //log_info(DEBUG, "it's exp: %s", NodeTypeString[node->node_type]);
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

bool _id_is_a_function_call(parser* parser)
{
    return parser->allow_id_as_a_func && (parser->curr_token.token_type == TOKEN_IDENT || parser->curr_token.token_type == TOKEN_NUM || parser->curr_token.token_type == TOKEN_IF || parser->curr_token.token_type == TOKEN_UNARY || parser->curr_token.token_type == TOKEN_LPAREN);
}

exp_node* _parse_ident(parser* parser, exp_node* parent)
{
    std::string id_name(string_get(parser->curr_token.ident_str));
    source_loc loc = parser->curr_token.loc;

    parse_next_token(parser); // take identifier
    //log_info(DEBUG, "parsed id: %s, curtoken: %s", id_name.c_str(), TokenTypeString[parser->curr_token.token_type]);
    if (_id_is_a_function_call(parser)) { // pure variable
        // fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
        //(
        std::vector<exp_node*> args;
        while (true) {
            if (auto arg = parse_exp(parser, parent))
                args.push_back(arg);
            else
                return 0;
            if (!_id_is_a_function_call(parser))
                break;

            parse_next_token(parser);
        }
        parse_next_token(parser);
        return (exp_node*)create_call_node(parent, loc, id_name.c_str(), args);
    }
    return (exp_node*)create_ident_node(parent, parser->curr_token.loc, id_name.c_str());
}

exp_node* _parse_node(parser* parser, exp_node* parent)
{
    if (parser->curr_token.token_type == TOKEN_IDENT)
        return _parse_ident(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_NUM)
        return _parse_number(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_IF)
        return _parse_if(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_FOR)
        return _parse_for(parser, parent);
    else if (parser->curr_token.token_type == TOKEN_LPAREN)
        return _parse_parentheses(parser, parent);
    else {
        std::string error = "unknown token: " + std::to_string(parser->curr_token.token_type);
        if (parser->curr_token.token_type == TOKEN_OP)
            error += " op: " + std::string(string_get(parser->curr_token.ident_str));
        return (exp_node*)log_info(ERROR, error.c_str());
    }
}

std::string map_to_string(std::map<char, int>& m)
{
    std::string output = "";
    std::string convrt = "";
    std::string result = "";
    for (auto it = m.cbegin(); it != m.cend(); it++) {
        convrt = std::to_string(it->second);
        output += it->first;
        output += ":" + convrt + ", ";
    }
    result = output.substr(0, output.size() - 2);
    return result;
}

exp_node* _parse_binary(parser* parser, exp_node* parent, int exp_prec, exp_node* lhs)
{
    while (true) {
        if (parser->curr_token.token_type == TOKEN_EOS)
            return lhs;
        int tok_prec = _get_op_precedence(parser);
        if (tok_prec < exp_prec)
            return lhs;
        //log_info(DEBUG, "bin exp: [%s, %c], %d, %s", TokenTypeString[parser->curr_token.token_type], parser->curr_token.op_val, tok_prec, map_to_string(g_op_precedences).c_str());
        std::string binary_op(string_get(parser->curr_token.ident_str));
        parse_next_token(parser);
        auto rhs = _parse_unary(parser, parent);
        if (!rhs)
            return lhs;
        //log_info(DEBUG, "bin exp: rhs: %s", NodeTypeString[rhs->node_type]);
        auto next_prec = _get_op_precedence(parser);
        if (tok_prec < next_prec) {
            //log_info(DEBUG, "right first %s, %d, %d", TokenTypeString[parser->curr_token.token_type], tok_prec, next_prec);
            rhs = _parse_binary(parser, parent, tok_prec + 1, rhs);
            if (!rhs)
                return 0;
        }
        //log_info(DEBUG, "left first: %s, %d, %d", TokenTypeString[parser->curr_token.token_type], tok_prec, next_prec);
        lhs = (exp_node*)create_binary_node(parent, lhs->loc, binary_op.c_str(), lhs, rhs);
    }
}

exp_node* parse_exp(parser* parser, exp_node* parent, exp_node* lhs)
{
    if (parser->curr_token.token_type == TOKEN_EOS)
        return lhs;
    if (!lhs)
        lhs = _parse_unary(parser, parent);
    //log_info(DEBUG, "got lhs: %s, %s", NodeTypeString[lhs->node_type], parser->curr_token.ident_str->c_str());
    if (!lhs || parser->curr_token.token_type == TOKEN_EOS)
        return lhs;
    return _parse_binary(parser, parent, 0, lhs);
}

/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
exp_node* _parse_prototype(parser* parser, exp_node* parent)
{
    std::string fun_name;
    source_loc loc = parser->curr_token.loc;
    // SourceLocation FnLoc = CurLoc;
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    switch (parser->curr_token.token_type) {
    case TOKEN_IDENT:
        fun_name = std::string(string_get(parser->curr_token.ident_str));
        proto_type = 0;
        // fprintf(stderr, "ident token in parse prototype: %s\n",
        // fun_name.c_str());
        parse_next_token(parser);
        break;
    case TOKEN_UNARY:
        parse_next_token(parser);
        if (parser->curr_token.token_type != TOKEN_OP)
            return (exp_node*)log_info(ERROR, "Expected unary operator");
        fun_name = "unary";
        fun_name += std::string(string_get(parser->curr_token.ident_str));
        //log_info(DEBUG, "finding unary operator: %s", fun_name.c_str());
        proto_type = 1;
        parse_next_token(parser);
        break;
    case TOKEN_BINARY:
        parse_next_token(parser);
        if (parser->curr_token.token_type != TOKEN_OP)
            return (exp_node*)log_info(ERROR, "Expected binary operator");
        fun_name = "binary";
        fun_name += std::string(string_get(parser->curr_token.ident_str));
        proto_type = 2;
        parse_next_token(parser);
        // Read the precedence if present.
        if (parser->curr_token.token_type == TOKEN_NUM) {
            if (parser->curr_token.double_val < 1 || parser->curr_token.double_val > 100)
                return (exp_node*)log_info(ERROR, "Invalid precedecnce: must be 1..100");
            bin_prec = (unsigned)parser->curr_token.double_val;
            //log_info(DEBUG, "finding binary operator: %s, prec: %d", fun_name.c_str(), bin_prec);
            parse_next_token(parser);
        }
        break;
    default:
        return (exp_node*)log_info(ERROR, "Expected function name in prototype");
    }
    auto has_parenthese = parser->curr_token.token_type == TOKEN_LPAREN;
    if (has_parenthese)
        parse_next_token(parser); // skip '('
    std::vector<std::string> arg_names;
    while (parser->curr_token.token_type == TOKEN_IDENT) {
        // fprintf(stderr, "arg names: %s",
        // (*parser->curr_token.ident_str).c_str());
        arg_names.push_back(std::string(string_get(parser->curr_token.ident_str)));
        parse_next_token(parser);
    }
    if (has_parenthese && parser->curr_token.token_type != TOKEN_RPAREN)
        return (exp_node*)log_info(ERROR, "Expected ')' to match '('");
    // success.
    if (has_parenthese)
        parse_next_token(parser); // eat ')'.
    // Verify right number of names for operator.
    if (proto_type && arg_names.size() != proto_type)
        return (exp_node*)log_info(ERROR, "Invalid number of operands for operator");
    return (exp_node*)create_prototype_node(parent, loc, fun_name.c_str(), arg_names, proto_type != 0,
        bin_prec);
}

exp_node* _parse_function_with_prototype(parser* parser,
    prototype_node* prototype)
{
    auto block = parse_block(parser, (exp_node*)prototype);
    if (block) {
        return (exp_node*)create_function_node(prototype, block);
    }
    return 0;
}

exp_node* _parse_var(parser* parser, exp_node* parent, const char *name)
{
    if (string_eq_chars(parser->curr_token.ident_str, "="))
        parse_next_token(parser); // skip '='
            // token
    if (auto exp = parse_exp(parser, parent)) {
        // not a function but a value
        //log_info(INFO, "_parse_variable:  %lu!", var_names.size());
        return (exp_node*)create_var_node(parent, parser->curr_token.loc, name, exp);
    }
    return 0;
}

exp_node* parse_exp_to_function(parser* parser, exp_node* exp, const char* fn)
{
    if (!exp)
        exp = parse_exp(parser, nullptr);
    if (exp) {
        auto args = std::vector<std::string>();
        auto prototype = create_prototype_node(nullptr, exp->loc, fn, args);
        std::vector<exp_node*> nodes;
        nodes.push_back(exp);
        auto block = create_block_node((exp_node*)prototype, nodes);
        return (exp_node*)create_function_node(prototype, block);
    }
    return 0;
}

exp_node* parse_import(parser* parser, exp_node* parent)
{
    parse_next_token(parser);
    return _parse_prototype(parser, parent);
}

/// unary
///   ::= primary
///   ::= '!' unary
exp_node* _parse_unary(parser* parser, exp_node* parent)
{
    // If the current token is not an operator, it must be a primary expr.
    if (parser->curr_token.token_type == TOKEN_RANGE
        || parser->curr_token.token_type == TOKEN_EOS
        || parser->curr_token.token_type == TOKEN_EOF)
        return 0;
    auto loc = parser->curr_token.loc;
    if (parser->curr_token.token_type != TOKEN_OP || parser->curr_token.token_type == TOKEN_LPAREN 
        || string_eq_chars(parser->curr_token.ident_str, ",")) {
        return _parse_node(parser, parent);
    }
    //log_info(DEBUG, "unary: %c", parser->curr_token.op_val);
    // If this is a unary operator, read it.
    std::string opc = std::string(string_get(parser->curr_token.ident_str));
    parse_next_token(parser);
    if (exp_node* operand = _parse_unary(parser, parent)) {
        //log_info(DEBUG, "unary node:%c: %s", opc, NodeTypeString[operand->node_type]);
        return (exp_node*)create_unary_node(parent, loc, opc.c_str(), operand);
    }
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
exp_node* _parse_for(parser* parser, exp_node* parent)
{
    source_loc loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the for.

    if (parser->curr_token.token_type != TOKEN_IDENT)
        return (exp_node*)log_info(ERROR, "expected identifier after for, got %s", TokenTypeString[parser->curr_token.token_type]);

    std::string id_name = std::string(string_get(parser->curr_token.ident_str));
    parse_next_token(parser); // eat identifier.

    if (parser->curr_token.token_type != TOKEN_IN)
        return (exp_node*)log_info(ERROR, "expected 'in' after for %s", parser->curr_token.ident_str);
    parse_next_token(parser); // eat 'in'.

    exp_node* start = parse_exp(parser, parent);
    if (start == 0)
        return 0;
    if (parser->curr_token.token_type != TOKEN_RANGE)
        return (exp_node*)log_info(ERROR, "expected '..' after for start value got token: %s: %s",
            TokenTypeString[parser->curr_token.token_type], parser->curr_token.ident_str);
    parse_next_token(parser);

    //step or end
    exp_node* end_val = parse_exp(parser, parent);
    if (end_val == 0)
        return 0;

    // The step value is optional.
    exp_node* step = 0;
    if (parser->curr_token.token_type == TOKEN_RANGE) {
        step = end_val;
        parse_next_token(parser);
        end_val = parse_exp(parser, parent);
        if (end_val == 0)
            return 0;
    } else { //default 1
        step = (exp_node*)create_num_node(parent, parser->curr_token.loc, 1.0);
    }
    //convert end variable to a logic
    auto id_node = (exp_node*)create_ident_node(parent, start->loc, id_name.c_str());
    auto end = (exp_node*)create_binary_node(parent, end_val->loc, "<", id_node, end_val);
    while (parser->curr_token.token_type == TOKEN_EOS)
        parse_next_token(parser);

    exp_node* body = parse_exp(parser, parent);
    if (body == 0)
        return 0;
    return (exp_node*)create_for_node(parent, loc, id_name.c_str(), start, end, step, body);
}

exp_node* _parse_if(parser* parser, exp_node* parent)
{
    source_loc loc = parser->curr_token.loc;
    parse_next_token(parser); // eat the if.

    // condition.
    //log_info(DEBUG, "parsing if exp");
    exp_node* cond = parse_exp(parser, parent);
    if (!cond)
        return 0;
    //log_info(DEBUG, "conf: %s, %s", NodeTypeString[cond->node_type], dump(cond).c_str());
    if (parser->curr_token.token_type == TOKEN_THEN) {
        parse_next_token(parser); // eat the then
    }
    while (parser->curr_token.token_type == TOKEN_EOS)
        parse_next_token(parser);
    //log_info(DEBUG, "parsing then exp");
    exp_node* then = parse_exp(parser, parent);
    if (then == 0)
        return 0;

    while (parser->curr_token.token_type == TOKEN_EOS)
        parse_next_token(parser);
    if (parser->curr_token.token_type != TOKEN_ELSE)
        return (exp_node*)log_info(ERROR, "expected else, got type: %s", NodeTypeString[parser->curr_token.token_type]);

    parse_next_token(parser);

    //log_info(DEBUG, "parsing else exp");
    exp_node* else_exp = parse_exp(parser, parent);
    if (!else_exp)
        return 0;

    //log_info(DEBUG, "creating if nodes");
    return (exp_node*)create_if_node(parent, loc, cond, then, else_exp);
}

block_node* parse_block(parser* parser, exp_node* parent, void (*fun)(void*, exp_node*), void* jit)
{
    int col = parent ? parent->loc.col : 1;
    std::vector<exp_node*> nodes;
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
        //log_info(DEBUG, "got token in block: (%d, %d), %s", parser->curr_token.loc.line, parser->curr_token.loc.col, TokenTypeString[parser->curr_token.token_type]);
        source_loc loc = parser->curr_token.loc;
        //log_info(DEBUG, "parsing statement in block--: (%d, %d), %d", loc.line, loc.col, parent);
        auto node = parse_statement(parser, parent);
        if (node) {
            col = node->loc.col;
            //log_info(DEBUG, "parsed statement in block: (%d, %d), %s, %d", node->loc.line, col, node?NodeTypeString[node->node_type]:"null node", parent);
        }
        if (node) {
            nodes.push_back(node);
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
    //log_info(DEBUG, "parsed statements: %d", nodes.size());
    return nodes.size() > 0 ? create_block_node(parent, nodes) : nullptr;
}