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
        printf("%d", result.i_value);
    } else if (result.type == TYPE_BOOL) {
        printf("%s", boolean_values[result.b_value]);
    } else if (result.type == TYPE_DOUBLE) {
        printf("%f", result.d_value);
    } else if (result.type == TYPE_CHAR) {
        printf("%c", result.c_value);
    } else if (result.type == TYPE_STRING) {
        printf("%s", result.s_value);
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
    if (node) {
        void* p_fun = generate_code(jit->cg, node);
        if (p_fun) {
            //LLVMDumpModule(jit->cg->module);
            _add_current_module_to_jit(jit);
            void* fp = find_target_address(jit, string_get(&fn));
            // keep global variables in the jit
            enum type ret_type = get_type(type);
            if (is_int_type(ret_type)) {
                int (*i_fp)() = (int (*)())fp;
                result.i_value = i_fp();
                result.type = ret_type;
            } else if (ret_type == TYPE_DOUBLE||ret_type==TYPE_EXT) {
                double (*d_fp)() = (double (*)())fp;
                result.d_value = d_fp();
                result.type = TYPE_DOUBLE;
            } else if (ret_type == TYPE_STRING) {
                char* (*s_fp)() = (char* (*)())fp;
                result.s_value = s_fp();
                result.type = TYPE_STRING;
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
        //printf("node->type: %s\n", node_type_strings[node->node_type]);
        struct JIT* jit = (struct JIT*)p_jit;
        analyze(jit->env->type_env, node);
        string type_node_str = to_string(node->type);
        if (!node->type)
            goto exit;
        if (node->node_type == PROTOTYPE_NODE) {
            generate_code(jit->cg, node);
        } else if (node->node_type == FUNCTION_NODE||node->node_type == TYPE_NODE) {
            // function definition
            generate_code(jit->cg, node);
            //LLVMDumpModule(jit->cg->module);
            _add_current_module_to_jit(jit);
            _create_jit_module(jit->cg);
        } else {
            /*
             * evaluate an expression
             */
            struct eval_result result = eval_exp(jit, node);
            if (node->node_type != VAR_NODE)
                _print(result);
        }
        //printf(":%s\n", string_get(&type_node_str));
        string_deinit(&type_node_str);
    }
exit:
    fprintf(stderr, "m> ");
}

struct JIT* build_jit(struct menv* env)
{
    struct code_generator* cg = env->type_env->cg;
    struct JIT* jit = jit_new(cg);
    jit->env = env;
    _create_jit_module(cg);
    generate_runtime_module(cg);
    _add_current_module_to_jit(jit);
    _create_jit_module(cg);
    return jit;
}

int run_repl()
{
    struct menv* env = env_new(true);
    struct JIT* jit = build_jit(env);
    printf("m> ");
    parse_repl(env->parser, &eval_statement, jit);
    printf("bye !\n");
    jit_free(jit);
    env_free(env);
    return 0;
}
