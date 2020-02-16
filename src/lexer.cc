#include <cctype>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include "lexer.h"
#include "util.h"

using namespace std;

token& get_token(FILE* file){
  static map<string, TokenType> tokens;
  static token token;
  static int curr_char = ' ';
  static std::string ident_str;
  static int indent = 0; //ident spaces
  tokens["let"] = TOKEN_LET;
  tokens["var"] = TOKEN_VAR;
  tokens["import"] = TOKEN_IMPORT;
  tokens["exit"] = TOKEN_EOF;
  tokens["if"] = TOKEN_IF;
  tokens["else"] = TOKEN_ELSE;
  tokens["then"] = TOKEN_THEN;
  tokens["in"] = TOKEN_IN;
  tokens["for"] = TOKEN_FOR;
  tokens["binary"] = TOKEN_BINARY;
  tokens["unary"] = TOKEN_UNARY;

  //skip spaces
  //log(DEBUG, "skiping space\n");
  while (isspace(curr_char))
  {
    if (curr_char == '\t')
      indent += 2;
    else if (curr_char == '\r' || curr_char == '\n')
      indent = 0;
    else
      indent++;
    //fprintf(stderr, "space char: %d.", curr_char);
    curr_char = getc(file);
  }
  //log(DEBUG, "skiping space - done\n");
  if (isalpha(curr_char))
  {
    ident_str = curr_char;
    while (isalnum((curr_char = getc(file))))
      ident_str += curr_char;
    //fprintf(stderr, "id token: %s\n", ident_str.c_str());
    auto token_type = tokens[ident_str];
    token.type = token_type != 0 ? token_type : TOKEN_IDENT;
    token.ident_str = &ident_str;
    token.indent = indent;
    return token;
  }
  else if (isdigit(curr_char) || curr_char == '.')
  {
    std::string num_str;
    do
    {
      num_str += curr_char;
      curr_char = getc(file);
    } while (isdigit(curr_char) || curr_char == '.');
    token.num_val = strtod(num_str.c_str(), nullptr);
    //fprintf(stderr, "num token: %f\n", token.num_val);
    token.indent = indent;
    token.type = TOKEN_NUM;
    return token;
  }
  else if (curr_char == '#')
  { //skip comments
    do
      curr_char = getc(file);
    while (curr_char != EOF && curr_char != '\n' && curr_char != '\r');
    if (curr_char != EOF)
      return get_token(file);
  }
  else if (curr_char == EOF)
  {
    token.type = TOKEN_EOF;
    token.indent = indent;
    return token;
  }
  token.indent = indent;
  token.op_val = curr_char;
  token.type  = TOKEN_OP;
  curr_char = getc(file);
  indent = 0;
  //fprintf(stderr, "op token: %c, %c\n", token.op_val, curr_char);
  return token;
}
