#include "lexer.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "util.h"

using namespace std;

static source_loc loc = {1, 0};
static source_loc tok_loc;
static token _token;
static token _next_token = {.type = TOKEN_UNK};
static int curr_char = ' ';
static string ident_str;
static map<string, TokenType> tokens = {
    {"import", TOKEN_IMPORT}, {"if", TOKEN_IF},         {"else", TOKEN_ELSE},
    {"then", TOKEN_THEN},     {"in", TOKEN_IN},         {"for", TOKEN_FOR},
    {"unary", TOKEN_UNARY},   {"binary", TOKEN_BINARY}, {"..", TOKEN_RANGE},
};
static set<char> symbol_chars = {'.'};

static int get_char(FILE* file) {
  int last_char = getc(file);
  if (is_new_line(last_char)) {
    loc.line++;
    loc.col = 0;
  } else
    loc.col++;
  return last_char;
}

token& _tokenize_symbol_type(token& t, TokenType type) {
  t.type = type;
  t.loc = tok_loc;
  return t;
}

void _tokenize_symbol(FILE* file, string& symbol) {
  symbol = "";
  do {
    if (symbol_chars.find(curr_char) != symbol_chars.end())
      symbol += curr_char;
    else
      break;
  } while ((curr_char = get_char(file)));
}

token& _tokenize_number(FILE* file) {
  string num_str = "";
  do {
    string symbol;
    _tokenize_symbol(file, symbol);
    if (auto type = tokens[symbol]) {
      if (num_str == "") {
        return _tokenize_symbol_type(_token, type);
      } else {
        _tokenize_symbol_type(_next_token, type);
        break;
      }
    }
    num_str += curr_char;
    curr_char = get_char(file);
  } while (isdigit(curr_char) || curr_char == '.');
  _token.num_val = strtod(num_str.c_str(), nullptr);
  _token.type = TOKEN_NUM;
  _token.loc = tok_loc;
  return _token;
}

token& _tokenize_id_keyword(FILE* file) {
  ident_str = curr_char;
  while (isalnum((curr_char = get_char(file)))) ident_str += curr_char;
  auto token_type = tokens[ident_str];
  _token.type = token_type != 0 ? token_type : TOKEN_IDENT;
  _token.ident_str = &ident_str;
  _token.loc = tok_loc;
  return _token;
}

token& _tokenize_eos() {
  _token.loc = tok_loc;
  _token.op_val = curr_char;
  _token.type = TOKEN_EOS;
  curr_char = ' ';  // replaced with empty space
  return _token;
}

token& _tokenize_eof() {
  _token.loc = tok_loc;
  _token.op_val = curr_char;
  _token.type = TOKEN_EOF;
  return _token;
}

token& _tokenize_operator(FILE* file) {
  _token.loc = tok_loc;
  _token.op_val = curr_char;
  _token.type = TOKEN_OP;
  return _token;
}

void _skip_to_line_end(FILE* file) {
  do
    curr_char = get_char(file);
  while (curr_char != EOF && !is_new_line(curr_char));
  if (is_new_line(curr_char)) curr_char = ' ';  // eaten for next get
}

void repeat_token(){
  _next_token = _token;
}

token& get_token(FILE* file) {
  // skip spaces
  if (_next_token.type) {
    // cleanup looked ahead tokens
    _token = _next_token;
    _next_token.type = TOKEN_UNK;
    return _token;
  }
  while (isspace(curr_char)) {
    if (is_new_line(curr_char)) break;
    curr_char = get_char(file);
  }

  tok_loc = loc;
  //log(DEBUG, "skiped spaces: %d, %d", tok_loc.line, tok_loc.col);
  if (curr_char == EOF)
    return _tokenize_eof();
  else if (is_new_line(curr_char)) {
    return _tokenize_eos();
  } else if (isalpha(curr_char)) {
    return _tokenize_id_keyword(file);
  } else if (isdigit(curr_char) || curr_char == '.') {
    return _tokenize_number(file);
  } else if (curr_char == '#') {
    // skip comments
    _skip_to_line_end(file);
    if (curr_char != EOF) return get_token(file);
    else
      return _tokenize_eof();
  }
  _tokenize_operator(file);
  curr_char = get_char(file);
  return _token;
}
