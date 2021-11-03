#include "codegen/env.h"
#include "clib/util.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"
#include "lexer/lexer.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include <assert.h>

struct env *g_env = 0;

struct env *env_new(bool is_repl)
{
    symbols_init();
    struct env *env;
    MALLOC(env, sizeof(struct env));
    env->parser = parser_new(is_repl);
    env->sema_context = sema_context_new(env->parser);
    env->cg = cg_new(env->sema_context);
    g_env = env;
    return env;
}

void env_free(struct env *env)
{
    parser_free(env->parser);
    sema_context_free(env->sema_context);
    cg_free(env->cg);
    free(env);
    g_env = 0;
    symbols_deinit();
}

struct env *get_env()
{
    assert(g_env);
    return g_env;
}

struct type_exp *emit_code(struct env *env, struct exp_node *node)
{
    struct type_exp *type = analyze(env->sema_context, node);
    if (array_size(&env->sema_context->used_builtin_names)) {
        for (size_t i = 0; i < array_size(&env->sema_context->used_builtin_names); i++) {
            symbol built_name = *((symbol *)array_get(&env->sema_context->used_builtin_names, i));
            struct exp_node *n = hashtable_get_p(&env->sema_context->builtin_ast, built_name);
            if (!hashset_in_p(&env->cg->builtins, built_name)) {
                hashset_set_p(&env->cg->builtins, built_name);
                emit_ir_code(env->cg, n);
            }
        }
        array_clear(&env->sema_context->used_builtin_names);
    }
    return type;
}
