/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m repl, interactive JIT running environment
 */
#include <assert.h>

#include "jit.h"
#include "env.h"


void _add_current_module_to_jit(JIT* jit)
{
    add_module(jit, jit->cg->module);
    jit->cg->module = NULL;
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
    NodeType node_type = node->node_type;
    double result = 0.0;
    node = parse_exp_to_function(jit->cg->parser, node, string_get(&fn));
    if (node) {
        void* p_fun = generate_code(jit->cg, node);
        if (p_fun) {
            if(node->type){
                printf("hello0, %p, %s\n", (void*)node->type, NodeTypeString[node->node_type]);
                string node_type_str = to_string(node->type);
                printf("hello1\n");
                printf("inferred type: %s\n", string_get(&node_type_str));
            }
            _add_current_module_to_jit(jit);

            target_address_double fp = find_target_address_double(jit, string_get(&fn));
            //LLVMDumpModule(module);
            // keep global variables in the jit
            result = fp();
            if (node_type != VAR_NODE) {
                //jit->mjit->removeModule(mk);
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
        JIT* jit = (JIT*)p_jit;
        analyze(jit->env->type_sys, node);
        string type_node_str = to_string(node->type);
        printf("%s\n", string_get(&type_node_str));
        if (node->node_type == PROTOTYPE_NODE){
            generate_code(jit->cg, node);
        }
        else if (node->node_type == FUNCTION_NODE) {
            // function definition
            generate_code(jit->cg, node);
            _add_current_module_to_jit(jit);
            _create_jit_module(jit->cg);
        } else {
            /*
             * evaluate an expression
             */
            double result = eval_exp(jit, node);
            if (node->node_type != VAR_NODE)
                printf("%f\n", result);
        }
    }
    fprintf(stderr, "m> ");
}

JIT* build_jit(menv* env, parser* parser)
{
    code_generator* cg = cg_new(env, parser);
    JIT* jit = jit_new(cg);
    jit->env = env;
    //log_info(DEBUG, "creating builtins");
    create_builtins(parser, cg->context);
    //log_info(DEBUG, "creating jit modules");
    _create_jit_module(cg);
    //log_info(DEBUG, "generating runtime modules");
    generate_runtime_module(cg, &parser->ast->builtins);
    //log_info(DEBUG, "adding to jit");
    _add_current_module_to_jit(jit);
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
    parse_block(parser, NULL, &eval_statement, jit);
    printf("bye !\n");
    jit_free(jit);
    env_free(env);
    return 0;
}
