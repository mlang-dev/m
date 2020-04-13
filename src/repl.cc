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
    unique_ptr<llvm::Module> module((llvm::Module*)jit->cg->module);
    jit->cg->module = nullptr;
    return jit->mjit->addModule(std::move(module));
}

double eval_exp(JIT* jit, exp_node* node)
{
    // expression: statement or expression evalution
    auto fn = make_unique_name("main-fn");
    auto node_type = node->node_type;
    double result;
    node = parse_exp_to_function(jit->cg->parser, node, fn.c_str());
    if (node) {
        if (auto p_fun = generate_code(jit->cg, node)) {
            auto mk = _add_module_to_jit(jit);
            auto mf = jit->mjit->findSymbol(fn);
            //dumpm(jit->cg->module.get());
            double (*fp)() = (double (*)())(intptr_t)cantFail(mf.getAddress());
            // keep global variables in the jit
            result = fp();
            if (node_type != NodeType::VAR_NODE) {
                jit->mjit->removeModule(mk);
            }
            create_module_and_pass_manager(jit->cg,
                make_unique_name("mjit").c_str());
        }
    }
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
            create_module_and_pass_manager(jit->cg, make_unique_name("mjit").c_str());
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
    create_builtins(parser, cg->context);
    create_module_and_pass_manager(cg, make_unique_name("mjit").c_str());
    generate_runtime_module(cg, parser);
    _add_module_to_jit(jit);
    create_module_and_pass_manager(cg, make_unique_name("mjit").c_str());
    return jit;
}

int run_repl()
{
    menv* env = env_new();
    parser* parser = parser_new(NULL, true, NULL);
    JIT* jit = build_jit(env, parser);
    fprintf(stderr, "m> ");
    parse_block(parser, nullptr, &eval_statement, jit);
    fprintf(stderr, "bye !\n");
    jit_free(jit);
    env_free(env);
    return 0;
}