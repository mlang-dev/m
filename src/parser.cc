#include "parser.h"
#include <map>
#include <memory>
#include "builtins.h"
#include "util.h"

using namespace std;

int _get_op_precedence(parser *parser);
exp_node *_parse_number(parser *parser, exp_node* parent);
exp_node *_parse_parentheses(parser *parser, exp_node* parent);
exp_node *_parse_node(parser *parser, exp_node* parent);
exp_node *_parse_binary(parser *parser, exp_node* parent, int exp_prec, exp_node *lhs);
exp_node *_parse_exp(parser *parser, exp_node* parent, exp_node *lhs = 0);
exp_node *_parse_for(parser *parser, exp_node* parent);
exp_node *_parse_if(parser *parser, exp_node* parent);
exp_node *_parse_unary(parser *parser, exp_node* parent);
exp_node *_parse_var(parser *parser, exp_node* parent);
exp_node *_parse_prototype(parser *parser, exp_node* parent);
exp_node *_parse_variable(parser *parser, exp_node* parent, std::string &name);
exp_node *_parse_function_with_prototype(parser *parser,
                                         prototype_node *prototype);

bool _is_exp(exp_node* node){
  return node->type != VAR_NODE && node->type != FUNCTION_NODE && node->type != PROTOTYPE_NODE;
}

function_node *_create_function_node(prototype_node *prototype,
                                     block_node *body) {
  auto node = new function_node();
  node->base.type = NodeType::FUNCTION_NODE;
  node->base.parent = (exp_node*)prototype;
  node->base.loc = prototype->base.loc;
  node->prototype = prototype;
  node->body = body;
  return node;
}

ident_node *_create_ident_node(exp_node* parent, source_loc loc, string &name) {
  auto node = new ident_node();
  node->base.type = NodeType::IDENT_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->name = name;
  return node;
}

num_node *_create_num_node(exp_node* parent, source_loc loc, double val) {
  auto node = new num_node();
  node->base.type = NodeType::NUMBER_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->num_val = val;
  return node;
}

var_node *_create_var_node(exp_node *parent, source_loc loc, const vector<pair<string, exp_node *>> &var_names,
                           exp_node *body) {
  auto node = new var_node();
  node->base.type = NodeType::VAR_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->body = body;
  node->var_names = var_names;
  return node;
}

call_node *_create_call_node(exp_node* parent, source_loc loc, const string &callee,
                             vector<exp_node *> &args) {
  auto node = new call_node();
  node->base.type = NodeType::CALL_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->callee = callee;
  node->args = args;
  return node;
}

prototype_node *create_prototype_node(exp_node* parent, source_loc loc, const string &name,
                                      vector<string> &args,
                                      bool is_operator, unsigned precedence) {
  auto node = new prototype_node();
  node->base.type = NodeType::PROTOTYPE_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->name = name;
  node->args = args;
  node->is_operator = is_operator;
  node->precedence = precedence;
  return node;
}

condition_node *_create_if_node(exp_node* parent, source_loc loc, exp_node *condition, exp_node *then_node,
                                       exp_node *else_node) {
  auto node = new condition_node();
  node->base.type = NodeType::CONDITION_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->condition_node = condition;
  node->then_node = then_node;
  node->else_node = else_node;
  return node;
}

unary_node *_create_unary_node(exp_node* parent, source_loc loc, char op, exp_node *operand) {
  auto node = new unary_node();
  node->base.type = NodeType::UNARY_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->op = op;
  node->operand = operand;
  return node;
}

