/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * AST print functions
 */
#include "parser/astdump.h"
#include "clib/string.h"
#include "clib/util.h"
#include "parser/m_grammar.h"

string _dump_block(struct ast_node *node)
{
    string block;
    string_init_chars(&block, "");
    for (size_t i = 0; i < array_size(&node->block->nodes); i++) {
        struct ast_node *stmt = *(struct ast_node **)array_get(&node->block->nodes, i);
        string str_stmt = dump(stmt);
        string_add(&block, &str_stmt);
    }
    return block;
}

string _dump_func_type(struct ast_node *func_type)
{
    string result;
    string_init(&result);
    if (func_type->ft->is_extern)
        string_add_chars(&result, "extern ");
    string_add_chars(&result, string_get(func_type->ft->name));
    ARRAY_STRING(args);
    string var_str;
    string_init(&var_str);
    for (size_t i = 0; i < array_size(&func_type->ft->params->block->nodes); i++) {
        struct ast_node *var = *(struct ast_node **)array_get(&func_type->ft->params->block->nodes, i);
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
    if (func_type->annotated_type_name) {
        string_copy_chars(&var_str, string_get(func_type->annotated_type_name));
        string_add_chars(&result, ":");
        string_add(&result, &var_str);
    }
    return result;
}

string _dump_function(struct ast_node *func)
{
    string result = _dump_func_type(func->func->func_type);
    string_add_chars(&result, "\n");
    string block_str = _dump_block(func->func->body);
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
    string_add_chars(&un, get_opcode(unary->unop->opcode));
    string str_op = dump(unary->unop->operand);
    string_add(&un, &str_op);
    return un;
}

string _dump_binary(struct ast_node *binary)
{
    string lhs_str = dump(binary->binop->lhs);
    string rhs_str = dump(binary->binop->rhs);
    string bin;
    string_init_chars(&bin, "");
    string_add_chars(&bin, "(");
    string_add(&bin, &lhs_str);
    string_add_chars(&bin, get_opcode(binary->binop->opcode));
    string_add(&bin, &rhs_str);
    string_add_chars(&bin, ")");
    return bin;
}

string _dump_call(struct ast_node *call)
{
    ARRAY_STRING(args);
    for (size_t i = 0; i < array_size(&call->call->args); i++) {
        string dp = dump(*(struct ast_node **)array_get(&call->call->args, i));
        array_push(&args, &dp);
    }
    string args_str = string_join(&args, " ");
    string result;
    string_init_chars(&result, string_get(call->call->callee));
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

string _dump_for(struct ast_node *fornode)
{
    string result;
    string_init_chars(&result, "for ");
    string_add(&result, fornode->forloop->var_name);
    string_add_chars(&result, " in ");
    string str_start = dump(fornode->forloop->start);
    string str_end = dump(fornode->forloop->end);
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
    string_init_chars(&str_num, "");
    char double_str[64];
    snprintf(double_str, sizeof(double_str), "%d", node->liter->int_val);
    string_add_chars(&str_num, double_str);
    return str_num;
}

string dump(struct ast_node *node)
{
    if (node->node_type == FUNC_NODE)
        return _dump_function(node);
    else if (node->node_type == FUNC_TYPE_NODE)
        return _dump_func_type(node);
    else if (node->node_type == VAR_NODE)
        return _dump_var(node);
    else if (node->node_type == UNARY_NODE)
        return _dump_unary(node);
    else if (node->node_type == BINARY_NODE)
        return _dump_binary(node);
    else if (node->node_type == IF_NODE)
        return _dump_if(node);
    else if (node->node_type == CALL_NODE)
        return _dump_call(node);
    else if (node->node_type == FOR_NODE)
        return _dump_for(node);
    else if (node->node_type == IDENT_NODE)
        return _dump_id(node);
    else if (node->node_type == LITERAL_NODE)
        return _dump_number(node);
    else if (node->node_type == BLOCK_NODE)
        return _dump_block(node);
    else {
        string not_supported;
        string_init_chars(&not_supported, "ast->node_type not supported: ");
        string_add_chars(&not_supported, node_type_strings[node->node_type]);
        return not_supported;
    }
}
