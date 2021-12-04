/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * AST print functions
 */
#include "parser/astdump.h"
#include "clib/string.h"
#include "clib/util.h"

string _dump_block(struct block_node *node)
{
    string block;
    string_init_chars(&block, "blk: \n");
    for (size_t i = 0; i < array_size(&node->nodes); i++) {
        struct exp_node *stmt = *(struct exp_node **)array_get(&node->nodes, i);
        string str_stmt = dump(stmt);
        string_add_chars(&block, "  ");
        string_add(&block, &str_stmt);
        string_add_chars(&block, "\n");
    }
    return block;
}

string _dump_func_type(struct func_type_node *func_type)
{
    string result;
    string_init(&result);
    if (func_type->is_extern)
        string_add_chars(&result, "extern ");
    string_add_chars(&result, string_get(func_type->name));
    ARRAY_STRING(args);
    string var_str;
    string_init(&var_str);
    for (size_t i = 0; i < array_size(&func_type->fun_params); i++) {
        struct ast_node *var = *(struct ast_node **)array_get(&func_type->fun_params, i);
        string_copy(&var_str, var->var->var_name);
        if (var->annotated_type_enum && var->annotated_type_enum != TYPE_GENERIC) {
            string var_type;
            string_init_chars(&var_type, string_get(var->annotated_type_name));
            string_add_chars(&var_str, ":");
            string_add(&var_str, &var_type);
        }
        array_push(&args, &var_str);
    }
    string joined = string_join(&args, " ");
    string_add_chars(&result, "(");
    string_add(&result, &joined);
    string_add_chars(&result, ")");
    if (func_type->base.annotated_type_name) {
        string_copy_chars(&var_str, string_get(func_type->base.annotated_type_name));
        string_add_chars(&result, ":");
        string_add(&result, &var_str);
    }
    return result;
}

string _dump_function(struct function_node *func)
{
    string result = _dump_func_type(func->func_type);
    string_add_chars(&result, "\n");
    string block_str = _dump_block(func->body);
    string_add(&result, &block_str);
    return result;
}

string _dump_var(struct ast_node *var)
{
    string var_str;
    string_init_chars(&var_str, "var: ");
    string_add(&var_str, var->var->var_name);
    string_add_chars(&var_str, "=");
    string init_value = dump(var->var->init_value);
    string_add(&var_str, &init_value);
    return var_str;
}

string _dump_unary(struct ast_node *unary)
{
    string un;
    string_init_chars(&un, "un: ");
    string_add(&un, unary->unop->op);
    string str_op = dump(unary->unop->operand);
    string_add(&un, &str_op);
    return un;
}

string _dump_binary(struct ast_node *binary)
{
    string lhs_str = dump(binary->binop->lhs);
    string rhs_str = dump(binary->binop->rhs);
    string bin;
    string_init_chars(&bin, "bin:");
    string_add(&bin, binary->binop->op);
    string_add_chars(&bin, "[");
    string_add(&bin, &lhs_str);
    string_add_chars(&bin, ",");
    string_add(&bin, &rhs_str);
    string_add_chars(&bin, "]");
    return bin;
}

string _dump_call(struct call_node *call)
{
    ARRAY_STRING(args);
    for (size_t i = 0; i < array_size(&call->args); i++) {
        string dp = dump(*(struct exp_node **)array_get(&call->args, i));
        array_push(&args, &dp);
    }
    string args_str = string_join(&args, " ");
    string result;
    string_init_chars(&result, string_get(call->callee));
    string_add_chars(&result, " ");
    string_add(&result, &args_str);
    return result;
}

string _dump_if(struct ast_node *cond)
{
    string result;
    string_init_chars(&result, "if ");
    string if_str = dump(cond->cond->if_node);
    string then_str = dump(cond->cond->then_node);
    string_add(&result, &if_str);
    string_add_chars(&result, "then ");
    string_add(&result, &then_str);
    if (cond->cond->else_node) {
        string else_str = dump(cond->cond->else_node);
        string_add_chars(&result, " else ");
        string_add(&result, &else_str);
    }
    return result;
}

string _dump_for(struct for_node *fornode)
{
    string result;
    string_init_chars(&result, "for ");
    string_add(&result, fornode->var_name);
    string_add_chars(&result, " in ");
    string str_start = dump(fornode->start);
    string str_end = dump(fornode->end);
    string_add(&result, &str_start);
    string_add_chars(&result, "..");
    string_add(&result, &str_end);
    return result;
}

string _dump_id(struct ast_node *idnode)
{
    string str_id;
    string_init_chars(&str_id, "id: ");
    string_add(&str_id, idnode->ident->name);
    return str_id;
}

string _dump_number(struct ast_node *node)
{
    string str_num;
    string_init_chars(&str_num, "num: ");
    char double_str[64];
    snprintf(double_str, sizeof(double_str), "%.2f", node->liter->double_val);
    string_add_chars(&str_num, double_str);
    return str_num;
}

string dump(struct exp_node *node)
{
    if (node->node_type == FUNCTION_NODE)
        return _dump_function((struct function_node *)node);
    else if (node->node_type == FUNC_TYPE_NODE)
        return _dump_func_type((struct func_type_node *)node);
    else if (node->node_type == VAR_NODE)
        return _dump_var((struct ast_node *)node);
    else if (node->node_type == UNARY_NODE)
        return _dump_unary((struct ast_node *)node);
    else if (node->node_type == BINARY_NODE)
        return _dump_binary((struct ast_node *)node);
    else if (node->node_type == CONDITION_NODE)
        return _dump_if((struct ast_node *)node);
    else if (node->node_type == CALL_NODE)
        return _dump_call((struct call_node *)node);
    else if (node->node_type == FOR_NODE)
        return _dump_for((struct for_node *)node);
    else if (node->node_type == IDENT_NODE)
        return _dump_id((struct ast_node *)node);
    else if (node->node_type == LITERAL_NODE)
        return _dump_number((struct ast_node *)node);
    else if (node->node_type == BLOCK_NODE)
        return _dump_block((struct block_node *)node);
    else {
        string not_supported;
        string_init_chars(&not_supported, "ast->node_type not supported: ");
        string_add_chars(&not_supported, node_type_strings[node->node_type]);
        return not_supported;
    }
}
