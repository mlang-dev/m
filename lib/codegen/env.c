#include "codegen/env.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"
#include "sema/sema_context.h"
#include <assert.h>

struct env *g_env = 0;

struct env *env_new(bool is_repl)
{
    struct env *env = malloc(sizeof(*env));
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
}

struct env *get_env()
{
    assert(g_env);
    return g_env;
}