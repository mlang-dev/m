#include "jit.h"

void run(){
    parser* parser = create_parser();
    code_generator* cg = create_code_generator(parser);
    JIT* jit = create_jit(cg);
    fprintf(stderr, "m> ");
    while(1){
        advance_to_next_token(parser);
        if (parser->_curr_token.type == TOKEN_EOF){
            fprintf(stderr, "eof done");
            break;
        }
        switch(parser->_curr_token.type){
            case TOKEN_LET:{
                //fprintf(stderr, "parsing function...");
                if (auto node = parse_function(parser)){
                    if(auto v = generate_function_node(cg, node)){
                        dump(v);
                        fprintf(stderr, "Parsed a function definition\n");
                    }
                }
                break;
            }
            case TOKEN_IMPORT:{
                if (auto node= parse_import(parser)){
                    if(auto v = generate_prototype_node(cg, node)){
                        dump(v);
                        fprintf(stderr, "Parsed an import\n");
                    }
                }
                break;
            }
            case TOKEN_OP:{
                if (parser->_curr_token.op_val == ';'||parser->_curr_token.op_val == '\r' || parser->_curr_token.op_val == '\n')
                    break;
            }
            default:{
                if(auto node=parse_exp_to_function(parser)){
                    if(auto p_fun = generate_function_node(cg, node)){
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
        fprintf(stderr, "m> ");
    }
    destroy_jit(jit);
    destroy_code_generator(cg);
    destroy_parser(parser);
}