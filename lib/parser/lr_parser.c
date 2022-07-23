/*
 * lr_parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement an LR parser, taking a EBNF grammar text and parse text 
 * into ast for the grammar
 */
#include "parser/lr_parser.h"
#include "clib/stack.h"
#include "clib/util.h"
#include "parser/grammar.h"
#include <assert.h>

#define END_OF_RULE 0xff

link_list_append_data_fn(index_list, index_list_entry, u8)
link_list_append_data_fn(parse_item_list, parse_item_list_entry, struct parse_item)

bool _exists(struct index_list *dst, u8 data)
{
    struct index_list_entry *entry;
    list_foreach(entry, dst)
    {
        if(entry->data == data)
            return true;
    }
    return false;
}

int _append_list(struct index_list *dst, struct index_list *src)
{
    struct index_list_entry *entry;
    int n = 0;
    list_foreach(entry, src)
    {
        if (entry->data != TOKEN_EPSILON && !_exists(dst, entry->data)) {
            index_list_append_data(dst, entry->data);
            n++;
        }
    }
    return n;
}

int _append_data(struct index_list *dst, u8 data)
{
    int n = 0;
    if (!_exists(dst, data)) {
        index_list_append_data(dst, data);
        n++;
    }
    return n;
}

/*expand bracket in grammar*/
void _expr_2_gr(struct expr *expr, struct parse_rule *gr)
{
    struct expr_item *ei;
    gr->symbol_count = 0;
    for(size_t i=0; i < array_size(&expr->items); i++){
        assert(i < MAX_SYMBOLS_RULE);
        ei = (struct expr_item *)array_get(&expr->items, i);
        gr->rhs[gr->symbol_count++] = get_symbol_index(ei->sym);
    }
    gr->action = expr->action;
}

void _expand_expr(struct expr *rule_expr, struct array *a)
{
    int which_in_items = -1;
    size_t i;
    struct expr_item *ei;
    struct array symbols;
    symbol s;
    array_init(&symbols, sizeof(symbol));
    for(i = 0; i < array_size(&rule_expr->items); i++){
        ei = array_get(&rule_expr->items, i);
        if(ei->ei_type == EI_IN_MATCH){
            which_in_items = i;
            const char *sym = string_get(ei->sym);
            while(*sym){
                s = to_symbol2(sym, 1);
                array_push(&symbols, &s);
                sym++;
            }
            break;
        }
    }
    struct expr *expr;
    if(which_in_items < 0){
        array_push(a, rule_expr);
        expr = array_back(a);
        array_copy(&expr->items, &rule_expr->items);
    }else{
        for(i=0; i < array_size(&symbols); i++){
            array_push(a, rule_expr);
            expr = array_back(a);
            array_copy(&expr->items, &rule_expr->items);
            ei = array_get(&expr->items, which_in_items);
            ei->ei_type = EI_TOKEN_MATCH;
            ei->sym = *(symbol*)array_get(&symbols, i);
        }
    }
    array_deinit(&symbols);
}

void _init_index_list(struct index_list *list)
{
    list->first = 0;
    list->len = 0;
    list->tail = 0;
}

void _init_parse_item_list(struct parse_item_list *list)
{
    list->first = 0;
    list->len = 0;
    list->tail = 0;
}

bool _is_nullable(u8 *symbols, u8 symbol_count, struct rule_symbol_data *symbol_data)
{
    for(u8 i = 0; i < symbol_count; i ++){
        if(!symbol_data[symbols[i]].is_nullable) return false;
    }
    return true;
}

void _compute_is_nullable(struct parse_rule *rules, u16 rule_count, struct rule_symbol_data *symbol_data)
{
    struct parse_rule *rule;
    int change_count = 0;
    do {
        change_count = 0;
        for (u16 i = 0; i < rule_count; i++) {
            rule = &rules[i];
            if ((rule->symbol_count == 1 && rule->rhs[0] == TOKEN_EPSILON) || _is_nullable(rule->rhs, rule->symbol_count, symbol_data)) {
                symbol_data[rule->lhs].is_nullable = true;
                change_count ++;
            } 
        }
    } while (change_count > 0);
}

