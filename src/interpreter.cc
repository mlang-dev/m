#include "jit.h"

void run(){
    parser* parser = create_parser();
    code_generator* cg = create_code_generator(parser);
    JIT* jit = create_jit(cg);
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
                if (auto node = parse_function(parser)){
                    if(auto v = generate_code(cg, node)){
                        dump(v);
                        //fprintf(stderr, "codegen a %d node.\n", node->type);
                    }
                }
                break;
            }
            case TOKEN_IMPORT:{
                if (auto node= parse_import(parser)){
                    if(auto v = generate_code(cg, node)){
                        dump(v);
                        //fprintf(stderr, "Parsed an import\n");
                    }
                }
                break;
            }
            case TOKEN_OP:{
                if (parser->curr_token.op_val == ';'){
                    break;
                }
            }
            default:{
                //fprintf(stderr, "parsing exp to function: token type: %d, %f\n", parser->curr_token.type, parser->curr_token.num_val);
                if(auto node=parse_exp_to_function(parser)){
                    if(auto p_fun = generate_code(cg, node)){
                        void* ptr = get_pointer_to_function(jit, p_fun);
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
    }
    destroy_jit(jit);
    destroy_code_generator(cg);
    destroy_parser(parser);
}