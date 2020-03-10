#include "parser.h"
#include <map>
#include <memory>
#include "builtins.h"
#include "util.h"

#define exit_block(parser, parent, col) (parent && parser->curr_token.loc.col <= col && (parser->curr_token.type != TOKEN_EOS||parser->is_repl))

using namespace std;

int _get_op_precedence(parser *parser);
exp_node *_parse_number(parser *parser, exp_node* parent);
exp_node *_parse_parentheses(parser *parser, exp_node* parent);
exp_node *_parse_node(parser *parser, exp_node* parent);
exp_node *_parse_binary(parser *parser, exp_node* parent, int exp_prec, exp_node *lhs);
exp_node *_parse_for(parser *parser, exp_node* parent);
exp_node *_parse_if(parser *parser, exp_node* parent);
exp_node *_parse_unary(parser *parser, exp_node* parent);
exp_node *_parse_prototype(parser *parser, exp_node* parent);
exp_node *_parse_var(parser *parser, exp_node* parent, std::string &name);
exp_node *_parse_function_with_prototype(parser *parser,
                                         prototype_node *prototype);

const char* get_ctt(parser* parser){
  return TokenTypeString[parser->curr_token.type];
}
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

var_node *_create_var_node(exp_node *parent, source_loc loc, string var_name, exp_node *init_value) {
  auto node = new var_node();
  node->base.type = NodeType::VAR_NODE;
  node->base.parent = parent;
  node->base.loc = loc;
  node->var_name = var_name;
  node->init_value = init_value;
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
map<char, int> g_op_precedences = {
    {'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}, {'/', 40}};

module *create_module(const char *mod_name, FILE* file) {
  module *mod = new module();
  mod->name = mod_name;
  mod->block = new block_node();
  mod->tokenizer = create_tokenizer(file);
  return mod;
}
parser *create_parser(const char *file_name, bool is_repl, FILE* (*open_file)(const char* file_name)) {
  FILE* file;
  if (open_file)
    file = open_file(file_name);
  else
    file = file_name? fopen(file_name, "r") : stdin;
  const char* mod_name = file_name? file_name : "intepreter_main";
  auto psr = new parser();
  psr->op_precedences = &g_op_precedences;
  psr->ast = new ast();
  psr->allow_id_as_a_func = true;
  psr->is_repl = is_repl;
  psr->current_module = create_module(mod_name, file);
  psr->ast->modules.push_back(psr->current_module);
  return psr;
}

void create_builtins(parser *parser, void *context) {
  get_builtins(context, parser->ast->builtins);
}

void destroy_module(module *module) {
  destroy_tokenizer(module->tokenizer);
  delete module->block;
  delete module;
}

void destroy_parser(parser *parser) {
  for (auto it : parser->ast->modules) destroy_module(it);
  delete parser->ast;
  delete parser;
}

void parse_next_token(parser *parser) {
  auto token = get_token(parser->current_module->tokenizer);
  parser->curr_token = token;
}

int _get_op_precedence(parser *parser) {
  if (parser->curr_token.type != TOKEN_OP) return -1;
  int op_precedence = (*parser->op_precedences)[parser->curr_token.op_val];
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
  auto v = parse_exp(parser, parent);
  if (!v) return 0;
  if (parser->curr_token.op_val != ')')
    return (exp_node *)log(ERROR, "expected ')'");
  parse_next_token(parser);
  return v;
}

exp_node *_parse_function_app_or_def(parser *parser, exp_node* parent, source_loc loc, string id_name) {
  if (parser->curr_token.op_val == '(') parse_next_token(parser);  // skip '('
  auto func_definition = false;
  vector<exp_node *> args;
  //doesn't allow function inside parameter or argument
  //will be enhanced later
  parser->allow_id_as_a_func = false;
  if (parser->curr_token.op_val != ')') {
    while (true) {
      if (auto arg = parse_exp(parser, parent)){
        args.push_back(arg);
      }
      if (parser->curr_token.op_val == '=') {
        func_definition = true;
        break;
      }
      else if (parser->curr_token.op_val == ')'||parser->curr_token.type == TOKEN_EOS||parser->curr_token.type == TOKEN_EOF)
        break;
      else if (parser->curr_token.op_val == ',')
        parse_next_token(parser);
    }
  }
  parser->allow_id_as_a_func = true;
  //log(DEBUG, "is %s a function def: %d", id_name.c_str(), func_definition);
  if (func_definition) {
    vector<string> argNames;
    for (auto exp : args) {
      auto id = (ident_node *)exp;
      argNames.push_back(id->name);
    }
    prototype_node *prototype =
        create_prototype_node(parent, loc, id_name, argNames, false, 0);
    return _parse_function_with_prototype(parser, prototype);
  }
  // function application
  //log(DEBUG, "funcation application: %s", id_name.c_str());
  exp_node* call_node = (exp_node *)_create_call_node(parent, loc, id_name, args);
  return parse_exp(parser, parent, call_node);
}

exp_node *parse_statement(parser *parser, exp_node *parent) {
  exp_node *node;
  if (parser->curr_token.type == TOKEN_EOF)
    return nullptr;
  else if (parser->curr_token.type == TOKEN_IMPORT)
    node = parse_import(parser, parent);
  else if (parser->curr_token.type == TOKEN_UNARY || parser->curr_token.type == TOKEN_BINARY){
    //function def
    auto proto = _parse_prototype(parser, parent);
    node = _parse_function_with_prototype(parser, (prototype_node*)proto);
  }
  else if (parser->curr_token.type == TOKEN_IDENT){ 
    string id_name = *parser->curr_token.ident_str;
    source_loc loc = parser->curr_token.loc;
    parse_next_token(parser);  // skip identifier
    char op = parser->curr_token.op_val;
    //log(DEBUG, "id token: %s, %c, %d, %d", id_name.c_str(), op, op, parent);
    if (op == '=') {
      // variable definition
      node = _parse_var(parser, parent, id_name);
    } else if (parser->curr_token.type == TOKEN_EOS || parser->curr_token.type == TOKEN_EOF ||g_op_precedences[op]) {
      // just id expression evaluation
      auto lhs = (exp_node *)_create_ident_node(parent, parser->curr_token.loc, id_name);
      node = parse_exp(parser, parent, lhs);
      //log(DEBUG, "parsed exp: id exp: %d", node->type);
    } else {  
      // function definition or application
      node = _parse_function_app_or_def(parser, parent, loc, id_name);
    }
  }
  else{
    node = parse_exp(parser, parent);
    //log(DEBUG, "not id token exp: %s", NodeTypeString[node->type]);
  }
  if(node)
    node->parent = parent;
  return node;
}

bool _is_new_line(int cha){
  return cha == '\r' || cha == '\n';
}

bool _id_is_a_function_call(parser* parser){
  return parser->allow_id_as_a_func && (parser->curr_token.type == TOKEN_IDENT ||
    parser->curr_token.type == TOKEN_NUM ||
    parser->curr_token.type == TOKEN_IF ||
    parser->curr_token.type == TOKEN_UNARY || 
    parser->curr_token.op_val == '(');
}

exp_node *_parse_ident(parser *parser, exp_node* parent) {
  string id_name = *parser->curr_token.ident_str;
  source_loc loc = parser->curr_token.loc;
  
  parse_next_token(parser);             // take identifier
  if (_id_is_a_function_call(parser)) {  // pure variable
    // fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
    //(
    vector<exp_node *> args;
    while (true) {
      if (auto arg = parse_exp(parser, parent))
        args.push_back(arg);
      else
        return 0;
      if (!_id_is_a_function_call(parser)) break;

      parse_next_token(parser);
    }
    parse_next_token(parser);
    return (exp_node *)_create_call_node(parent, loc, id_name, args);
  }
  return (exp_node *)_create_ident_node(parent, parser->curr_token.loc, id_name);
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
  else if (parser->curr_token.op_val == '(')
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
    if(parser->curr_token.type==TOKEN_EOS) return lhs;
    int tok_prec = _get_op_precedence(parser);
    if (tok_prec < exp_prec) return lhs;
    int binary_op = parser->curr_token.op_val;
    parse_next_token(parser);
    auto rhs = _parse_unary(parser, parent);
    if (!rhs) return lhs;
    auto next_prec = _get_op_precedence(parser);
    if (tok_prec < next_prec) {
//      log(DEBUG, "right first %s", TokenTypeString[parser->curr_token.type]);
      rhs = _parse_binary(parser, parent, tok_prec + 1, rhs);
      if (!rhs) return 0;
    }
//    log(DEBUG, "left first: %s", TokenTypeString[parser->curr_token.type]);
    lhs = (exp_node *)_create_binary_node(parent, lhs->loc, binary_op, lhs, rhs);
  }
}

exp_node *parse_exp(parser *parser, exp_node* parent, exp_node *lhs) {
  if(parser->curr_token.type == TOKEN_EOS) return lhs;
  if (!lhs) lhs = _parse_unary(parser, parent);
  if (!lhs||parser->curr_token.type == TOKEN_EOS) return lhs;
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
  switch (parser->curr_token.type) {
    case TOKEN_IDENT:
      fun_name = *parser->curr_token.ident_str;
      proto_type = 0;
      // fprintf(stderr, "ident token in parse prototype: %s\n",
      // fun_name.c_str());
      parse_next_token(parser);
      break;
    case TOKEN_UNARY:
      //log(DEBUG, "found unary operator");
      parse_next_token(parser);
      if (parser->curr_token.type!=TOKEN_OP)
        return (exp_node *)log(ERROR, "Expected unary operator");
      fun_name = "unary";
      fun_name += (char)parser->curr_token.op_val;
      //log(DEBUG, "finding unary operator: %s", fun_name.c_str());
      proto_type = 1;
      parse_next_token(parser);
      break;
    case TOKEN_BINARY:
      parse_next_token(parser);
      if (parser->curr_token.type!=TOKEN_OP)
        return (exp_node *)log(ERROR, "Expected binary operator");
      fun_name = "binary";
      fun_name += (char)parser->curr_token.op_val;
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
  auto has_parenthese = parser->curr_token.op_val == '(';
  if (has_parenthese) parse_next_token(parser);  // skip '('
  std::vector<std::string> arg_names;
  while (parser->curr_token.type == TOKEN_IDENT) {
    // fprintf(stderr, "arg names: %s",
    // (*parser->curr_token.ident_str).c_str());
    arg_names.push_back(*parser->curr_token.ident_str);
    parse_next_token(parser);
  }
  if (has_parenthese && parser->curr_token.op_val != ')')
    return (exp_node *)log(ERROR, "Expected ')' to match '('");
  // success.
  if (has_parenthese) parse_next_token(parser);  // eat ')'.
  // Verify right number of names for operator.
  if (proto_type && arg_names.size() != proto_type)
    return (exp_node *)log(ERROR, "Invalid number of operands for operator");
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

exp_node *_parse_var(parser *parser, exp_node* parent, string &name) {
  if (parser->curr_token.op_val == '=')
    parse_next_token(parser);  // skip '='
                               // token
  if (auto exp = parse_exp(parser, parent)) {
    // not a function but a value
    //log(INFO, "_parse_variable:  %lu!", var_names.size());
    return (exp_node *)_create_var_node(parent, parser->curr_token.loc, name, exp);
  }
  return 0;
}

exp_node *parse_exp_to_function(parser *parser, exp_node *exp, const char *fn) {
  if (!exp) exp = parse_exp(parser, nullptr);
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
  if(parser->curr_token.type == TOKEN_RANGE
    ||parser->curr_token.type == TOKEN_EOS
    ||parser->curr_token.type == TOKEN_EOF)
    return 0;
  auto loc = parser->curr_token.loc;
  if (parser->curr_token.type != TOKEN_OP || parser->curr_token.op_val == '(' ||
      parser->curr_token.op_val == ',') {
    return _parse_node(parser, parent);
  }
  //log(DEBUG, "unary: %c", parser->curr_token.op_val);
  // If this is a unary operator, read it.
  int opc = parser->curr_token.op_val;
  parse_next_token(parser);
  if (exp_node *operand = _parse_unary(parser, parent)){
    //log(DEBUG, "unary node:%c: %s", opc, NodeTypeString[operand->type]);
    return (exp_node *)_create_unary_node(parent, loc, opc, operand);
  }
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

  exp_node *start = parse_exp(parser, parent);
  if (start == 0) return 0;
  if (parser->curr_token.type != TOKEN_RANGE)
    return (exp_node *)log(ERROR, "expected '..' after for start value");
  parse_next_token(parser);

  //step or end
  exp_node *end_val = parse_exp(parser, parent);
  if (end_val == 0) return 0;

  // The step value is optional.
  exp_node *step = 0;
  if (parser->curr_token.type == TOKEN_RANGE) {
    step = end_val;
    parse_next_token(parser);
    end_val = parse_exp(parser, parent);
    if (end_val == 0) return 0;
  }else{//default 1
    step = (exp_node*)_create_num_node(parent, parser->curr_token.loc, 1);
  }
  //convert end variable to a logic
  auto id_node = (exp_node*)_create_ident_node(parent, start->loc, id_name);
  auto end = (exp_node*)_create_binary_node(parent, end_val->loc, '<', id_node, end_val);
  while(parser->curr_token.type == TOKEN_EOS)
    parse_next_token(parser);

  exp_node *body = parse_exp(parser, parent);
  if (body == 0) return 0;
  return (exp_node *)_create_for_node(parent, loc, id_name, start, end, step, body);
}

exp_node *_parse_if(parser *parser, exp_node* parent) {
  source_loc loc = parser->curr_token.loc;
  parse_next_token(parser);  // eat the if.

  // condition.
  //log(DEBUG, "parsing if exp");
  exp_node *cond = parse_exp(parser, parent);
  if (!cond) return 0;

  if (parser->curr_token.type == TOKEN_THEN)
    parse_next_token(parser);  // eat the then
  while (parser->curr_token.type==TOKEN_EOS) parse_next_token(parser);
  //log(DEBUG, "parsing then exp");
  exp_node *then = parse_exp(parser, parent);
  if (then == 0) return 0;

  while (parser->curr_token.type==TOKEN_EOS) parse_next_token(parser);
  if (parser->curr_token.type != TOKEN_ELSE)
    return (exp_node *)log(ERROR, "expected else, got type: %d", parser->curr_token.type);

  parse_next_token(parser);

  //log(DEBUG, "parsing else exp");
  exp_node *else_exp = parse_exp(parser, parent);
  if (!else_exp) return 0;

  //log(DEBUG, "creating if nodes");
  return (exp_node *)_create_if_node(parent, loc, cond, then, else_exp);
}

block_node *parse_block(parser *parser, exp_node *parent, void (*fun)(void*, exp_node*), void* jit) {
  int col = parent? parent->loc.col : 1;
  vector<exp_node*> nodes;
  while (true) {
    parse_next_token(parser);
    //log(DEBUG, "got token in block: (%d, %d), %d", parser->curr_token.loc.line, parser->curr_token.loc.col, parser->curr_token.type);
    if (exit_block(parser, parent, col)) {
      //repeat the token
      repeat_token(parser->current_module->tokenizer);
      break;
    }
    static source_loc loc1 = parser->curr_token.loc;
    auto node = parse_statement(parser, parent);
    //log(DEBUG, "parsed statement in block: (%d, %d), %s", loc1.line, loc1.col, node?NodeTypeString[node->type]:"null node");
    if (node)
      nodes.push_back(node);
    if(fun){
      (*fun)(jit, node);
    }
    if (exit_block(parser, parent, col)) {
      break;
    }
    if (parser->curr_token.type == TOKEN_EOF) break;
    //   //log(DEBUG, "is exp: %d", _is_exp(node));
    //   if(_is_exp(node)) {
    //     //if exp returning it
    //     break;
    //   }
    // }
  }
  //log(DEBUG, "parsed statements: %d", nodes.size());
  return nodes.size() > 0 ? _create_block_node(parent, nodes) : nullptr;
}