int _append_first_set_to(struct index_list *dst, struct parse_rule *rule, u8 rhs_start_pos, struct rule_symbol_data *symbol_data)
{
    bool is_previous_symbol_nullable = true;
    int change_count = 0;
    for (int j = rhs_start_pos; j < rule->symbol_count; j++) {
        if (is_previous_symbol_nullable) {
            change_count += _append_list(dst, &symbol_data[rule->rhs[j]].first_list);
        }
        is_previous_symbol_nullable = symbol_data[rule->rhs[j]].is_nullable;
    }
    return change_count;
}

void _compute_first_set(struct parse_rule *rules, u16 rule_count, struct rule_symbol_data *symbol_data)
{
    struct parse_rule *rule;
    int change_count = 0;
    u16 i;
    for (i = 0; i < TERMINAL_COUNT; i++) {
        // terminal symbol's first set is itself
        index_list_append_data(&symbol_data[i].first_list, i);
    }
    do {
        change_count = 0;
        for (i = 0; i < rule_count; i++)
        {
            rule = &rules[i];
            change_count += _append_first_set_to(&symbol_data[rule->lhs].first_list, rule, 0, symbol_data);
            //adding epsilon separately
            if (symbol_data[rule->lhs].is_nullable) {
                change_count += _append_data(&symbol_data[rule->lhs].first_list, TOKEN_EPSILON);
            }
        }
    } while (change_count > 0);
}

void _compute_follow_set(struct parse_rule *rules, u16 rule_count, struct rule_symbol_data *symbol_data)
{
    struct parse_rule *rule;
    int change_count = 0;
    u16 i, j;
    //put $ EOF into the start symbol's follow set
    index_list_append_data(&symbol_data[rules[0].lhs].follow_list, TOKEN_EOF);
    do {
        change_count = 0;
        for (i = 0; i < rule_count; i++) {
            rule = &rules[i];
            for (j = 0; j < rule->symbol_count; j++) {
                if(is_terminal(rule->rhs[j])) continue;
                change_count += _append_first_set_to(&symbol_data[rule->rhs[j]].follow_list, rule, j + 1, symbol_data);
                //
                if(j == rule->symbol_count - 1 || _is_nullable(&rule->rhs[j+1], rule->symbol_count -1 - j, symbol_data)){
                    change_count += _append_list(&symbol_data[rule->rhs[j]].follow_list, &symbol_data[rule->lhs].follow_list);
                }
            }
        }
    } while (change_count > 0);
}

void _fill_rule_symbol_data(struct parse_rule *rules, u16 rule_count, struct rule_symbol_data *symbol_data)
{
    _compute_is_nullable(rules, rule_count, symbol_data);
    _compute_first_set(rules, rule_count, symbol_data);
    _compute_follow_set(rules, rule_count, symbol_data);
    /*add rule index to each grammar symbol*/
    struct index_list *il;
    struct parse_rule *rule;
    for (u16 i = 0; i < rule_count; i++) {
        rule = &rules[i];
        il = &symbol_data[rule->lhs].rule_list;
        index_list_append_data(il, i);
    }
}

u8 _eq_parse_item(struct parse_item *item1, struct parse_item *item2)
{
    return item1->rule == item2->rule && item1->dot == item2->dot;
}

bool _exists_parse_item(struct parse_item_list *items, struct parse_item *item)
{
    struct parse_item_list_entry *entry;
    list_foreach(entry, items){
        if(_eq_parse_item(&entry->data, item))
            return true;
    }
    return false;
}

u8 _add_parse_item(struct parse_item_list *items, struct parse_item item)
{
    if(_exists_parse_item(items, &item))
        return 0;
    parse_item_list_append_data(items, item);
    return 1;
}

struct parse_state _closure(struct rule_symbol_data *symbol_data, struct parse_rule *rules, struct parse_state state)
{
    struct parse_item item;
    struct index_list_entry *rule_entry;
    int items_added = 0;
    struct parse_item_list_entry *entry;
    struct parse_item_list *items = &state.items;
    list_foreach(entry, items)
    {
        struct parse_rule *rule = &rules[entry->data.rule];
        if(entry->data.dot < rule->symbol_count){
            u8 symbol_index = rule->rhs[entry->data.dot];
            if(!is_terminal(symbol_index)){//non terminal
                struct index_list *nt_rules = &symbol_data[symbol_index].rule_list;
                list_foreach(rule_entry, nt_rules){
                    item.rule = rule_entry->data; //ile->data stores the index of the rule
                    item.dot = 0;
                    items_added += _add_parse_item(items, item);
                }
            }
        }
    }
    printf("items added: %d\n", items_added);
    return state;
}