binary_node *_create_binary_node(exp_node* parent, source_loc loc, char op,  exp_node *lhs, exp_node *rhs) {
  auto node = new binary_node();
  node->base.type = NodeType::BINARY_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->op = op;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

for_node *_create_for_node(exp_node* parent, source_loc loc, const string &var_name, exp_node *start,
                           exp_node *end, exp_node *step, exp_node *body) {
  auto node = new for_node();
  node->base.type = NodeType::FOR_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->var_name = var_name;
  node->start = start;
  node->end = end;
  node->step = step;
  node->body = body;
  return node;
}

block_node *_create_block_node(exp_node* parent, vector<exp_node *>& nodes) {
  block_node *block = new block_node();
  block->base.type = NodeType::BLOCK_NODE;
  block->base.parent = parent;
  block->base.loc = nodes[0]->loc;
  for(auto node:nodes)
    block->nodes.push_back(node);
  return block;
}
std::map<char, int> g_op_precedences = {
    {'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}, {'/', 40}};

module *create_module(const char *mod_name) {
  module *mod = new module();
  mod->name = mod_name;
  mod->block = new block_node();
  return mod;
}
parser *create_parser(bool create_entry, FILE *file) {
  auto psr = new parser();
  psr->op_precedences = &g_op_precedences;
  psr->ast = new ast();
  psr->file = file;
  if (create_entry) {
    psr->ast->entry_module = create_module("main");
    psr->ast->modules.push_back(psr->ast->entry_module);
  }
  return psr;
}

void create_builtins(parser *parser, void *context) {
  get_builtins(context, parser->ast->builtins);
}

void destroy_module(module *module) {
  delete module->block;
  delete module;
}

void destroy_parser(parser *parser) {
  for (auto it : parser->ast->modules) destroy_module(it);
  delete parser->ast;
  delete parser;
}

int parse_next_token(parser *parser) {
  auto token = get_token(parser->file);
  parser->curr_token = token;
  parser->curr_token_num = token.type == TOKEN_OP ? token.op_val : -token.type;
  // fprintf(stderr, "got token: %d, %f, %d\n", parser->curr_token.type,
  //    parser->curr_token.num_val, parser->curr_token_num);
  return parser->curr_token_num;
}

int _get_op_precedence(parser *parser) {
  if (!isascii(parser->curr_token_num)) return -1;
  int op_precedence = (*parser->op_precedences)[parser->curr_token_num];
  // fprintf(stderr, "op %d: pre: %d\n", op, op_precedence);
  if (op_precedence <= 0) return -1;
  return op_precedence;
}

exp_node *_parse_number(parser *parser, exp_node* parent) {
  auto result = _create_num_node(parent, parser->curr_token.loc, parser->curr_token.num_val);
  if(parser->curr_token.type != TOKEN_EOS)
    parse_next_token(parser);
  return (exp_node *)result;
}

exp_node *_parse_parentheses(parser *parser, exp_node* parent) {
  parse_next_token(parser);
  auto v = _parse_exp(parser, parent);
  if (!v) return 0;
  if (parser->curr_token.op_val != ')')
    return (exp_node *)log(ERROR, "expected ')'");
  parse_next_token(parser);
  return v;
}

exp_node *_parse_function_app_or_def(parser *parser, exp_node* parent, source_loc loc, string id_name) {
  if (parser->curr_token_num == '(') parse_next_token(parser);  // skip '('
  auto func_definition = false;
  vector<exp_node *> args;
  if (parser->curr_token_num != ')') {
    while (true) {
      if (auto arg = _parse_exp(parser, parent)){
        args.push_back(arg);
      }
      else
        return 0;
      if (parser->curr_token_num == '=') {
        func_definition = true;
        break;
      }
      else if (parser->curr_token_num == ')'||parser->curr_token.type == TOKEN_EOS)
        break;
      // else if (parser->curr_token_num != ',')
      //     return (exp_node*)log(ERROR, "Expected ')' or ',' in argument
      //     list");
      else if (parser->curr_token_num == ',')
        parse_next_token(parser);
    }
  }
  if (func_definition) {
    vector<string> argNames;
    for (auto exp : args) {
      auto id = (ident_node *)exp;
      argNames.push_back(id->name);
    }
    prototype_node *prototype =
        create_prototype_node(parent, loc, id_name, argNames, false, 0);
    //log(DEBUG, "parsing function body: %c", parser->curr_token_num);
    return _parse_function_with_prototype(parser, prototype);
  }
  // function application
  //log(DEBUG, "funcation application: %s", id_name.c_str());
  exp_node* call_node = (exp_node *)_create_call_node(parent, loc, id_name, args);
  return _parse_exp(parser, parent, call_node);
}

exp_node *parse_statement(parser *parser, exp_node *parent) {
  exp_node *node;
  if (parser->curr_token.type == TOKEN_IMPORT)
    node = parse_import(parser, parent);
  else if (parser->curr_token.type != TOKEN_IDENT){
    //log(DEBUG, "not id token exp: %d", parser->curr_token.type);
    node = _parse_exp(parser, parent);
  }
  else if (parser->curr_token.type == TOKEN_EOF)
    return nullptr;
  else {
    string id_name = *parser->curr_token.ident_str;
    source_loc loc = parser->curr_token.loc;
    //log(DEBUG, "id token: %s", id_name.c_str());
    parse_next_token(parser);  // skip identifier
    char op = parser->curr_token_num;
    if (op == '=') {
      // variable definition
      node = _parse_variable(parser, parent, id_name);
    } else if (parser->curr_token.type == TOKEN_EOS||g_op_precedences[op]) {
      // just id expression evaluation
      auto lhs = (exp_node *)_create_ident_node(parent, parser->curr_token.loc, id_name);
      node = _parse_exp(parser, parent, lhs);
      //log(DEBUG, "parsed exp: id exp: %d", node->type);
    } else {  // if(parser->curr_token_num=='('|| parser->curr_token.type ==
              // TOKEN_IDENT,)
      // function definition or application
      node = _parse_function_app_or_def(parser, parent, loc, id_name);
    }
  }
  node->parent = parent;
  return node;
}

bool _is_new_line(int cha){
  return cha == '\r' || cha == '\n';
}
exp_node *_parse_ident(parser *parser, exp_node* parent) {
  std::string id_name = *parser->curr_token.ident_str;
  source_loc loc = parser->curr_token.loc;
  
  //log(DEBUG, "current id: %s", id_name.c_str());
  if (parser->curr_token.type != TOKEN_EOS)
    parse_next_token(parser);             // take identifier
  if (parser->curr_token_num != '(') {  // pure variable
    // fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
    return (exp_node *)_create_ident_node(parent, parser->curr_token.loc, id_name);
  }
  parse_next_token(parser);  // take next
  std::vector<exp_node *> args;
  if (parser->curr_token_num != ')') {
    while (1) {
      if (auto arg = _parse_exp(parser, parent))
        args.push_back(arg);
      else
        return 0;
      if (parser->curr_token_num == ')') break;

      if (parser->curr_token_num != ',')
        return (exp_node *)log(ERROR, "Expected ')' or ',' in argument list");
      parse_next_token(parser);
    }
  }
  parse_next_token(parser);
  return (exp_node *)_create_call_node(parent, loc, id_name, args);
}

/// varexpr ::= 'let' identifier ('=' expression)?
//                    (',' identifier ('=' expression)?)* 'in' expression
exp_node *_parse_var(parser *parser, exp_node* parent) {
  parse_next_token(parser);  // eat the let.
  // At least one variable name is required.
  if (parser->curr_token.type != TOKEN_IDENT)
    return (exp_node *)log(ERROR, "expected identifier after let");
  std::vector<std::pair<std::string, exp_node *>> var_names;
  while (true) {
    std::string name = *parser->curr_token.ident_str;
    parse_next_token(parser);  // eat identifier.

    // Read the optional initializer.
    exp_node *init = 0;
    if (parser->curr_token_num == '=') {
      parse_next_token(parser);  // eat the '='.
      init = _parse_exp(parser, parent);
      if (init == 0) return 0;
    }

    var_names.push_back(std::make_pair(name, init));
    // End of let list, exit loop.
    if (parser->curr_token_num != ',') break;
    parse_next_token(parser);  // eat the ','.

    if (parser->curr_token.type != TOKEN_IDENT)
      return (exp_node *)log(ERROR, "expected identifier list after var");
  }
  // At this point, we have to have 'in'.
  if (parser->curr_token.type != TOKEN_IN)
    return (exp_node *)log(ERROR, "expected 'in' keyword after 'var'");
  parse_next_token(parser);  // eat 'in'.
  exp_node *body = _parse_exp(parser, parent);
  if (body == 0) return 0;
  return (exp_node *)_create_var_node(parent, parser->curr_token.loc, var_names, body);
}

exp_node *_parse_node(parser *parser, exp_node* parent) {
  if (parser->curr_token.type == TOKEN_IDENT)
    return _parse_ident(parser, parent);
  else if (parser->curr_token.type == TOKEN_NUM)
    return _parse_number(parser, parent);
  else if (parser->curr_token.type == TOKEN_IF)
    return _parse_if(parser, parent);
  else if (parser->curr_token.type == TOKEN_FOR)
    return _parse_for(parser, parent);
  else if (parser->curr_token.type == TOKEN_VAR)
    return _parse_var(parser, parent);
  else if (parser->curr_token_num == '(')
    return _parse_parentheses(parser, parent);
  else {
    std::string error =
        "unknown token: " + std::to_string(parser->curr_token.type);
    if (parser->curr_token.type == TOKEN_OP)
      error += " op: " + std::to_string(parser->curr_token.op_val);
    return (exp_node *)log(ERROR, error.c_str());
  }
}

exp_node *_parse_binary(parser *parser, exp_node* parent, int exp_prec, exp_node *lhs) {
  while (true) {
    int tok_prec = _get_op_precedence(parser);
    //log(DEBUG, "op prec1: %d, %d", tok_prec, exp_prec);
    if (tok_prec < exp_prec) return lhs;
    int binary_op = parser->curr_token_num;
    parse_next_token(parser);
    auto rhs = _parse_unary(parser, parent);
    if (!rhs) return lhs;
    auto next_prec = _get_op_precedence(parser);
    if (tok_prec < next_prec) {
      rhs = _parse_binary(parser, parent, tok_prec + 1, rhs);
      if (!rhs) return 0;
    }
    //log(DEBUG, "op prec2: %d, %d", tok_prec, next_prec);
    lhs = (exp_node *)_create_binary_node(parent, lhs->loc, binary_op, lhs, rhs);
  }
}

exp_node *_parse_exp(parser *parser, exp_node* parent, exp_node *lhs) {
  if (!lhs) lhs = _parse_unary(parser, parent);
  if (!lhs) return 0;
  return _parse_binary(parser, parent, 0, lhs);
}

/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
exp_node *_parse_prototype(parser *parser, exp_node* parent) {
  std::string fun_name;
  source_loc loc = parser->curr_token.loc;
  // SourceLocation FnLoc = CurLoc;
  unsigned proto_type = 0;  // 0 = identifier, 1 = unary, 2 = binary.
  unsigned bin_prec = 30;
  int token = 0;
  switch (parser->curr_token.type) {
    case TOKEN_IDENT:
      fun_name = *parser->curr_token.ident_str;
      proto_type = 0;
      // fprintf(stderr, "ident token in parse prototype: %s\n",
      // fun_name.c_str());
      parse_next_token(parser);
      break;
    case TOKEN_UNARY:
      token = parse_next_token(parser);
      if (!isascii(token))
        return (exp_node *)log(ERROR, "Expected unary operator");
      fun_name = "unary";
      fun_name += (char)token;
      proto_type = 1;
      parse_next_token(parser);
      break;
    case TOKEN_BINARY:
      token = parse_next_token(parser);
      if (!isascii(token))
        return (exp_node *)log(ERROR, "Expected binary operator");
      fun_name = "binary";
      fun_name += (char)token;
      proto_type = 2;
      parse_next_token(parser);
      // Read the precedence if present.
      if (parser->curr_token.type == TOKEN_NUM) {
        if (parser->curr_token.num_val < 1 || parser->curr_token.num_val > 100)
          return (exp_node *)log(ERROR, "Invalid precedecnce: must be 1..100");
        bin_prec = (unsigned)parser->curr_token.num_val;
        parse_next_token(parser);
      }
      break;
    default:
      return (exp_node *)log(ERROR, "Expected function name in prototype");
  }
  auto has_parenthese = parser->curr_token_num == '(';
  if (has_parenthese) parse_next_token(parser);  // skip '('
  std::vector<std::string> arg_names;
  while (parser->curr_token_num == TOKEN_IDENT) {
    // fprintf(stderr, "arg names: %s",
    // (*parser->curr_token.ident_str).c_str());
    arg_names.push_back(*parser->curr_token.ident_str);
    parse_next_token(parser);
  }
  if (has_parenthese && parser->curr_token_num != ')')
    return (exp_node *)log(ERROR, "Expected ')' to match '('");
  // success.
  if (has_parenthese) parse_next_token(parser);  // eat ')'.
  // Verify right number of names for operator.
  if (proto_type && arg_names.size() != proto_type)
    return (exp_node *)log(ERROR, "Invalid number of operands for operator");
  // is a value: has no parenthese and no parameters
  // fprintf(stderr, "creating prototype: %s, args: %lu, current token: %c\n",
  // fun_name.c_str(), arg_names.size(), parser->curr_token_num);
  return (exp_node *)create_prototype_node(parent, loc, fun_name, arg_names, proto_type != 0,
                                           bin_prec);
}

exp_node *_parse_function_with_prototype(parser *parser,
                                         prototype_node *prototype) {
  auto block = parse_block(parser, (exp_node*)prototype);
  if (block) {
    return (exp_node *)_create_function_node(prototype, block);
  }
  return 0;
}

exp_node *_parse_variable(parser *parser, exp_node* parent, std::string &name) {
  if (parser->curr_token_num == '=')
    parse_next_token(parser);  // skip '='
                               // token
  if (auto exp = _parse_exp(parser, parent)) {
    // not a function but a value
    std::vector<std::pair<std::string, exp_node *>> var_names;
    var_names.push_back(std::make_pair(name, exp));
    //log(INFO, "_parse_variable:  %lu!", var_names.size());
    return (exp_node *)_create_var_node(parent, parser->curr_token.loc, var_names, exp);
  }
  return 0;
}

exp_node *parse_exp_to_function(parser *parser, exp_node *exp, const char *fn) {
  if (!exp) exp = _parse_exp(parser, nullptr);
  if (exp) {
    auto args = vector<string>();
    auto prototype = create_prototype_node(nullptr, exp->loc, fn, args);
    vector<exp_node*> nodes;
    nodes.push_back(exp);
    auto block = _create_block_node((exp_node*)prototype, nodes);
    return (exp_node *)_create_function_node(prototype, block);
  }
  return 0;
}

exp_node *parse_import(parser *parser, exp_node* parent) {
  parse_next_token(parser);
  return _parse_prototype(parser, parent);
}

/// unary
///   ::= primary
///   ::= '!' unary
exp_node *_parse_unary(parser *parser, exp_node* parent) {
  // If the current token is not an operator, it must be a primary expr.
  auto loc = parser->curr_token.loc;
  if (!isascii(parser->curr_token_num) || parser->curr_token_num == '(' ||
      parser->curr_token_num == ',') {
    //log(DEBUG, "parse unary xx: %d, %d", parser->curr_token.type, parser->curr_token_num);
    return _parse_node(parser, parent);
  }
  // fprintf(stderr, "unary: %d, %d\n", parser->curr_token.op_val,
  // parser->curr_token_num);
  // If this is a unary operator, read it.
  int opc = parser->curr_token.op_val;
  parse_next_token(parser);
  if (exp_node *operand = _parse_unary(parser, parent))
    return (exp_node *)_create_unary_node(parent, loc, opc, operand);
  return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
exp_node *_parse_for(parser *parser, exp_node* parent) {
  source_loc loc = parser->curr_token.loc;
  parse_next_token(parser);  // eat the for.

  if (parser->curr_token.type != TOKEN_IDENT)
    return (exp_node *)log(ERROR, "expected identifier after for, got %s", TokenTypeString[parser->curr_token.type]);

  std::string id_name = *parser->curr_token.ident_str;
  parse_next_token(parser);  // eat identifier.

  if (parser->curr_token.type != TOKEN_IN)
    return (exp_node *)log(ERROR, "expected 'in' after for %s", parser->curr_token.ident_str);
  parse_next_token(parser);  // eat 'in'.

  exp_node *start = _parse_exp(parser, parent);
  if (start == 0) return 0;
  if (parser->curr_token.type != TOKEN_RANGE)
    return (exp_node *)log(ERROR, "expected '..' after for start value");
  parse_next_token(parser);

  //step or end
  exp_node *end = _parse_exp(parser, parent);
  if (end == 0) return 0;

  // The step value is optional.
  exp_node *step = 0;
  if (parser->curr_token.type == TOKEN_RANGE) {
    step = end;
    parse_next_token(parser);
    end = _parse_exp(parser, parent);
    if (end == 0) return 0;
  }

  exp_node *body = _parse_exp(parser, parent);
  if (body == 0) return 0;

  return (exp_node *)_create_for_node(parent, loc, id_name, start, end, step, body);
}

exp_node *_parse_if(parser *parser, exp_node* parent) {
  source_loc loc = parser->curr_token.loc;
  parse_next_token(parser);  // eat the if.

  // condition.
  //log(DEBUG, "parsing if exp");
  exp_node *cond = _parse_exp(parser, parent);
  if (!cond) return 0;

  // if (parser->curr_token.type != TOKEN_THEN)
  //   return (exp_node *)log(ERROR, "expected then");
  // parse_next_token(parser);  // eat the then
  while (parser->curr_token.type==TOKEN_EOS) parse_next_token(parser);
  //log(DEBUG, "parsing then exp");
  exp_node *then = _parse_exp(parser, parent);
  if (then == 0) return 0;

  while (parser->curr_token.type==TOKEN_EOS) parse_next_token(parser);
  if (parser->curr_token.type != TOKEN_ELSE)
    return (exp_node *)log(ERROR, "expected else, got type: %d", parser->curr_token.type);

  parse_next_token(parser);

  //log(DEBUG, "parsing else exp");
  exp_node *else_exp = _parse_exp(parser, parent);
  if (!else_exp) return 0;

  //log(DEBUG, "creating if nodes");
  return (exp_node *)_create_if_node(parent, loc, cond, then, else_exp);
}

block_node *parse_block(parser *parser, exp_node *parent, void (*fun)(void*, exp_node*), void* jit) {
  int col = parent? parent->loc.col : 0;
  vector<exp_node*> nodes;
  while (true) {
    parse_next_token(parser);
    if(parser->curr_token.type == TOKEN_EOS)
      continue;
    if (parser->curr_token.loc.col < col || parser->curr_token.type == TOKEN_EOF) {
      break;
    }
    auto node = parse_statement(parser, parent);
    if (!node)
      break;
    if(fun){
      (*fun)(jit, node);
    }
    nodes.push_back(node);
    if(parent){
      //log(DEBUG, "is exp: %d", _is_exp(node));
      if(_is_exp(node)) {
        //if exp returning it
        break;
      }
    }
  }
  return nodes.size() > 0 ? _create_block_node(parent, nodes) : nullptr;
}