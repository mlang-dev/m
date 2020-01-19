#include <cctype>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include "lexer.h"


token& get_token(){
    static std::map<std::string, TokenType> tokens;
    static token token;
    static int curr_char = ' ';
    static std::string ident_str;
    tokens["let"] = TOKEN_LET;
    tokens["import"] = TOKEN_IMPORT;
    tokens["exit"] = TOKEN_EOF;
    tokens["if"] = TOKEN_IF;
    tokens["else"] = TOKEN_ELSE;
    tokens["then"] = TOKEN_THEN;
    tokens["in"] = TOKEN_IN;
    tokens["for"] = TOKEN_FOR;
    tokens["binary"] = TOKEN_BINARY;
    tokens["unary"] = TOKEN_UNARY;
    tokens["var"] = TOKEN_VAR;
    if (curr_char == '\n' || curr_char == '\r'){
        token.type = TOKEN_OP;
        token.op_val = curr_char;
        curr_char = ' ';
        return token;
    }
    while (isspace(curr_char))
        curr_char = getchar();

    if (isalpha(curr_char)){
        ident_str = curr_char;
        while(isalnum((curr_char = getchar())))
            ident_str += curr_char;
        auto token_type = tokens[ident_str];
        token.type = token_type != 0 ? token_type: TOKEN_IDENT;
        token.ident_str = &ident_str;
        return token;
    }
    
    if (isdigit(curr_char) || curr_char == '.'){
        std::string num_str;
        do{
            num_str += curr_char;
            curr_char = getchar();
        }while(isdigit(curr_char)||curr_char == '.');
        token.num_val = strtod(num_str.c_str(), nullptr);
        token.type = TOKEN_NUM;
        return token;
    }

    if (curr_char == '#' || curr_char == ';') {
        do
            curr_char = getchar();
        while(curr_char!=EOF && curr_char != '\n' && curr_char != '\r');

        if (curr_char!=EOF)
            return get_token();
    }
  
    if(curr_char == EOF){
        token.type = TOKEN_EOF;
        return token;
    }

    token.op_val = curr_char;
    token.type  = TOKEN_OP;
    curr_char = getchar();
    return token;
}
