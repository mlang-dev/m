#include <string>

enum TokenType {
    TokenEof = -1,
    //primary
    TokenIdent = -2,
    TokenNum = -3,
    
    //command
    TokenLet = -4,
    TokenImport = -5,
    
    //control
    TokenIf = -6,
    TokenThen = -7,
    TokenElse = -8,
    TokenFor = -9,
    TokenIn = -10,
    
    //operators
    TokenUnary = -11,
    TokenBinary = -12,
    
    //var
    TokenVar = -13,
    
    //others
    TokenOp = -14,
};

struct Token{
    TokenType type;
    union{
        std::string* ident_str;
        double num_val;
        int op_val;
    };
};

Token& get_token();


