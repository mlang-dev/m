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
    create_module_and_pass_manager(cg);
    while(true){
        fprintf(stderr, "m> ");
        parse_next_token(parser);
        //fprintf(stderr, "got token: token type: %d\n", parser->curr_token.type);
        if (parser->curr_token.type == TOKEN_EOF){
            //dumpm(jit->cg->module.get());
            fprintf(stderr, "bye !\n");
            break;
        }
        switch(parser->curr_token.type){
            case TOKEN_IMPORT:{
                if (auto node= parse_import(parser)){
                    parser->ast->entry_module->nodes.push_back(node);
                    if(auto v = generate_code(cg, node)){
                        //dumpf(v);
                        //fprintf(stderr, "Parsed an import\n");
                    }
                }
                break;
            }
            default:{
                if(auto node=parse_exp_or_def(parser)){
                    if (node->type != NodeType::FUNCTION_NODE){
                        auto fn = make_unique_name("main-fn");
                        auto node_type = node->type;
                        node = parse_exp_to_function(parser, node, fn.c_str());
                        if(node){
                            if(auto p_fun = generate_code(cg, node)){
                                //dumpm(jit->cg->module.get());
                                auto mk = jit->mjit->addModule(std::move(jit->cg->module));
                                auto mf = jit->mjit->findSymbol(fn);
                                double (*fp)() = (double (*)())(intptr_t)cantFail(mf.getAddress());
                                fprintf(stderr, "%f\n", fp());
                                if (node_type != NodeType::VAR_NODE) //keep global variables in the jit
                                    jit->mjit->removeModule(mk);
                                create_module_and_pass_manager(cg);
                            }
                        }
                    }else{
                        //log(DEBUG, "it's a definition. %d", node->type);
                        auto def = generate_code(cg, node);
                        //dumpm(jit->cg->module.get());
                        jit->mjit->addModule(std::move(jit->cg->module));
                        create_module_and_pass_manager(jit->cg);
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