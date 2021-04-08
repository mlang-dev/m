#include "codegen/codegen.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"
#include "sema/sema_context.h"
#include <assert.h>

struct env *g_env = 0;

struct env *env_new(bool is_repl)
{
    LLVMContextRef context = LLVMContextCreate();
    LLVMInitializeCore(LLVMGetGlobalPassRegistry());
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    struct env *env = malloc(sizeof(*env));
    env->context = context;
    env->builder = LLVMCreateBuilderInContext(context);
    hashset_init(&env->builtins);
    env->module = 0;
    _set_bin_ops(env);
    hashtable_init(&env->gvs);
    hashtable_init(&env->protos);
    hashtable_init(&env->named_values);
    hashtable_init(&env->specialized_nodes);
    hashtable_init(&env->ext_types);
    hashtable_init(&env->ext_nodes);
    hashtable_init(&env->ext_vars);
    hashtable_init_with_value_size(&env->type_size_infos, sizeof(struct type_size_info), 0);
    hashtable_init_with_value_size(&env->fun_infos, sizeof(struct fun_info), (free_fun)fun_info_deinit);
    env->target_info = ti_new();
    env->sema_context = sema_context_new(is_repl);
    g_env = env;
    return env;
}

void env_free(struct env *env)
{
    LLVMDisposeBuilder(env->builder);
    //delete (llvm::legacy::FunctionPassManager*)env->fpm;
    if (env->module)
        LLVMDisposeModule(env->module);
    LLVMContextDispose(env->context);
    sema_context_free(env->sema_context);
    ti_free(env->target_info);
    hashtable_deinit(&env->fun_infos);
    hashtable_deinit(&env->type_size_infos);
    hashtable_deinit(&env->specialized_nodes);
    hashtable_deinit(&env->gvs);
    hashtable_deinit(&env->protos);
    hashtable_deinit(&env->named_values);
    hashset_deinit(&env->builtins);
    hashtable_deinit(&env->ext_types);
    hashtable_deinit(&env->ext_nodes);
    hashtable_deinit(&env->ext_vars);
    free(env);
    g_env = 0;
    //LLVMShutdown();
}

struct env *get_env()
{
    assert(g_env);
    return g_env;
}