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
#include "app/error.h"
#include "codegen/llvm/cg_llvm.h"

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
    } else if (result.type == TYPE_F64) {
        printf("%f", result.d_value);
    } else if (result.type == TYPE_CHAR) {
        printf("%c", result.c_value);
    } else if (result.type == TYPE_STRING) {
        printf("%s", result.s_value);
    }
}

void* _add_current_module_to_jit(struct JIT *jit)
{
    struct cg_llvm *cg = jit->engine->be->cg;
    assert(cg->module);
    //the function takes the ownership of the module.
    void *result = jit_add_module(jit, cg->module);
    cg->module = 0;
    return result;
}

void _create_new_module(struct cg_llvm *cg)
{
    string mod_name = make_unique_name("mjit");
    create_ir_module(cg, string_get(&mod_name));
    string_deinit(&mod_name);
}

struct eval_result eval_exp(struct JIT *jit, struct ast_node *node)
{
    struct cg_llvm *cg = jit->engine->be->cg;
    struct type_context *tc = cg->base.sema_context->tc;
    string fn = make_unique_name("main-fn");
    symbol fn_symbol = string_2_symbol(&fn);
    enum node_type node_type = node->node_type;
    if (!node->type){
        //analyze(jit->cg->base.sema_context, node);
        emit_code(jit->engine->be->cg, node);
    }
    struct type_item *type = node->type;
    struct eval_result result = { 0 };
    node = wrap_expr_as_function(jit->engine->fe->parser->tc, node, fn_symbol);
    analyze(cg->base.sema_context, node);
    _create_new_module(cg);
    emit_code(cg, node);
    if (node) {
        void *p_fun = emit_ir_code(cg, node);
        if (p_fun) {
            //LLVMDumpModule(jit->env->module);
            void *resource_tracker = jit_add_module(jit, cg->module);
            cg->module = 0;
            struct fun_pointer fp = jit_find_symbol(jit, string_get(&fn));
            // keep global variables in the jit
            enum type ret_type = get_type(tc, type);
            if (is_int_type(ret_type)) {
                result.i_value = fp.fp.i_fp();
                result.type = ret_type;
            } else if (ret_type == TYPE_F64 || ret_type == TYPE_STRUCT) {
                result.d_value = fp.fp.d_fp();
                result.type = TYPE_F64;
            } else if (ret_type == TYPE_STRING) {
                result.s_value = fp.fp.s_fp();
                result.type = TYPE_STRING;
            }
            if (node_type != VAR_NODE) {
                //jit->mjit->removeModule(mk);
            }
            jit_remove_module(resource_tracker);
        }
    }
    string_deinit(&fn);
    return result;
}

struct eval_result eval_module(struct JIT *jit, struct ast_node *node)
{
    struct cg_llvm *cg = jit->engine->be->cg;
    struct type_context *tc = cg->base.sema_context->tc;
    struct eval_result result = { 0 };
    analyze(cg->base.sema_context, node);
    struct error_report *er = get_last_error_report(jit->engine->fe->sema_context);
    if(er){
        return result;
    }
    _create_new_module(cg);
    enum node_type node_type = node->node_type;
    if (!node->type){
        //analyze(jit->cg->base.sema_context, node);
        emit_code(jit->engine->be->cg, node);
    }
    struct type_item *type = node->type;
    emit_code(cg, node);
    if (node) {
        void *p_fun = emit_ir_code(cg, node);
        if (p_fun) {
            //LLVMDumpModule(jit->env->module);
            void *resource_tracker = jit_add_module(jit, cg->module);
            cg->module = 0;
            struct fun_pointer fp = jit_find_symbol(jit, "_start");
            // keep global variables in the jit
            enum type ret_type = get_return_type(tc, type);
            if (is_int_type(ret_type)) {
                result.i_value = fp.fp.i_fp();
                result.type = ret_type;
            } else if (ret_type == TYPE_F64 || ret_type == TYPE_STRUCT) {
                result.d_value = fp.fp.d_fp();
                result.type = TYPE_F64;
            } else if (ret_type == TYPE_STRING) {
                result.s_value = fp.fp.s_fp();
                result.type = TYPE_STRING;
            }
            if (node_type != VAR_NODE) {
                //jit->mjit->removeModule(mk);
            }
            jit_remove_module(resource_tracker);
        }
    }
    return result;
}

void eval_node(void *p_jit, struct ast_node *node)
{
    if(!node) return;
    struct JIT *jit = (struct JIT *)p_jit;
    struct cg_llvm *cg = jit->engine->be->cg;
    analyze(cg->base.sema_context, node);
    eval_statement(p_jit, node);
}

void eval_statement(void *p_jit, struct ast_node *node)
{
    if (!node)
        return;
    //printf("node->type: %s\n", node_type_strings[node->node_type]);
    struct JIT *jit = (struct JIT *)p_jit;
    struct cg_llvm *cg = jit->engine->be->cg;
    struct type_context *tc = cg->base.sema_context->tc;
    _create_new_module(cg);
    emit_code(cg, node);
    string type_node_str = to_string(tc, node->type);
    if (!node->type)
        goto exit;
    if (node->node_type == FUNC_TYPE_NODE) {
        emit_ir_code(cg, node);
    } else if (node->node_type == FUNC_NODE || node->node_type == STRUCT_NODE) {
        // function definition
        emit_ir_code(cg, node);
        //LLVMDumpModule(jit->env->module);
        _add_current_module_to_jit(jit);
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
exit:
    fprintf(stderr, "m> ");
}

int run_repl(void)
{
    struct engine *engine = engine_llvm_new(0, true);
    struct JIT *jit = jit_new(engine);
    printf("m> ");
    parse_repl_code(engine->fe->parser, &eval_node, jit);
    printf("bye !\n");
    jit_free(jit);
    engine_free(engine);
    return 0;
}