struct parse_state _goto(struct rule_symbol_data *symbol_data, struct parse_rule *rules, struct parse_state state, u8 rule_symbol)
{
    struct parse_state next_state;
    _init_parse_item_list(&next_state.items);
    next_state.kernel_item_count = 0;
    struct parse_item_list *next_items = &next_state.items;

    struct parse_item_list_entry *entry;
    struct parse_item_list *items = &state.items;
    struct parse_item item;
    struct parse_rule *rule;
    list_foreach(entry, items)
    {
        rule = &rules[entry->data.rule];
        if(entry->data.dot < rule->symbol_count && rule_symbol == rule->rhs[entry->data.dot]){
            item = entry->data;
            item.dot++;
            parse_item_list_append_data(next_items, item);
            next_state.kernel_item_count++;
        }
    }
    return next_state;
}

bool _eq_state(struct parse_state *state1, struct parse_state *state2)
{
    if(state1->kernel_item_count != state2->kernel_item_count)
        return false;
    struct parse_item_list_entry *entry1;
    struct parse_item_list_entry *entry2 = list_first(&state2->items);
    u8 top_items = 0;
    list_foreach(entry1, &state1->items){
        if (top_items ++ == state1->kernel_item_count)
            break;
        // only compare kernel
        if(_eq_parse_item(&entry1->data, &entry2->data)){
            entry2 = list_next(entry2);
            continue;
        }else{
            return false;
        }
    }
    return true;
}

int _find_state(struct parse_state *states, u16 state_count, struct parse_state *state)
{
    for(u16 i = 0; i < state_count; i++){
        if(_eq_state(&states[i], state)) 
            return i;
    }
    return -1;
}

bool _exists_in_array(u8 *array, u8 size, u8 match)
{
    for (u8 i = 0; i < size; i++){
        if (array[i] == match)
            return true;
    }
    return false;
}

u16 _build_states(struct rule_symbol_data *symbol_data, struct parse_rule *rules, u16 rule_count, struct parse_state *states, struct parser_action parsing_table[][MAX_GRAMMAR_SYMBOLS])
{
    u16 i, state_count = 0;
    struct parse_item item;
    struct parse_state *state;
    _init_parse_item_list(&states[state_count].items);
    item.dot = 0;
    //item.lookahead = TOKEN_EOF;
    item.rule = 0;
    parse_item_list_append_data(&states[state_count].items, item);
    states[state_count].kernel_item_count = 1;
    states[state_count] = _closure(symbol_data, rules, states[state_count]);
    state_count++;
    struct parse_item_list_entry *entry;
    struct parse_rule *rule;
    for(i = 0; i < state_count; i++){
        state = &states[i];

        //iterate each rule to get unique symbol to create new state 
        u8 visited_symbols[16];
        u8 visited_count = 0;
        list_foreach(entry, &state->items){
            rule = &rules[entry->data.rule];
            if(entry->data.dot >= rule->symbol_count) {
                continue;
            }
            u8 x = rule->rhs[entry->data.dot]; 
            if(_exists_in_array(visited_symbols, visited_count, x)){
                continue;
            }
            visited_symbols[visited_count ++] = x;
            struct parse_state next_state = _goto(symbol_data, rules, *state, x);
            int existing_state_index = _find_state(states, state_count, &next_state);
                // if not in the states, then closure the state and add it to states
            struct parser_action pa;
            pa.code = is_terminal(x) ? ACTION_SHIFT : ACTION_GOTO;
            if (existing_state_index < 0) {
                pa.state_index = state_count;
                states[state_count++] = _closure(symbol_data, rules, next_state);
            } else {
                pa.state_index = existing_state_index;
            }
            parsing_table[i][x] = pa;
        }
    }
    return state_count;
}

