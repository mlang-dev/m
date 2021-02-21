/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Type Inference Analyzer
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "sys.h"
#include "analyzer.h"
#include "clib/hash.h"
#include "cmodule.h"
#include "clib/hashset.h"
#include "clib/hashtable.h"


struct type_exp* _analyze(struct type_env* env, struct exp_node* node);
const char* relational_ops[] = {
    "<",
    ">",
    "==",
    "!=",
    "<=",
    ">="
};

bool _is_predicate_op(const char* op)
{
    for (size_t i = 0; i < ARRAY_SIZE(relational_ops); i++) {
        if (strcmp(relational_ops[i], op) == 0)
            return true;
    }
    return false;
}

void _log_err(struct type_env* env, struct source_loc loc,  const char *msg)
{
    (void)env;
    char full_msg[512];
    sprintf(full_msg, "%s:%d:%d: %s", "", loc.line, loc.col, msg);
    log_info(ERROR, full_msg);
}

struct type_exp* _analyze_unk(struct type_env* env, struct exp_node* node)
{
    printf("analyzing unk: %s\n", node_type_strings[node->node_type]);
    if (!env || !node)
        return 0;
    return 0;
}

struct type_exp* retrieve(struct type_env* env, const char* name)
{
    struct type_exp* type = retrieve_type(name, &env->nongens, &env->type_env);
    return type? type : retrieve_type(name, &env->nongens, &env->builtin_types);
}

void set(struct type_env* env, const char* name, struct type_exp* type)
{
    set_type(&env->type_env, name, type);
}

bool has(struct type_env* env, const char* name)
{
    return has_type(&env->type_env, name);
}

void set_builtin(struct type_env* env, const char* name, struct type_exp* type)
{
    set_type(&env->builtin_types, name, type);
}

bool is_builtin(struct type_env* env, const char* name)
{
    return hashtable_in(&env->builtin_types, name);
}
struct type_exp* _analyze_ident(struct type_env* env, struct exp_node* node)
{
    struct ident_node* ident = (struct ident_node*)node;
    struct type_exp* type = 0;
    for(size_t i = 0; i < array_size(&ident->member_accessors); i++){
        string* id = (string*)array_get(&ident->member_accessors, i);
        if(i==0){
            type = retrieve(env, string_get(id));
        }else{
            assert(type);
            struct type_oper* oper = (struct type_oper*)type;
            struct type_node* type_node = (struct type_node*)hashtable_get(&env->type_nodes, string_get(&oper->base.name));
            int index = find_member_index(type_node, string_get(id));
            if (index < 0){
                _log_err(env, node->loc, "%s member not matched.");
                return 0;
            }
            type = *(struct type_exp**)array_get(&oper->args, index);
        }
    }
    return type;
}

struct type_exp* _analyze_num(struct type_env* env, struct exp_node* node)
{
    return retrieve(env, type_strings[node->annotated_type->type]);
}

struct type_exp* _analyze_var(struct type_env* env, struct exp_node* node)
{
    struct var_node* var = (struct var_node*)node;
    struct type_exp* type;
    assert(var->base.annotated_type || var->init_value);
    if(var->base.annotated_type&&var->base.annotated_type->type == TYPE_EXT){
        assert(var->base.annotation);
        type = retrieve(env, string_get(var->base.annotation));
        set(env, string_get(&var->var_name), type);
        analyze(env, var->init_value);
        return type;
    }
    else if(var->base.annotated_type && !var->init_value){
        type = var->base.annotated_type;
        set(env, string_get(&var->var_name), type);
        return type;
    }
    type = _analyze(env, var->init_value);
    if(!type) return 0;
    if(var->base.annotated_type && var->init_value->annotated_type 
        && var->base.annotated_type->type != var->init_value->annotated_type->type){
        _log_err(env, node->loc, "variable type not matched with literal constant");
        return 0;
    }
    struct type_exp* result_type = (struct type_exp*)create_type_var();
    unify(result_type, type, &env->nongens);
    set(env, string_get(&var->var_name), result_type);
    return result_type;
}

