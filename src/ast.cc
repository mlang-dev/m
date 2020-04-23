#include "ast.h"
extern std::map<std::string, int> g_op_precedences;

bool is_unary_op(prototype_node* pnode)
{
    return pnode->is_operator && array_size(&pnode->args) == 1;
}

bool is_binary_op(prototype_node* pnode)
{
    return pnode->is_operator && array_size(&pnode->args) == 2;
}

char get_op_name(prototype_node* pnode)
{
    assert(is_unary_op(pnode) || is_binary_op(pnode));
    return string_back(&pnode->name);
}

function_node* create_function_node(prototype_node* prototype,
    block_node* body)
{
    auto node = new function_node();
    node->base.node_type = NodeType::FUNCTION_NODE;
    node->base.parent = (exp_node*)prototype;
    node->base.loc = prototype->base.loc;
    node->prototype = prototype;
    node->body = body;
    if (is_binary_op(prototype)) {
        g_op_precedences[std::string(string_get(&prototype->op))] = prototype->precedence;
    }
    return node;
}

ident_node* create_ident_node(exp_node* parent, source_loc loc, const char *name)
{
    auto node = new ident_node();
    node->base.node_type = NodeType::IDENT_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    return node;
}

num_node* create_num_node(exp_node* parent, source_loc loc, double val)
{
    string str;
    string_init_chars(&str, "double");
    auto node = new num_node();
    node->base.node_type = NUMBER_NODE;
    node->base.type.name = str;
    node->base.parent = parent;
    node->base.loc = loc;
    node->double_val = val;
    return node;
}

num_node* create_num_node(exp_node* parent, source_loc loc, int val)
{
    string str;
    string_init_chars(&str, "int");
    auto node = new num_node();
    node->base.node_type = NUMBER_NODE;
    node->base.type.name = str;
    node->base.parent = parent;
    node->base.loc = loc;
    node->double_val = val;
    return node;
}

var_node* create_var_node(exp_node* parent, source_loc loc, const char *var_name, exp_node* init_value)
{
    auto node = new var_node();
    node->base.node_type = VAR_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->var_name, var_name);
    node->init_value = init_value;
    return node;
}

call_node* create_call_node(exp_node* parent, source_loc loc, const char *callee,
    std::vector<exp_node*>& args)
{
    auto node = new call_node();
    node->base.node_type = NodeType::CALL_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->callee, callee);
    node->args = args;
    return node;
}

prototype_node* create_prototype_node(exp_node* parent, source_loc loc, const char *name,
    array* args,
    bool is_operator, unsigned precedence, const char *op)
{
    auto node = new prototype_node();
    node->base.node_type = NodeType::PROTOTYPE_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->name, name);
    node->args = *args;
    node->is_operator = is_operator;
    node->precedence = precedence;
    string_init_chars(&node->op, op);
    return node;
}

condition_node* create_if_node(exp_node* parent, source_loc loc, exp_node* condition, exp_node* then_node,
    exp_node* else_node)
{
    auto node = new condition_node();
    node->base.node_type = NodeType::CONDITION_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    node->condition_node = condition;
    node->then_node = then_node;
    node->else_node = else_node;
    return node;
}

unary_node* create_unary_node(exp_node* parent, source_loc loc, const char *op, exp_node* operand)
{
    auto node = new unary_node();
    node->base.node_type = NodeType::UNARY_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->op, op);
    node->operand = operand;
    return node;
}

binary_node* create_binary_node(exp_node* parent, source_loc loc, const char *op, exp_node* lhs, exp_node* rhs)
{
    auto node = new binary_node();
    node->base.node_type = NodeType::BINARY_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->op, op);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

for_node* create_for_node(exp_node* parent, source_loc loc, const char *var_name, exp_node* start,
    exp_node* end, exp_node* step, exp_node* body)
{
    auto node = new for_node();
    node->base.node_type = NodeType::FOR_NODE;
    node->base.parent = parent;
    node->base.loc = loc;
    string_init_chars(&node->var_name, var_name);
    node->start = start;
    node->end = end;
    node->step = step;
    node->body = body;
    return node;
}

block_node* create_block_node(exp_node* parent, std::vector<exp_node*>& nodes)
{
    block_node* block = new block_node();
    block->base.node_type = NodeType::BLOCK_NODE;
    block->base.parent = parent;
    block->base.loc = nodes[0]->loc;
    for (auto node : nodes)
        block->nodes.push_back(node);
    return block;
}

module* create_module(const char* mod_name, FILE* file)
{
    module* mod = new module();
    string_init_chars(&mod->name, mod_name);
    mod->block = new block_node();
    mod->tokenizer = create_tokenizer(file);
    return mod;
}