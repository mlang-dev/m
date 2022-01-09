/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m repl, interactive JIT running environment
 */
#include <assert.h>

#include "compiler/jit.h"
#include "compiler/repl.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"

const char *boolean_values[2] = {
    "false",
    "true",
};

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

void _add_current_module_to_jit(struct JIT *jit)
{
    add_module(jit, jit->env->cg->module);
    jit->env->cg->module = 0;
}

void _create_jit_module(struct env *env)
{
    string mod_name = make_unique_name("mjit");
    create_ir_module(env->cg, string_get(&mod_name));
    string_deinit(&mod_name);
}

struct eval_result eval_exp(struct JIT *jit, struct ast_node *node)
{
    string fn = make_unique_name("main-fn");
    symbol fn_symbol = string_2_symbol(&fn);
    enum node_type node_type = node->node_type;
    if (!node->type)
        emit_code(jit->env, node);
    struct type_exp *type = node->type;
    struct eval_result result = { 0 };
    node = parse_exp_to_function(jit->env->cg->sema_context->parser, node, fn_symbol);
    emit_code(jit->env, node);
    if (node) {
        void *p_fun = emit_ir_code(jit->env->cg, node);
        if (p_fun) {
            //LLVMDumpModule(jit->env->module);
            _add_current_module_to_jit(jit);
            struct fun_pointer fp = find_target_address(jit, string_get(&fn));
            // keep global variables in the jit
            enum type ret_type = get_type(type);
            if (is_int_type(ret_type)) {
                result.i_value = fp.fp.i_fp();
                result.type = ret_type;
            } else if (ret_type == TYPE_DOUBLE || ret_type == TYPE_EXT) {
                result.d_value = fp.fp.d_fp();
                result.type = TYPE_DOUBLE;
            } else if (ret_type == TYPE_STRING) {
                result.s_value = fp.fp.s_fp();
                result.type = TYPE_STRING;
            }
            if (node_type != VAR_NODE) {
                //jit->mjit->removeModule(mk);
            }
            _create_jit_module(jit->env);
        }
    }
    string_deinit(&fn);
    return result;
}

void eval_statement(void *p_jit, struct ast_node *node)
{
    if (node) {
        //printf("node->type: %s\n", node_type_strings[node->node_type]);
        struct JIT *jit = (struct JIT *)p_jit;
        emit_code(jit->env, node);
        string type_node_str = to_string(node->type);
        if (!node->type)
            goto exit;
        if (node->node_type == FUNC_TYPE_NODE) {
            emit_ir_code(jit->env->cg, node);
        } else if (node->node_type == FUNC_NODE || node->node_type == TYPE_NODE) {
            // function definition
            emit_ir_code(jit->env->cg, node);
            //LLVMDumpModule(jit->env->module);
            _add_current_module_to_jit(jit);
            _create_jit_module(jit->env);
        } else {
            /*
             * evaluate an expression
             */
            struct eval_result result = eval_exp(jit, node);
            if (node->node_type != VAR_NODE)
                _print(result);
        }
        printf(":%s\n", string_get(&type_node_str));
        string_deinit(&type_node_str);
    }
exit:
    fprintf(stderr, "m> ");
}

struct JIT *build_jit(struct env *env)
{
    struct JIT *jit = jit_new(env);
    _create_jit_module(env);
    _add_current_module_to_jit(jit);
    _create_jit_module(env);
    return jit;
}

int run_repl()
{
    struct env *env = env_new(true);
    struct JIT *jit = build_jit(env);
    printf("m> ");
    parse_repl(env->cg->sema_context->parser, &eval_statement, jit);
    printf("bye !\n");
    jit_free(jit);
    env_free(env);
    return 0;
}
