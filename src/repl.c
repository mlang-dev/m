/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m repl, interactive JIT running environment
 */
#include <assert.h>

#include "env.h"
#include "jit.h"
#include "repl.h"

void _print(struct eval_result result)
{
    if (result.type == TYPE_INT) {
        printf("%d\n", result.i_value);
    } else if (result.type == TYPE_DOUBLE) {
        printf("%f\n", result.d_value);
    }
}

void _add_current_module_to_jit(struct JIT* jit)
{
    add_module(jit, jit->cg->module);
    jit->cg->module = 0;
}

void _create_jit_module(struct code_generator* cg)
{
    string mod_name = make_unique_name("mjit");
    create_module_and_pass_manager(cg, string_get(&mod_name));
    string_deinit(&mod_name);
}

struct eval_result eval_exp(struct JIT* jit, struct exp_node* node)
{
    string fn = make_unique_name("main-fn");
    enum node_type node_type = node->node_type;
    if (!node->type)
        analyze(jit->env->type_env, node);
    struct type_exp* type = node->type;
    struct eval_result result = { 0 };
    node = parse_exp_to_function(jit->cg->parser, node, string_get(&fn));
    analyze(jit->env->type_env, node);
    //string node_type_str = to_string(node->type);
    //printf("%s\n", string_get(&node_type_str));
    if (node) {
        void* p_fun = generate_code(jit->cg, node);
        if (p_fun) {
            _add_current_module_to_jit(jit);
            void* fp = find_target_address(jit, string_get(&fn));
            //LLVMDumpModule(module);
            // keep global variables in the jit
            if (type && is_int_type(type->type)) {
                int (*i_fp)() = (int (*)())fp;
                result.i_value = i_fp();
                result.type = TYPE_INT;
                printf("excuting int result: %d, %d\n", result.i_value, type->type);
            } else {
                double (*d_fp)() = (double (*)())fp;
                result.d_value = d_fp();
                result.type = TYPE_DOUBLE;
            }
            if (node_type != VAR_NODE) {
                //jit->mjit->removeModule(mk);
            }
            _create_jit_module(jit->cg);
        }
    }
    string_deinit(&fn);
    return result;
}

void eval_statement(void* p_jit, struct exp_node* node)
{
    if (node) {
        struct JIT* jit = (struct JIT*)p_jit;
        analyze(jit->env->type_env, node);
        string type_node_str = to_string(node->type);
        printf("%s\n", string_get(&type_node_str));
        string_deinit(&type_node_str);
        if (!node->type)
            goto exit;
        if (node->node_type == PROTOTYPE_NODE) {
            generate_code(jit->cg, node);
        } else if (node->node_type == FUNCTION_NODE) {
            // function definition
            generate_code(jit->cg, node);
            _add_current_module_to_jit(jit);
            _create_jit_module(jit->cg);
        } else {
            /*
             * evaluate an expression
             */
            //printf("eval exp\n");
            struct eval_result result = eval_exp(jit, node);
            if (node->node_type != VAR_NODE)
                _print(result);
        }
    }
exit:
    fprintf(stderr, "m> ");
}

struct JIT* build_jit(struct menv* env, struct parser* parser)
{
    struct code_generator* cg = cg_new(env, parser);
    struct JIT* jit = jit_new(cg);
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
    struct menv* env = env_new();
    struct parser* parser = parser_new(0, true, 0);
    struct JIT* jit = build_jit(env, parser);
    printf("m> ");
    parse_block(parser, 0, &eval_statement, jit);
    printf("bye !\n");
    jit_free(jit);
    env_free(env);
    return 0;
}
