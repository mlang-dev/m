/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m repl, interactive JIT running environment
 */
#include "jit.h"
#include "llvm/IR/Module.h"

#include "env.h"


llvm::orc::VModuleKey _add_module_to_jit(JIT* jit)
{
    std::unique_ptr<llvm::Module> module((llvm::Module*)jit->cg->module);
    jit->cg->module = nullptr;
    return jit->mjit->addModule(std::move(module));
}

void _create_jit_module(code_generator *cg)
{
    string mod_name = make_unique_name("mjit");
    create_module_and_pass_manager(cg, string_get(&mod_name));
    string_deinit(&mod_name);
}

double eval_exp(JIT* jit, exp_node* node)
{
    // expression: statement or expression evalution
    string fn = make_unique_name("main-fn");
    auto node_type = node->node_type;
    double result;
    node = parse_exp_to_function(jit->cg->parser, node, string_get(&fn));
    if (node) {
        if (auto p_fun = generate_code(jit->cg, node)) {
            auto mk = _add_module_to_jit(jit);
            auto mf = jit->mjit->findSymbol(std::string(string_get(&fn)));
            //LLVMDumpModule(module);
            double (*fp)() = (double (*)())(intptr_t)cantFail(mf.getAddress());
            // keep global variables in the jit
            result = fp();
            if (node_type != NodeType::VAR_NODE) {
                jit->mjit->removeModule(mk);
            }
            _create_jit_module(jit->cg);
        }
    }
    string_deinit(&fn);
    return result;
}

void eval_statement(void* p_jit, exp_node* node)
{
    if (node) {
        auto jit = (JIT*)p_jit;
        if (node->node_type == NodeType::PROTOTYPE_NODE)
            generate_code(jit->cg, node);
        else if (node->node_type == NodeType::FUNCTION_NODE) {
            // function definition
            auto def = generate_code(jit->cg, node);
            _add_module_to_jit(jit);
            _create_jit_module(jit->cg);
        } else {
            auto result = eval_exp(jit, node);
            if (node->node_type != NodeType::VAR_NODE)
                printf("%f\n", result);
        }
    }
    fprintf(stderr, "m> ");
}

JIT* build_jit(menv* env, parser* parser)
{
    code_generator* cg = cg_new(env, parser);
    JIT* jit = jit_new(cg);
    //log_info(DEBUG, "creating builtins");
    create_builtins(parser, cg->context);
    //log_info(DEBUG, "creating jit modules");
    _create_jit_module(cg);
    //log_info(DEBUG, "generating runtime modules");
    generate_runtime_module(cg, parser);
    //log_info(DEBUG, "adding to jit");
    _add_module_to_jit(jit);
    //log_info(DEBUG, "creating jit modules 2");
    _create_jit_module(cg);
    return jit;
}

int run_repl()
{
    menv* env = env_new();
    parser* parser = parser_new(NULL, true, NULL);
    JIT* jit = build_jit(env, parser);
    printf("m> ");
    parse_block(parser, nullptr, &eval_statement, jit);
    printf("bye !\n");
    jit_free(jit);
    env_free(env);
    return 0;
}