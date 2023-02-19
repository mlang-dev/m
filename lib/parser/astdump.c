/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * AST print functions
 */
#include "parser/astdump.h"
#include "sema/analyzer.h"
#include "clib/string.h"
#include "clib/util.h"
#include <assert.h>

string _dump_block(struct sema_context *context, struct ast_node *node)
{
    string block;
    string_init_chars(&block, "");
    for (size_t i = 0; i < array_size(&node->block->nodes); i++) {
        struct ast_node *stmt = *(struct ast_node **)array_get(&node->block->nodes, i);
        string str_stmt = dump(context, stmt);
        string_add(&block, &str_stmt);
    }
    return block;
}

string _dump_func_type(struct sema_context *context, struct ast_node *func_type)
{
    string result;
    string_init(&result);
    string var_str;
    string_init(&var_str);
    if (func_type->ft->is_extern)
        string_add_chars(&result, "func ");
    string_add_chars(&result, string_get(func_type->ft->name));
    ARRAY_STRING(args);
    for (size_t i = 0; i < array_size(&func_type->ft->params->block->nodes); i++) {
        struct ast_node *var = *(struct ast_node **)array_get(&func_type->ft->params->block->nodes, i);
        string_copy(&var_str, var->var->var->ident->name);
        if (var->var->is_of_type && (var->var->is_of_type->type_node->kind == TypeName || var->var->is_of_type->type_node->kind == BuiltinType) 
            &&var->var->is_of_type->type_node->type_name) {
            enum type type_enum = get_type_enum_from_symbol(var->var->is_of_type->type_node->type_name);
            if(type_enum != TYPE_GENERIC){
                string var_type;
                string_init_chars(&var_type, string_get(var->var->is_of_type->type_node->type_name));
                string_add_chars(&var_str, ":");
                string_add(&var_str, &var_type);
            }
        }
        array_push(&args, &var_str);
    }
    string joined = string_join(&args, " ");
    string_add_chars(&result, " ");
    string_add(&result, &joined);
    if (array_size(&func_type->ft->params->block->nodes) == 0){
        string_add_chars(&result, "()");
    }

    // function type
    if (func_type->ft->ret_type_node) {
        struct type_expr *type = create_type_from_type_node(context, func_type->ft->ret_type_node->type_node, Immutable);
        string_copy_chars(&var_str, string_get(type->name));
        string_add_chars(&result, " -> ");
        string_add(&result, &var_str);
    }
    return result;
}

string _dump_function(struct sema_context *context, struct ast_node *func)
{
    string result = _dump_func_type(context, func->func->func_type);
    string_add_chars(&result, "\n");
    string block_str = _dump_block(context, func->func->body);
    string_add(&result, &block_str);
    return result;
}

string _dump_var(struct sema_context *context, struct ast_node *var)
{
    string var_str;
    string_init_chars(&var_str, "var: ");
    string_add(&var_str, var->var->var->ident->name);
    string_add_chars(&var_str, "=");
    string init_value = dump(context, var->var->init_value);
    string_add(&var_str, &init_value);
    return var_str;
}

string _dump_unary(struct sema_context *context, struct ast_node *unary)
{
    string un;
    string_init_chars(&un, "");
    string_add_chars(&un, get_opcode(unary->unop->opcode));
    string str_op = dump(context, unary->unop->operand);
    string_add(&un, &str_op);
    return un;
}

string _dump_binary(struct sema_context *context, struct ast_node *binary)
{
    string lhs_str = dump(context, binary->binop->lhs);
    string rhs_str = dump(context, binary->binop->rhs);
    string bin;
    string_init_chars(&bin, "");
    string_add_chars(&bin, "(");
    string_add(&bin, &lhs_str);
    string_add_chars(&bin, get_opcode(binary->binop->opcode));
    string_add(&bin, &rhs_str);
    string_add_chars(&bin, ")");
    return bin;
}

string _dump_field_access(struct sema_context *context, struct ast_node *member_index)
{
    string lhs_str = dump(context, member_index->index->object);
    string rhs_str = dump(context, member_index->index->index);
    string field;
    string_init_chars(&field, "");
    string_add(&field, &lhs_str);
    string_add_chars(&field, ".");
    string_add(&field, &rhs_str);
    return field;
}