struct type_exp* _analyze_type(struct type_env* env, struct exp_node* node)
{
    struct type_node* type = (struct type_node*)node;
    struct array args; 
    array_init(&args, sizeof(struct type_exp*));
    for(size_t i = 0; i < array_size(&type->body->nodes); i++){
        //printf("creating type: %zu\n", i);
        struct type_exp* arg = _analyze_var(env, *(struct exp_node**)array_get(&type->body->nodes, i));
        array_push(&args, &arg);
    }
    struct type_exp* result_type = (struct type_exp*)create_type_oper_ext(type->name, &args);
    assert(string_eq(&type->name, &result_type->name));
    set(env, string_get(&type->name), result_type);
    hashtable_set(&env->type_nodes, string_get(&type->name), node);
    return result_type;
}

struct type_exp* _analyze_type_value(struct type_env* env, struct exp_node* node)
{
    struct type_value_node* type_value = (struct type_value_node*)node;
    for(size_t i = 0; i < array_size(&type_value->body->nodes); i++){
        //printf("creating type: %zu\n", i);
        analyze(env, *(struct exp_node**)array_get(&type_value->body->nodes, i));
    }
    return 0;
}

struct type_exp* _analyze_proto(struct type_env* env, struct exp_node* node)
{
    (void)env;
    struct prototype_node* proto = (struct prototype_node*)node;
    struct array fun_sig; 
    array_init(&fun_sig, sizeof(struct type_exp*));
    for (size_t i = 0; i < array_size(&proto->fun_params); i++) {
        struct var_node* param = (struct var_node*)array_get(&proto->fun_params, i);
        assert(param->base.annotated_type);
        param->base.type = param->base.annotated_type;
        array_push(&fun_sig, &param->base.annotated_type);
    }   
    assert(proto->base.annotated_type);
    array_push(&fun_sig, &proto->base.annotated_type);
    //printf("ret type analyzing proto: %p, %p\n", (void*)proto->base.annotated_type, *(void**)array_back(&fun_sig));
    proto->base.type = (struct type_exp*)create_type_fun(&fun_sig);
    return proto->base.type;
}

struct type_exp* _analyze_fun(struct type_env* env, struct exp_node* node)
{
    struct function_node* fun = (struct function_node*)node;
    //# create a new non-generic variable for the binder
    struct array fun_sig; 
    array_init(&fun_sig, sizeof(struct type_exp*));
    for (size_t i = 0; i < array_size(&fun->prototype->fun_params); i++) {
        struct var_node* param = (struct var_node*)array_get(&fun->prototype->fun_params, i);
        struct type_exp* exp;
        if (param->base.annotated_type)
            exp = param->base.annotated_type;
        else
            exp = (struct type_exp*)create_type_var();
        array_push(&fun_sig, &exp);
        array_push(&env->nongens, &exp);
        set(env, string_get(&param->var_name), exp);
    }
    struct type_exp* fun_type = (struct type_exp*)create_type_var();
    set(env, string_get(&fun->prototype->name), fun_type);
    struct type_exp* ret_type = _analyze(env, (struct exp_node*)fun->body);
    array_push(&fun_sig, &ret_type);
    struct type_exp* result_type = (struct type_exp*)create_type_fun(&fun_sig);
    unify(fun_type, result_type, &env->nongens);
    struct type_exp* result = prune(fun_type);
    fun->prototype->base.type = result;
    if(is_generic(result)){
        hashtable_set(&env->generic_nodes, string_get(&fun->prototype->name), node);
    }
    return result;
}

struct type_exp* _analyze_call(struct type_env* env, struct exp_node* node)
{
    struct call_node* call = (struct call_node*)node;
    struct type_exp* fun_type = retrieve(env, string_get(&call->callee));
    if (!fun_type){
        struct source_loc loc = {1, 1};
        string error;
        string_copy(&error, &call->callee);
        string_add_chars(&error, " not defined");
        _log_err(env, loc, string_get(&error));
        string_deinit(&error);
        return 0;
    }
    struct array args;
    array_init(&args, sizeof(struct type_exp*));
    for (size_t i = 0; i < array_size(&call->args); i++) {
        struct exp_node* arg = *(struct exp_node**)array_get(&call->args, i);
        struct type_exp* type = _analyze(env, arg);
        array_push(&args, &type);
    }
    /*monomorphization of generic*/
    struct exp_node* specialized_node = 0;
    if(is_generic(fun_type)&&(!is_any_generic(&args)&&array_size(&args))&&!is_recursive(call)){
        call->specialized_callee = monomorphize(string_get(&call->callee), &args);
        if(has_type(&env->type_env, string_get(&call->specialized_callee))){
            fun_type = retrieve(env, string_get(&call->specialized_callee));
            struct type_oper* fun_op = (struct type_oper*)fun_type;
            return *(struct type_exp**)array_back(&fun_op->args);
        }
        /*specialized callee*/
        struct exp_node* generic_fun = (struct exp_node*)hashtable_get(&env->generic_nodes, string_get(&call->callee));
        struct function_node* sp_fun = (struct function_node*)node_copy(generic_fun);
        sp_fun->prototype->name = call->specialized_callee;
        fun_type = analyze(env, (struct exp_node*)sp_fun);
        hashtable_set(&env->cg->specialized_nodes, string_get(&sp_fun->prototype->name), sp_fun);
        set(env, string_get(&call->specialized_callee), fun_type);
        specialized_node = (struct exp_node*)sp_fun;
    }
    struct type_exp* result_type = (struct type_exp*)create_type_var();
    array_push(&args, &result_type);
    struct type_exp* call_fun = (struct type_exp*)create_type_fun(&args);
    unify(call_fun, fun_type, &env->nongens);
    if(is_builtin(env, string_get(&call->callee))){
        array_push(&env->ref_builtin_names, &call->callee);
    }
    if(specialized_node){
        generate_code(env->cg, specialized_node);
    }
    return prune(result_type);
}

