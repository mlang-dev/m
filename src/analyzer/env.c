/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m environment, a top class to store all data for compiling session
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "env.h"
#include "sys.h"
#include "clib/hash.h"
#include "cmodule.h"
#include "clib/hashset.h"
#include "clib/hashtable.h"
#include "analyzer.h"


struct env* env_new(bool is_repl)
{
    struct env* env = malloc(sizeof(*env));
    memset((void*)env, 0, sizeof(*env));
    env->parser = parser_new(is_repl);
    array_init(&env->nongens, sizeof(struct type_exp*));
    array_init(&env->used_builtin_names, sizeof(symbol));
    env->cg = cg_new(env->parser);
    symbols_init();
    hashtable_init(&env->tenv);
    hashtable_init(&env->venv);
    hashtable_init(&env->ext_type_ast);
    hashtable_init(&env->builtin_ast);
    hashtable_init(&env->generic_ast);
    struct array args;
    array_init(&args, sizeof(struct type_exp*));
    /*nullary type: builtin default types*/
    for (size_t i = 0; i < ARRAY_SIZE(type_strings); i++) {
        struct type_exp* exp = (struct type_exp*)create_type_oper(i, &args);
        symbol type_symbol = to_symbol(type_strings[i]);
        set_symbol_type(&env->tenv, type_symbol, exp);
    }
    char libpath[PATH_MAX];
    char* mpath = get_exec_path();
    join_path(libpath, mpath, "/lib/stdio.m");
    struct block_node* block = parse_file(env->parser, libpath);
    struct array builtins;
    array_init(&builtins, sizeof(struct exp_node*));
    for(size_t i = 0; i < array_size(&block->nodes); i++){
        struct exp_node* node = *(struct exp_node**)array_get(&block->nodes, i);
        array_push(&builtins, &node);
    }
    join_path(libpath, mpath, "/lib/math.m");
    block  = parse_file(env->parser, libpath);
    array_add(&builtins, &block->nodes);
    for (size_t i = 0; i < array_size(&builtins); i++) {
        struct exp_node* node = *(struct exp_node**)array_get(&builtins, i);
        assert(node->node_type == PROTOTYPE_NODE);
        struct prototype_node* proto = (struct prototype_node*)node;
        analyze(env, node);
        set_type(&env->venv, string_get(proto->name), proto->base.type);
        hashtable_set_p(&env->builtin_ast, proto->name, node);
        //string type = to_string(proto->base.type);
    }
    return env;
}

void env_free(struct env* env)
{
    hashtable_deinit(&env->ext_type_ast);
    hashtable_deinit(&env->generic_ast);
    hashtable_deinit(&env->builtin_ast);
    hashtable_deinit(&env->venv);
    hashtable_deinit(&env->tenv);
    symbols_deinit();
    array_deinit(&env->used_builtin_names);
    array_deinit(&env->nongens);
    cg_free(env->cg);
    parser_free(env->parser);
    free(env);
}
