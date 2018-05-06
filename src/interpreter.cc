#include "jit.h"

extern "C" double putchard(double X) {
    putchar((char)X);
    return 10.01;
}

void Run(){
    Parser* parser = new Parser();
    LLVMCodeGenerator* code_generator = new LLVMCodeGenerator(parser);
    JIT* jit = new JIT(code_generator);
    fprintf(stderr, "m> ");
    while(1){
        parser->AdvanceToNextToken();
        if (parser->_curr_token.type == TokenEof){
            fprintf(stderr, "eof done");
            break;
        }
        switch(parser->_curr_token.type){
            case TokenLet:
                //fprintf(stderr, "parsing function...");
                if (auto node = parser->ParseFunction()){
                    if(auto v = ((llvm::Function*)node->codegen(code_generator))){
                        v->dump();
                        fprintf(stderr, "Parsed a function definition\n");
                    }
                }
                break;
            case TokenImport:
                if (auto node= parser->ParseImport()){
                    if(auto v = ((llvm::Function*)node->codegen(code_generator))){
                        v->dump();
                        fprintf(stderr, "Parsed an import\n");
                    }
                }
                break;
            case TokenOp:
                if (parser->_curr_token.op_val == ';'||parser->_curr_token.op_val == '\r' || parser->_curr_token.op_val == '\n')
                    break;
            default:
                if(auto node=parser->ParseExpToFunction()){
                    if(auto p_fun = ((llvm::Function*)node->codegen(code_generator))){
                        void* ptr = jit->GetPointerToFunction(p_fun);
                        if(ptr){
                            double (*fun)() = (double (*)())(intptr_t)ptr;
                            fprintf(stderr, "%f\n", fun());
                        }else
                            fprintf(stderr, "cannot evaluate the expression\n");
                    }
                }
                break;
        }
        fprintf(stderr, "m> ");
    }
    code_generator->Dump();
    delete jit;
    delete code_generator;
    delete parser;
}