struct type_exp* _analyze_unary(struct type_env* env, struct exp_node* node)
{
    struct unary_node* unary = (struct unary_node*)node;
    struct type_exp* op_type = _analyze(env, unary->operand);
    if (string_eq_chars(&unary->op, "!")) {
        struct type_exp* bool_type = (struct type_exp*)create_nullary_type(TYPE_BOOL);
        unify(op_type, bool_type, &env->nongens);
        unary->operand->type = op_type;
    }
    return op_type;
}

struct type_exp* _analyze_binary(struct type_env* env, struct exp_node* node)
{
    struct binary_node* bin = (struct binary_node*)node;
    struct type_exp* lhs_type = _analyze(env, bin->lhs);
    struct type_exp* rhs_type = _analyze(env, bin->rhs);
    struct type_exp* result = 0;
    if (unify(lhs_type, rhs_type, &env->nongens)) {
        if (_is_predicate_op(string_get(&bin->op)))
            result = (struct type_exp*)create_nullary_type(TYPE_BOOL);
        else
            result = lhs_type;
        return result;
    }else{
        string error;
        string_init_chars(&error, "type not same for binary op: ");
        string_add(&error, &bin->op);
        _log_err(env, bin->base.loc, string_get(&error));
    }
    return result;
}

struct type_exp* _analyze_if(struct type_env* env, struct exp_node* node)
{
    struct condition_node* cond_node = (struct condition_node*)node;
    struct type_exp* cond_type = _analyze(env, cond_node->condition_node);
    struct type_oper* bool_type = create_nullary_type(TYPE_BOOL);
    unify(cond_type, (struct type_exp*)bool_type, &env->nongens);
    struct type_exp* then_type = _analyze(env, cond_node->then_node);
    struct type_exp* else_type = _analyze(env, cond_node->else_node);
    unify(then_type, else_type, &env->nongens);
    return then_type;
}

struct type_exp* _analyze_for(struct type_env* env, struct exp_node* node)
{
    struct for_node* for_node = (struct for_node*)node;
    struct type_exp* int_type = (struct type_exp*)create_nullary_type(TYPE_INT);
    struct type_exp* bool_type = (struct type_exp*)create_nullary_type(TYPE_BOOL);
    set(env, string_get(&for_node->var_name), int_type);
    struct type_exp* start_type = _analyze(env, for_node->start);
    struct type_exp* step_type = _analyze(env, for_node->step);
    struct type_exp* end_type = _analyze(env, for_node->end);
    struct type_exp* body_type = _analyze(env, for_node->body);
    if(!unify(start_type, int_type, &env->nongens)){
        printf("failed to unify start type as int: %s, %s\n", kind_strings[start_type->kind], node_type_strings[for_node->start->node_type]);
    }
    unify(step_type, int_type, &env->nongens);
    unify(end_type, bool_type, &env->nongens);
    for_node->start->type = start_type;
    for_node->step->type = step_type;
    for_node->end->type = end_type;
    for_node->body->type = body_type;
    return (struct type_exp*)create_nullary_type(TYPE_UNIT);
}