string _dump_array_index(struct sema_context *context, struct ast_node *array_index)
{
    string lhs_str = dump(context, array_index->index->object);
    string rhs_str = dump(context, array_index->index->index);
    string field;
    string_init_chars(&field, "");
    string_add(&field, &lhs_str);
    string_add_chars(&field, "[");
    string_add(&field, &rhs_str);
    string_add_chars(&field, "]");
    return field;
}

string _dump_call(struct sema_context *context, struct ast_node *call)
{
    ARRAY_STRING(args);
    for (size_t i = 0; i < array_size(&call->call->arg_block->block->nodes); i++) {
        string dp = dump(context, *(struct ast_node **)array_get(&call->call->arg_block->block->nodes, i));
        array_push(&args, &dp);
    }
    string args_str = string_join(&args, " ");
    string result;
    string_init_chars(&result, string_get(call->call->callee));
    string_add_chars(&result, " ");
    string_add(&result, &args_str);
    return result;
}

string _dump_if(struct sema_context *context, struct ast_node *cond)
{
    string result;
    string_init_chars(&result, "if ");
    string if_str = dump(context, cond->cond->if_node);
    string then_str = dump(context, cond->cond->then_node);
    string_add(&result, &if_str);
    string_add_chars(&result, "then ");
    string_add(&result, &then_str);
    if (cond->cond->else_node) {
        string else_str = dump(context, cond->cond->else_node);
        string_add_chars(&result, " else ");
        string_add(&result, &else_str);
    }
    return result;
}

string _dump_for(struct sema_context *context, struct ast_node *fornode)
{
    string result;
    string_init_chars(&result, "for ");
    string_add(&result, fornode->forloop->var->var->var->ident->name);
    string_add_chars(&result, " in ");
    string str_start = dump(context, fornode->forloop->range->range->start);
    string str_end = dump(context, fornode->forloop->range->range->end);
    string_add(&result, &str_start);
    string_add_chars(&result, "..");
    string_add(&result, &str_end);
    return result;
}

string _dump_id(struct sema_context *context, struct ast_node *idnode)
{
    string str_id;
    string_init_chars(&str_id, "");
    string_add(&str_id, idnode->ident->name);
    return str_id;
}

string _dump_number(struct sema_context *context, struct ast_node *node)
{
    string str_num;
    string_init_chars(&str_num, "");
    char double_str[64];
    snprintf(double_str, sizeof(double_str), "%d", node->liter->int_val);
    string_add_chars(&str_num, double_str);
    return str_num;
}

string dump(struct sema_context *context, struct ast_node *node)
{
    switch (node->node_type){
    case FUNC_NODE:
        return _dump_function(context, node);
    case FUNC_TYPE_NODE:
        return _dump_func_type(context, node);
    case VAR_NODE:
        return _dump_var(context, node);
    case UNARY_NODE:
        return _dump_unary(context, node);
    case BINARY_NODE:
    case ASSIGN_NODE:
        return _dump_binary(context, node);
    case MEMBER_INDEX_NODE:
        if(node->index->aggregate_type == AGGREGATE_TYPE_RECORD)
            return _dump_field_access(context, node);        
        else {
            assert(node->index->aggregate_type == AGGREGATE_TYPE_ARRAY);
            return _dump_array_index(context, node);
        }
        break;
    case IF_NODE:
        return _dump_if(context, node);
    case CALL_NODE:
        return _dump_call(context, node);
    case FOR_NODE:
        return _dump_for(context, node);
    case IDENT_NODE:
        return _dump_id(context, node);
    case LITERAL_NODE:
        return _dump_number(context, node);
    case BLOCK_NODE:
        return _dump_block(context, node);
    default:
        {
            string not_supported;
            string_init_chars(&not_supported, "ast->node_type not supported: ");
            string_add_chars(&not_supported, node_type_strings[node->node_type]);
            return not_supported;
        }
    }
}
