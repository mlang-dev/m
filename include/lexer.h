#include <string>

enum TokenType {
    TOKEN_EOF = -1,
    //primary
    TOKEN_IDENT = -2,
    TOKEN_NUM = -3,
    
    //command
    TOKEN_LET = -4,
    TOKEN_IMPORT = -5,
    
    //control
    TOKEN_IF = -6,
    TOKEN_THEN = -7,
    TOKEN_ELSE = -8,
    TOKEN_FOR = -9,
    TOKEN_IN = -10,
    
    //operators
    TOKEN_UNARY = -11,
    TOKEN_BINARY = -12,
    
    //var
    TOKEN_VAR = -13,
    
    //others
    TOKEN_OP = -14,
};

struct token{
    TokenType type;
    int indent;
    union{
        std::string* ident_str;
        double num_val;
        int op_val;
    };
};

token& get_token(FILE* file);