struct type_env* type_env_new(struct parser* parser)
{
    struct type_env* env = malloc(sizeof(*env));
    memset((void*)env, 0, sizeof(struct type_env));
    array_init(&env->nongens, sizeof(struct type_exp*));
    array_init_free(&env->ref_builtin_names, sizeof(string), string_free_generic);
    env->cg = cg_new(parser);
    hashtable_init(&env->symbols);
    hashtable_init(&env->type_env);
    hashtable_init(&env->builtin_types);
    hashtable_init(&env->builtin_nodes);
    hashtable_init(&env->generic_nodes);
    hashtable_init(&env->type_nodes);
    struct array args;
    array_init(&args, sizeof(struct type_exp*));
    /*nullary type: builtin default types*/
    for (size_t i = 0; i < ARRAY_SIZE(type_strings); i++) {
        struct type_exp* exp = (struct type_exp*)create_type_oper(i, &args);
        set_builtin(env, type_strings[i], exp);
    }
    char libpath[PATH_MAX];
    char* mpath = get_exec_path();
    //printf("mpath is: %s\n", mpath);
    join_path(libpath, mpath, "/lib/stdio.m");
    struct block_node* block = parse_file(parser, libpath);
    struct array builtins;
    array_init(&builtins, sizeof(struct exp_node*));
    for(size_t i = 0; i < array_size(&block->nodes); i++){
        struct exp_node* node = *(struct exp_node**)array_get(&block->nodes, i);
        array_push(&builtins, &node);
    }
    join_path(libpath, mpath, "/lib/math.m");
    block  = parse_file(parser, libpath);
    array_add(&builtins, &block->nodes);
    for (size_t i = 0; i < array_size(&builtins); i++) {
        struct exp_node* node = *(struct exp_node**)array_get(&builtins, i);
        assert(node->node_type == PROTOTYPE_NODE);
        struct prototype_node* proto = (struct prototype_node*)node;
        _analyze(env, node);
        set_builtin(env, string_get(&proto->name), proto->base.type);
        hashtable_set(&env->builtin_nodes, string_get(&proto->name), node);
        //string type = to_string(proto->base.type);
        //printf("parsed builtins: %s, %s\n", string_get(&proto->name), string_get(&type));
    }
    return env;
}

void type_env_free(struct type_env* env)
{
    // for (int i = 0; i < env->type_env.buckets.cap; i++) {
    //     hashbox* box = (hashbox*)array_get(&env->type_env.buckets, i);
    //     if (box->status)
    //         type_exp_free(*(struct type_exp**)hashbox_get_value(box));
    // }
    hashtable_deinit(&env->type_nodes);
    hashtable_deinit(&env->generic_nodes);
    hashtable_deinit(&env->builtin_nodes);
    hashtable_deinit(&env->builtin_types);
    hashtable_deinit(&env->type_env);
    hashtable_deinit(&env->symbols);
    array_deinit(&env->ref_builtin_names);
    array_deinit(&env->nongens);
    cg_free(env->cg);
    free(env);
}

struct type_exp* _analyze_block(struct type_env* env, struct exp_node* node)
{
    struct block_node* block = (struct block_node*)node;
    struct type_exp* exp = 0;
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node* node = *(struct exp_node**)array_get(&block->nodes, i);
        exp = _analyze(env, node); 
    }
    return exp;
}

struct type_exp* (*analyze_fp[])(struct type_env*, struct exp_node*) = {
    _analyze_unk,
    _analyze_num,
    _analyze_ident,
    _analyze_var,
    _analyze_type,
    _analyze_type_value,
    _analyze_unary,
    _analyze_binary,
    _analyze_if,
    _analyze_for,
    _analyze_call,
    _analyze_proto,
    _analyze_fun,
    _analyze_block,
};

struct type_exp* _analyze(struct type_env* env, struct exp_node* node)
{
    struct type_exp* type = analyze_fp[node->node_type](env, node);
    node->type = type;
    return type;
}

struct type_exp* analyze(struct type_env* env, struct exp_node* node)
{
    struct type_exp* type = _analyze(env, node);
    struct code_generator* cg = env->cg;
    if(array_size(&env->ref_builtin_names)){
        for(size_t i = 0; i < array_size(&env->ref_builtin_names); i++){
            const char* built_name = string_get((string*)array_get(&env->ref_builtin_names, i));
            struct exp_node* node = hashtable_get(&env->builtin_nodes, built_name);
            if(!hashset_in(&cg->builtins, built_name)){
                hashset_set(&cg->builtins, built_name);
                generate_code(cg, node);
            }
        }
        array_clear(&env->ref_builtin_names);
    }
    return type;
}
