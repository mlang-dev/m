/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * provide functions to evaluate the ast nodes, this is primarily used in compiler optimization.
 * 
 */
#include "sema/eval.h"
#include <assert.h>

int eval(struct ast_node *node)
{
    if(node->transformed) node = node->transformed;
    switch(node->node_type){
    default:
        assert(false);
    case IDENT_NODE:
        return eval(node->ident->var->var->init_value);
    case UNARY_NODE:
        if(node->unop->opcode == OP_PLUS)
            return eval(node->unop->operand);
        else if(node->unop->opcode == OP_MINUS)
            return -eval(node->unop->operand);
        break;
    case LITERAL_NODE:
        return node->liter->int_val;
    case BINARY_NODE:
        {
            int lhs = eval(node->binop->lhs);
            int rhs = eval(node->binop->rhs);
            switch (node->binop->opcode)
            {
            case OP_PLUS:
                /* code */
                return lhs + rhs;
            case OP_STAR:
                return lhs * rhs;
            default:
                assert(false);
                break;
            }
        }
    }
    return 0;
}
