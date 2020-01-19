#include "jit.h"

void run(){
    Parser* parser = createParser();
    CodeGenerator* cg = createCodeGenerator(parser);
    JIT* jit = createJIT(cg);
    fprintf(stderr, "m> ");
    while(1){
        AdvanceToNextToken(parser);
        if (parser->_curr_token.type == TokenEof){
            fprintf(stderr, "eof done");
            break;
        }
        switch(parser->_curr_token.type){
            case TokenLet:{
                //fprintf(stderr, "parsing function...");
                if (auto node = ParseFunction(parser)){
                    if(auto v = generateFunctionNode(cg, node)){
                        dump(v);
                        fprintf(stderr, "Parsed a function definition\n");
                    }
                }
                break;
            }
            case TokenImport:{
                if (auto node= ParseImport(parser)){
                    if(auto v = generatePrototypeNode(cg, node)){
                        dump(v);
                        fprintf(stderr, "Parsed an import\n");
                    }
                }
                break;
            }
            case TokenOp:{
                if (parser->_curr_token.op_val == ';'||parser->_curr_token.op_val == '\r' || parser->_curr_token.op_val == '\n')
                    break;
            }
            default:{
                if(auto node=ParseExpToFunction(parser)){
                    if(auto p_fun = generateFunctionNode(cg, node)){
                        void* ptr = GetPointerToFunction(jit, p_fun);
                        if(ptr){
                            double (*fun)() = (double (*)())(intptr_t)ptr;
                            fprintf(stderr, "%f\n", fun());
                        }else
                            fprintf(stderr, "cannot evaluate the expression\n");
                    }
                }
                break;
            }
        }
        fprintf(stderr, "m> ");
    }
    destroyJIT(jit);
    destroyCodeGenerator(cg);
    destroyParser(parser);
}