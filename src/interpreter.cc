#include "jit.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"


void run(){
    llvm::LLVMContext context;
    Parser* parser = new Parser();
    llvm::IRBuilder<>* builder = new llvm::IRBuilder<>(context);
    CodeGenerator* cg = createCodeGenerator(&context, builder, parser);
    JIT* jit = new JIT(cg);
    fprintf(stderr, "m> ");
    while(1){
        parser->AdvanceToNextToken();
        if (parser->_curr_token.type == TokenEof){
            fprintf(stderr, "eof done");
            break;
        }
        switch(parser->_curr_token.type){
            case TokenLet:{
                //fprintf(stderr, "parsing function...");
                if (auto node = parser->ParseFunction()){
                    if(auto v = ((llvm::Function*)generateFunctionNode(cg, node))){
                        dump(v);
                        fprintf(stderr, "Parsed a function definition\n");
                    }
                }
                break;
            }
            case TokenImport:{
                if (auto node= parser->ParseImport()){
                    if(auto v = ((llvm::Function*)generatePrototypeNode(cg, node))){
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
                if(auto node=parser->ParseExpToFunction()){
                    if(auto p_fun = ((llvm::Function*)generateFunctionNode(cg, node))){
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
        }
        fprintf(stderr, "m> ");
    }
    delete jit;
    delete builder;
    free(cg);
    delete parser;
}