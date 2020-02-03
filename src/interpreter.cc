#include "jit.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

int run(){
    parser* parser = create_parser(true);
    code_generator* cg = create_code_generator(parser);
    JIT* jit = create_jit(cg);
    create_builtins(parser, cg->context);
    generate_default_code(cg, parser);
    while(true){
        fprintf(stderr, "m> ");
        parse_next_token(parser);
        //fprintf(stderr, "got token: token type: %d\n", parser->curr_token.type);
        if (parser->curr_token.type == TOKEN_EOF){
            fprintf(stderr, "bye !\n");
            break;
        }
        switch(parser->curr_token.type){
            case TOKEN_LET:{
                //fprintf(stderr, "parsing function...");
                if (auto node = parse_function(parser, true)){
                    parser->ast->entry_module->nodes.push_back(node);
                    if(auto v = generate_code(cg, node)){
                        dump(v);
                        //fprintf(stderr, "codegen a %d node.\n", node->type);
                    }
                }
                break;
            }
            case TOKEN_IMPORT:{
                if (auto node= parse_import(parser)){
                    parser->ast->entry_module->nodes.push_back(node);
                    if(auto v = generate_code(cg, node)){
                        dump(v);
                        //fprintf(stderr, "Parsed an import\n");
                    }
                }
                break;
            }
            default:{
                //fprintf(stderr, "default: %d, %f\n", parser->curr_token.type, parser->curr_token.num_val);
                if(auto node=parse_exp_or_def(parser)){
                    if (node->type != NodeType::FUNCTION_NODE){
                        //log(DEBUG, "it is an expression.");
                        node = parse_exp_to_function(parser, node);
                        if(node){
                            if(auto p_fun = generate_code(cg, node)){
                                // fprintf(stderr, "generated: %d\n", node->type);
                                optimize_function(jit, p_fun);
                                void* ptr = get_fun_ptr_to_execute(jit, p_fun);
                                if(ptr){
                                    double (*fun)() = (double (*)())(intptr_t)ptr;
                                    fprintf(stderr, "%f\n", fun());
                                    // fprintf(stderr, "executed \n");
                                }else
                                    fprintf(stderr, "cannot evaluate the expression\n");
                            }
                        }
                    }else{
                        auto def = generate_code(cg, node);
                        dump(def);
                    }

                }
                break;
            }
        }
    }
    destroy_jit(jit);
    destroy_code_generator(cg);
    destroy_parser(parser);
    return 0;
}