#include "codegen/env.h"
#include "clib/util.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"
#include "codegen/wat_codegen.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "lexer/init.h"
#include <assert.h>

struct env *g_env = 0;

struct env *env_new(bool is_repl)
{
    frontend_init();
    struct env *env;
    MALLOC(env, sizeof(struct env));
    env->parser = m_parser_new(is_repl);
    env->new_parser = parser_new();
    char libpath[4096];
    char *mpath = get_exec_path();
    join_path(libpath, sizeof(libpath), mpath, "mlib/stdio.m");
    struct ast_node *stdio = parse_file(env->parser, libpath);
    join_path(libpath, sizeof(libpath), mpath, "mlib/math.m");
    struct ast_node *math = parse_file(env->parser, libpath);
    env->cg = cg_new(sema_context_new(&env->parser->symbol_2_int_types, stdio, math, is_repl));
    wat_codegen_init();
    g_env = env;
    return env;
}

void env_free(struct env *env)
{
    m_parser_free(env->parser);
    lalr_parser_free(env->new_parser);
    sema_context_free(env->cg->sema_context);
    cg_free(env->cg);
    FREE(env);
    g_env = 0;
    frontend_deinit();
}

struct env *get_env()
{
    assert(g_env);
    return g_env;
}

void emit_sp_code(struct code_generator *cg)
{
    for(size_t i = 0; i < array_size(&cg->sema_context->new_specialized_asts); i++){
        struct ast_node *new_sp = *(struct ast_node **)array_get(&cg->sema_context->new_specialized_asts, i);
        emit_ir_code(cg, new_sp);
    }
    array_reset(&cg->sema_context->new_specialized_asts);
}

struct type_exp *emit_code(struct env *env, struct ast_node *node)
{
    struct type_exp *type = analyze(env->cg->sema_context, node);
    emit_sp_code(env->cg);
    if (array_size(&env->cg->sema_context->used_builtin_names)) {
        for (size_t i = 0; i < array_size(&env->cg->sema_context->used_builtin_names); i++) {
            symbol built_name = *((symbol *)array_get(&env->cg->sema_context->used_builtin_names, i));
            struct ast_node *n = hashtable_get_p(&env->cg->sema_context->builtin_ast, built_name);
            if (!hashset_in_p(&env->cg->builtins, built_name)) {
                hashset_set_p(&env->cg->builtins, built_name);
                emit_ir_code(env->cg, n);
            }
        }
        array_clear(&env->cg->sema_context->used_builtin_names);
    }
    return type;
}
