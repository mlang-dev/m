#include <cctype>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include "lexer.h"


Token& GetToken(){
    static std::map<std::string, TokenType> tokens;
    static Token token;
    static int curr_char = ' ';
    static std::string ident_str;
    tokens["let"] = TokenLet;
    tokens["import"] = TokenImport;
    tokens["exit"] = TokenEof;
    tokens["if"] = TokenIf;
    tokens["else"] = TokenElse;
    tokens["then"] = TokenThen;
    tokens["in"] = TokenIn;
    tokens["for"] = TokenFor;
    tokens["binary"] = TokenBinary;
    tokens["unary"] = TokenUnary;
    tokens["var"] = TokenVar;
    if (curr_char == '\n' || curr_char == '\r'){
        token.type = TokenOp;
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
        token.type = token_type != 0 ? token_type: TokenIdent;
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
        token.type = TokenNum;
        return token;
    }

    if (curr_char == '#' || curr_char == ';') {
        do
            curr_char = getchar();
        while(curr_char!=EOF && curr_char != '\n' && curr_char != '\r');

        if (curr_char!=EOF)
            return GetToken();
    }
  
    if(curr_char == EOF){
        token.type = TokenEof;
        return token;
    }

    token.op_val = curr_char;
    token.type  = TokenOp;
    curr_char = getchar();
    return token;
}
