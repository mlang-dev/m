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

static map<string, TokenType> tokens = {
    {"import", TOKEN_IMPORT}, {"if", TOKEN_IF},         {"else", TOKEN_ELSE},
    {"then", TOKEN_THEN},     {"in", TOKEN_IN},         {"for", TOKEN_FOR},
    {"unary", TOKEN_UNARY},   {"binary", TOKEN_BINARY}, {"..", TOKEN_RANGE},
};

static map<char, TokenType> char_tokens = {
  {'(', TOKEN_LPAREN}, {')', TOKEN_RPAREN}, {'[', TOKEN_LBRACKET}, {']', TOKEN_RBRACKET},
};

static set<char> symbol_chars = {'.'};

static int get_char(file_tokenizer* tokenizer) {
  int last_char = getc(tokenizer->file);
  if (is_new_line(last_char)) {
    tokenizer->loc.line++;
    tokenizer->loc.col = 0;
  } else
    tokenizer->loc.col++;
  return last_char;
}

file_tokenizer* create_tokenizer(FILE* file){
  auto tokenizer = new file_tokenizer();
  tokenizer->loc = {1, 0};
  tokenizer->next_token = {.type = TOKEN_UNK};
  tokenizer->curr_char = ' ';
  tokenizer->file = file;
  return tokenizer;
}

void destroy_tokenizer(file_tokenizer* tokenizer){
  fclose(tokenizer->file);
  delete tokenizer;
}

token& _tokenize_symbol_type(file_tokenizer* tokenizer, token& t, TokenType type) {
  t.type = type;
  t.loc = tokenizer->tok_loc;
  return t;
}

void _tokenize_symbol(file_tokenizer* tokenizer, string& symbol) {
  symbol = "";
  do {
    if (symbol_chars.find(tokenizer->curr_char) != symbol_chars.end())
      symbol += tokenizer->curr_char;
    else
      break;
  } while ((tokenizer->curr_char = get_char(tokenizer)));
}

token& _tokenize_number(file_tokenizer* tokenizer) {
  string num_str = "";
  do {
    string symbol;
    _tokenize_symbol(tokenizer, symbol);
    if (auto type = tokens[symbol]) {
      if (num_str == "") {
        return _tokenize_symbol_type(tokenizer, tokenizer->cur_token, type);
      } else {
        //log(ERROR, "ERROROROR !!!, %s", symbol.c_str());
        _tokenize_symbol_type(tokenizer, tokenizer->next_token, type);
        break;
      }
    }
    num_str += tokenizer->curr_char;
    tokenizer->curr_char = get_char(tokenizer);
  } while (isdigit(tokenizer->curr_char) || tokenizer->curr_char == '.');
  tokenizer->cur_token.num_val = strtod(num_str.c_str(), nullptr);
  tokenizer->cur_token.type = TOKEN_NUM;
  tokenizer->cur_token.loc = tokenizer->tok_loc;
  return tokenizer->cur_token;
}

token& _tokenize_id_keyword(file_tokenizer* tokenizer) {
  tokenizer->ident_str = tokenizer->curr_char;
  while (isalnum((tokenizer->curr_char = get_char(tokenizer))) || tokenizer->curr_char == '_') 
    tokenizer->ident_str += tokenizer->curr_char;
  auto token_type = tokens[tokenizer->ident_str];
  tokenizer->cur_token.type = token_type != 0 ? token_type : TOKEN_IDENT;
  tokenizer->cur_token.ident_str = &tokenizer->ident_str;
  tokenizer->cur_token.loc = tokenizer->tok_loc;
  //log(DEBUG, "id: %s, %d", tokenizer->ident_str.c_str(), tokenizer->cur_token.type);
  return tokenizer->cur_token;
}

token& _tokenize_type(file_tokenizer* tokenizer, TokenType token_type) {
  tokenizer->cur_token.loc = tokenizer->tok_loc;
  tokenizer->cur_token.op_val = tokenizer->curr_char;
  tokenizer->cur_token.type = token_type;
  return tokenizer->cur_token;
}

void _skip_to_line_end(file_tokenizer* tokenizer) {
  do
    tokenizer->curr_char = get_char(tokenizer);
  while (tokenizer->curr_char != EOF && !is_new_line(tokenizer->curr_char));
}


token& get_token(file_tokenizer* tokenizer) {
  // skip spaces
  if (tokenizer->next_token.type) {
    // cleanup looked ahead tokens
    tokenizer->cur_token = tokenizer->next_token;
    tokenizer->next_token.type = TOKEN_UNK;
    return tokenizer->cur_token;
  }
  while (isspace(tokenizer->curr_char)) {
    if (is_new_line(tokenizer->curr_char)) break;
    tokenizer->curr_char = get_char(tokenizer);
  }

  tokenizer->tok_loc = tokenizer->loc;
  //log(DEBUG, "skiped spaces: %d, %d, %c", tokenizer->tok_loc.line, tokenizer->tok_loc.col, tokenizer->curr_char);
  if (tokenizer->curr_char == EOF)
    return _tokenize_type(tokenizer, TOKEN_EOF);
  else if (is_new_line(tokenizer->curr_char)) {
    _tokenize_type(tokenizer, TOKEN_EOS);
    tokenizer->curr_char = ' ';  // replaced with empty space
    return tokenizer->cur_token;
  } else if (isalpha(tokenizer->curr_char)) {
    return _tokenize_id_keyword(tokenizer);
  } else if (isdigit(tokenizer->curr_char) || tokenizer->curr_char == '.') {
    return _tokenize_number(tokenizer);
  } else if (tokenizer->curr_char == '#') {
    // skip comments
    _skip_to_line_end(tokenizer);
    if (tokenizer->curr_char != EOF) return get_token(tokenizer);
    else
      return _tokenize_type(tokenizer, TOKEN_EOF);
  }
  auto token_type = char_tokens[tokenizer->curr_char];
  if (!token_type)
    token_type = TOKEN_OP;
  _tokenize_type(tokenizer, token_type);
  tokenizer->curr_char = get_char(tokenizer);
  return tokenizer->cur_token;
}
