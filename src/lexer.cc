#include "lexer.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "util.h"

using namespace std;

static source_loc loc = {1, 0};
static source_loc tok_loc;

static int get_char(FILE* file) {
  int last_char = getc(file);
  if (is_new_line(last_char)) {
    loc.line++;
    loc.col = 0;
    //log(DEBUG, "hey new line");
  } else
    loc.col++;
  return last_char;
}

token& get_token(FILE* file) {
  static map<string, TokenType> tokens;
  static token token;
  static int curr_char = ' ';
  static std::string ident_str;
  //tokens["let"] = TOKEN_LET;
  //tokens["var"] = TOKEN_VAR;
  //tokens["exit"] = TOKEN_EOF;
  tokens["import"] = TOKEN_IMPORT;
  tokens["if"] = TOKEN_IF;
  tokens["else"] = TOKEN_ELSE;
  tokens["then"] = TOKEN_THEN;
  tokens["in"] = TOKEN_IN;
  tokens["for"] = TOKEN_FOR;
  tokens["binary"] = TOKEN_BINARY;
  tokens["unary"] = TOKEN_UNARY;

  // skip spaces
  //log(DEBUG, "getting token");
  while (isspace(curr_char)) {
    if(is_new_line(curr_char)) break;
    // fprintf(stderr, "space char: %d.", curr_char);
    curr_char = get_char(file);
  }

  tok_loc = loc;
  // log(DEBUG, "skiping space - done\n");
  if(is_new_line(curr_char)){
    token.loc = tok_loc;
    token.op_val = curr_char;
    token.type = TOKEN_EOS;
    curr_char = ' '; //replaced with empty space
    return token;
  } 
  else if (isalpha(curr_char)) {
    ident_str = curr_char;
    while (isalnum((curr_char = get_char(file)))) ident_str += curr_char;
    // fprintf(stderr, "id token: %s\n", ident_str.c_str());
    auto token_type = tokens[ident_str];
    token.type = token_type != 0 ? token_type : TOKEN_IDENT;
    token.ident_str = &ident_str;
    token.loc = tok_loc;
    return token;
  } else if (isdigit(curr_char) || curr_char == '.') {
    std::string num_str;
    do {
      num_str += curr_char;
      curr_char = get_char(file);
    } while (isdigit(curr_char) || curr_char == '.');
    token.num_val = strtod(num_str.c_str(), nullptr);
    token.type = TOKEN_NUM;
    token.loc = tok_loc;
    return token;
  } else if (curr_char == '#') {  // skip comments
    do
      curr_char = get_char(file);
    while (curr_char != EOF && !is_new_line(curr_char));
    if(is_new_line(curr_char))
      curr_char = ' ';//eaten for next get
    if (curr_char != EOF) 
      return get_token(file);
  } 
  token.loc = tok_loc;
  token.op_val = curr_char;
  token.type = curr_char == EOF ? TOKEN_EOF : TOKEN_OP;
  curr_char = get_char(file);
  return token;
}