void _build_parsing_table(struct rule_symbol_data *symbol_data, struct parser_action parsing_table[][MAX_GRAMMAR_SYMBOLS], u16 state_count, struct parse_state *states, struct parse_rule *rules)
{
    struct parse_state *state;
    struct parser_action *action;
    struct parse_item_list_entry *entry;
    struct parse_item *item;
    struct parse_rule *rule;
    struct index_list *follow_list;
    struct index_list_entry *follow_entry;
    for(u16 i=0; i < state_count; i++){
        state = &states[i];
        list_foreach(entry, &state->items){
            item = &entry->data;
            rule = &rules[item->rule];
            if(item->dot == rule->symbol_count && item->rule > 0){/*except the augumented one*/
                /*we do reduction here. get follow set of the rule's nonterm symbol, for each
                symbol in follow set we do reduction*/
                follow_list = &symbol_data[rules[item->rule].lhs].follow_list;
                list_foreach(follow_entry, follow_list){
                    action = &parsing_table[i][follow_entry->data];
                    action->code = ACTION_REDUCE;
                    action->rule_index = item->rule;
                }
            }
            else if(item->dot == rule->symbol_count && item->rule == 0){/*the augumented one*/
                action = &parsing_table[i][TOKEN_EOF];
                action->code = ACTION_ACCEPT;                
            }
        }
    }
}

void _convert_grammar_rules_to_parse_rules(struct grammar *g, struct lr_parser *parser)
{
    struct parse_rule *gr;
    struct expr *rule_expr, *expr;
    struct rule *rule;
    u8 nonterm;
    size_t i, j, k;
    parser->rule_count = 0;
    for (i = 0; i < (u16)array_size(&g->rules); i++) {
        rule = *(struct rule **)array_get(&g->rules, i);
        nonterm = get_symbol_index(rule->nonterm);
        for (j = 0; j < array_size(&rule->exprs); j++) {
            rule_expr = (struct expr *)array_get(&rule->exprs, j);
            struct array exprs;
            array_init_free(&exprs, sizeof(struct expr), (free_fun)expr_deinit);
            _expand_expr(rule_expr, &exprs);
            for (k = 0; k < array_size(&exprs); k++) {
                expr = array_get(&exprs, k);
                gr = &parser->rules[parser->rule_count++];
                gr->lhs = nonterm;
                _expr_2_gr(expr, gr);
            }
            array_deinit(&exprs);
        }
    }
}

struct lr_parser *lr_parser_new(const char *grammar_text)
{
    size_t i,j;
    struct lr_parser *parser;
    MALLOC(parser, sizeof(*parser));
    parser->stack_top = 0;
    //1. initialize parsing table and symbol data
    //row: state index, col: symbol index
    for(i=0; i < MAX_STATES; i++){
        for(j=0; j < MAX_GRAMMAR_SYMBOLS; j++){
            parser->parsing_table[i][j].code = ACTION_ERROR;
            parser->parsing_table[i][j].state_index = 0;
        }
    }
    for (i = 0; i < get_symbol_count(); i++) {
        _init_index_list(&parser->symbol_data[i].first_list);
        _init_index_list(&parser->symbol_data[i].follow_list);
        _init_index_list(&parser->symbol_data[i].rule_list);
        parser->symbol_data[i].is_nullable = false;
    }

    //2. registering non-term symbols with integer
    struct grammar *g = grammar_parse(grammar_text);
    struct rule *rule;
    for(i = 0; i < array_size(&g->rules); i++){
        rule = *(struct rule **)array_get(&g->rules, i);
        u8 index = register_grammar_nonterm(rule->nonterm); //register new non-term symbol
        assert((u8)i + TERMINAL_COUNT == index);
    }

    //3. convert grammar to replace symbol with index:
    //all grammar symbol: non-terminal or terminal (token) 
    //has an integer of index representing itself
    _convert_grammar_rules_to_parse_rules(g, parser);

    //4. calculate production rule's nonterm's first set, follow set
    _fill_rule_symbol_data(parser->rules, parser->rule_count, parser->symbol_data);
    
    //5. build states
    parser->parse_state_count = _build_states(parser->symbol_data, parser->rules, parser->rule_count, parser->parse_states, parser->parsing_table);

    //6. construct parsing table
    //action: state, terminal and goto: state, nonterm
    _build_parsing_table(parser->symbol_data, parser->parsing_table, parser->parse_state_count, parser->parse_states, parser->rules);
    parser->g = g;
    return parser;
}

void lr_parser_free(struct lr_parser *parser)
{
    grammar_free(parser->g);
    FREE(parser);
}

void _push_state(struct lr_parser *parser, u16 state, struct ast_node *ast)
{
    struct stack_item *si = &parser->stack[parser->stack_top++];
    si->state_index = state;
    si->ast = ast;
}

struct stack_item *_pop_state(struct lr_parser *parser)
{
    return &parser->stack[--parser->stack_top];
}

struct stack_item *_get_top_state(struct lr_parser *parser)
{
    return &parser->stack[parser->stack_top-1];
}

struct stack_item *_get_start_item(struct lr_parser *parser, u8 symbol_count)
{
    return &parser->stack[parser->stack_top-symbol_count];
}

void _pop_states(struct lr_parser *parser, u8 symbol_count)
{
    assert(parser->stack_top >= symbol_count);
    parser->stack_top -= symbol_count;
}

struct ast_node *_build_terminal_ast(struct token *tok)
{
    enum node_type node_type = token_to_node_type(tok->token_type, tok->opcode);
    struct ast_node *ast = 0;
    switch(node_type){
        default:
            ast = ast_node_new(node_type, 0, tok->loc);
            break;
        case IDENT_NODE:
            ast = ident_node_new(tok->symbol_val, tok->loc);
            break;
    }
    return ast;
}

struct ast_node *_build_nonterm_ast(struct parse_rule *rule, struct stack_item *items)
{
    enum op_code opcode;
    struct ast_node *ast = 0;
    struct ast_node *node = 0;
    struct ast_node *rhs = 0;
    if (!rule->action.action){
        return items[0].ast;
    }
    enum node_type node_type = symbol_to_node_type(rule->action.action);
    switch (node_type) {
    default:
        assert(false);
        break;
    case UNARY_NODE:
        node = items[0].ast;
        opcode = node->node_type & 0xFFFF;
        node = items[1].ast;
        ast = unary_node_new(opcode, node, node->loc);
        break;
    case BINARY_NODE:
        node = items[1].ast;
        opcode = node->node_type & 0xFFFF;
        node = items[0].ast;
        rhs = items[2].ast;
        ast = binary_node_new(opcode, node, rhs, node->loc);
        break;
    case FUNC_NODE:
        node = items[0].ast;
        assert(node->node_type == IDENT_NODE);
        ARRAY_FUN_PARAM(fun_params);
        struct ast_node *ft = func_type_node_default_new(node->ident->name, &fun_params, 0, false, false, node->loc);
        node = items[1].ast;
        ast = function_node_new(ft, node, node->loc);
        break;
    }
    return ast;
}

struct ast_node *parse_text(struct lr_parser *parser, const char *text)
{
    struct ast_node *ast = 0;
    _push_state(parser, 0, 0); 
    struct lexer *lexer = lexer_new_for_string(text);
    struct token *tok = get_tok(lexer);
    u8 a = get_token_index(tok->token_type, tok->opcode);
    u16 s, t;
    struct parse_rule *rule;
    struct stack_item *si;
    struct parser_action *pa;
    //driver 
    while(1){
        s = _get_top_state(parser)->state_index;
        pa = &parser->parsing_table[s][a];
        if(pa->code == ACTION_SHIFT){
            ast = _build_terminal_ast(tok);
            _push_state(parser, pa->state_index, ast);
            tok = get_tok(lexer);
            a = get_token_index(tok->token_type, tok->opcode);
        }else if(pa->code == ACTION_REDUCE){
            //do reduce action and build ast node
            rule = &parser->rules[pa->rule_index];
            si = _get_start_item(parser, rule->symbol_count);
            ast = _build_nonterm_ast(rule, si); //build ast according to the rule 
            _pop_states(parser, rule->symbol_count);
            t = _get_top_state(parser)->state_index;
            assert(parser->parsing_table[t][rule->lhs].code == ACTION_GOTO);
            _push_state(parser, parser->parsing_table[t][rule->lhs].state_index, ast);
            //
        }else if(pa->code == ACTION_ACCEPT){
            si = _pop_state(parser);
            ast = si->ast;
            break;
        }else{
            //error recovery
            break;
        }
    }
    lexer_free(lexer);
    return ast;
}
