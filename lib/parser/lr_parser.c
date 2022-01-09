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

link_list_add_data_fn(index_list, index_list_entry, u8)
link_list_add_data_fn(parse_item_list, parse_item_list_entry, struct parse_item)

bool _exists(struct index_list *dst, u8 data)
{
    struct index_list_entry *entry;
    list_foreach(entry, dst, list)
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
    list_foreach(entry, src, list)
    {
        if(!_exists(dst, entry->data)){
            index_list_add_data_to_head(dst, entry->data);
            n++;
        }
    }
    return n;
}

/*expand bracket in grammar*/
void _expr_2_gr(struct expr *expr, struct grule *gr)
{
    struct expr_item *ei;
    gr->symbol_count = 0;
    for(size_t i=0; i < array_size(&expr->items); i++){
        assert(i < MAX_SYMBOLS_RULE);
        ei = (struct expr_item *)array_get(&expr->items, i);
        gr->rhs[gr->symbol_count++] = get_symbol_index(ei->sym);
    }
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

void _fill_symbol_data(struct grule *rules, u16 rule_count, struct rule_symbol_data *symbol_data)
{
    u8 i, j;
    u16 r;
    for(i = 0; i < get_symbol_count(); i++){
        symbol_data[i].first_list.first = 0;
        symbol_data[i].follow_list.first = 0;
        symbol_data[i].rule_list.first = 0;
        symbol_data[i].is_nullable = false;
    }
    for(i = 0; i < PATTERN_COUNT; i++){
        //terminal token
        index_list_add_data_to_head(&symbol_data[i].first_list, i);
    }
    int symbol_changes;
    struct grule *rule;
    u8 first_nullable_to;
    u8 follow_nullable_to;
    bool is_i_nullable, is_j_nullable;
    /*fill nullable, first set and follow set for each grammar symbol*/
    while(true){
        symbol_changes = 0;
        for(r = 0; r < rule_count; r++){
            first_nullable_to = 0xff;
            rule = &rules[r];
            is_i_nullable = true;
            for(i = 0; i < rule->symbol_count; i++){
                follow_nullable_to = 0xff;
                is_j_nullable = true;
                if(is_i_nullable){
                    //copy rhs[i] first list to lhs first list
                    symbol_changes += _append_list(&symbol_data[rule->lhs].first_list, &symbol_data[rule->rhs[i]].first_list);
                }
                if(symbol_data[rule->rhs[i]].is_nullable && is_i_nullable){
                    first_nullable_to = i;
                }else{
                    is_i_nullable = false;
                }
                for(j = i + 1; j < rule->symbol_count; j++){
                    if(is_j_nullable){
                        //all i+1 to j-1 nullable
                        symbol_changes += _append_list(&symbol_data[rule->rhs[i]].follow_list, &symbol_data[rule->rhs[j]].follow_list);
                    }
                    if(symbol_data[rule->rhs[j]].is_nullable && is_j_nullable){
                        follow_nullable_to = j;
                    }else{
                        is_j_nullable = false;
                    }
                }
                if(follow_nullable_to == rule->symbol_count - 1){//all i+1 to symbol_count - 1 is nullable
                    symbol_changes += _append_list(&symbol_data[rule->rhs[i]].follow_list, &symbol_data[rule->lhs].follow_list);
                }
            }
            //if rhs is nullable, then lhs nonterm symbol nullable
            if (first_nullable_to == rule->symbol_count - 1 && !symbol_data[rule->lhs].is_nullable){
                symbol_data[rule->lhs].is_nullable = true;
                symbol_changes ++;
            }
        }
        if(!symbol_changes) break;
    }
    /*add rule index to each grammar symbol*/
    struct index_list *il;
    for(r = 0; r < rule_count; r++){
        rule = &rules[r];
        il = &symbol_data[rule->lhs].rule_list;
        index_list_add_data_to_head(il, r);
    }
}

u8 _eq_parse_item(struct parse_item *item1, struct parse_item *item2)
{
    return item1->rule == item2->rule && item1->lookahead == item2->lookahead && item1->dot == item2->dot;
}

bool _exists_parse_item(struct parse_item_list *items, struct parse_item *item)
{
    struct parse_item_list_entry *entry;
    list_foreach(entry, items, list){
        if(_eq_parse_item(&entry->data, item))
            return true;
    }
    return false;
}

u8 _add_parse_item(struct parse_item_list *items, struct parse_item item)
{
    if(_exists_parse_item(items, &item))
        return 0;
    parse_item_list_add_data_to_head(items, item);
    return 1;
}

struct parse_state _closure(struct rule_symbol_data *symbol_data, struct grule *rules, struct parse_state state)
{
    struct parse_item item;
    struct index_list_entry *rule_entry;
    struct index_list_entry *symbol_entry;
    while(true){
        int items_added = 0;
        struct parse_item_list_entry *entry;
        struct parse_item_list *items = &state.items;
        list_foreach(entry, items, list)
        {
            struct grule *rule = &rules[entry->data.rule];
            if(entry->data.dot < rule->symbol_count){
                u8 symbol_index = rule->rhs[entry->data.dot];
                if(!is_terminal(symbol_index)){//non terminal
                    u8 next_symbol = entry->data.dot < rule->symbol_count - 1 ? rule->rhs[entry->data.dot + 1] : 0xff;
                    struct index_list *nt_rules = &symbol_data[symbol_index].rule_list;
                    list_foreach(rule_entry, nt_rules, list){
                        item.rule = rule_entry->data; //ile->data stores the index of the rule
                        item.dot = 0;
                        if(next_symbol != 0xff){
                            list_foreach(symbol_entry, &symbol_data[next_symbol].first_list, list){
                                item.lookahead = symbol_entry->data;
                                items_added += _add_parse_item(items, item);
                            }
                        }
                        
                        if(next_symbol == 0xff || symbol_data[next_symbol].is_nullable){
                            item.lookahead = entry->data.lookahead; //copy current lookahead
                        }
                        items_added += _add_parse_item(items, item);
                    }
                }
            }
        }
        if(!items_added) break;
        state.item_count += items_added;
    }
    return state;
}

struct parse_state _goto(struct rule_symbol_data *symbol_data, struct grule *rules, struct parse_state state, u8 rule_symbol)
{
    struct parse_state next_state;
    next_state.items.first = 0;
    next_state.item_count = 0;
    struct parse_item_list *next_items = &next_state.items;

    struct parse_item_list_entry *entry;
    struct parse_item_list *items = &state.items;
    struct parse_item item;
    list_foreach(entry, items, list)
    {
        if(entry->data.dot < rules[entry->data.rule].symbol_count){
            item = entry->data;
            item.dot++;
            parse_item_list_add_data_to_head(next_items, item);
            next_state.item_count++;
        }
    }
    return _closure(symbol_data, rules, next_state);
}

bool _eq_state(struct parse_state *state1, struct parse_state *state2)
{
    if(state1->item_count != state2->item_count)
        return false;
    struct parse_item_list_entry *entry;
    list_foreach(entry, &state1->items, list){
        if(!entry->data.dot) continue; //only compare kernel
        if(_exists_parse_item(&state2->items, &entry->data))
            return true;
    }

    return false;
}

bool _exists_state(struct parse_state *states, u16 state_count, struct parse_state *state)
{
    for(u16 i = 0; i < state_count; i++){
        if(_eq_state(&states[i], state)) return true;
    }
    return false;
}

u16 _build_states(struct rule_symbol_data *symbol_data, struct grule *rules, u16 rule_count, struct parse_state *states)
{
    u16 i, state_count = 0;
    struct parse_item item;
    struct parse_state *state;
    states[state_count].items.first = 0;
    item.dot = 0;
    item.lookahead = TOKEN_EOF;
    item.rule = 0;
    parse_item_list_add_data_to_head(&states[state_count].items, item);
    _closure(symbol_data, rules, states[state_count]);
    state_count++;
    struct parse_item_list_entry *entry;
    struct grule *rule;
    for(i = 0; i < state_count; i++){
        state = &states[i];
        list_foreach(entry, &state->items, list){
            rule = &rules[entry->data.rule];
            for(u8 j = 0; j < rule->symbol_count; j++){
                struct parse_state next_state = _goto(symbol_data, rules, *state, rule->rhs[j]);
                //if not in the states, then add it to states
                if(!_exists_state(states, state_count, &next_state))
                    states[state_count++] = next_state;
            }
        }
    }
    return state_count;
}

struct lr_parser *lr_parser_new(const char *grammar_text)
{
    size_t i,j,k;
    struct lr_parser *parser;
    MALLOC(parser, sizeof(*parser));
    parser->stack_top = 0;
    //1. initialize parsing table
    for(i=0; i < MAX_STATES; i++){
        for(j=0; j < MAX_GRAMMAR_SYMBOLS; j++){
            parser->parsing_table[i][j].code = ACTION_ERROR;
            parser->parsing_table[i][j].state_index = 0;
        }
    }
    //2. add rules
    struct grammar *g = grammar_parse(grammar_text);
    struct rule *rule;
    u8 nonterm;
    for(i = 0; i < array_size(&g->rules); i++){
        rule = *(struct rule **)array_get(&g->rules, i);
        nonterm = (u8)i+PATTERN_COUNT;
        u8 index = add_grammar_nonterm(rule->nonterm);
        assert(nonterm == index);
    }

    //3. convert grammar symbol to use index:
    //all grammar symbol: non-terminal or terminal (token) 
    //has an integer of index 
    struct grule *gr;
    struct expr *rule_expr, *expr;
    parser->rule_count = 0;
    for(i = 0; i < (u16)array_size(&g->rules); i++){
        rule = *(struct rule **)array_get(&g->rules, i);
        nonterm = get_symbol_index(rule->nonterm);
        for(j = 0; j < array_size(&rule->exprs); j++){
            rule_expr = (struct expr *)array_get(&rule->exprs, j);
            struct array exprs;
            array_init_free(&exprs, sizeof(struct expr), (free_fun)expr_deinit);
            _expand_expr(rule_expr, &exprs);
            for(k = 0; k < array_size(&exprs); k++){
                expr = array_get(&exprs, k);
                gr = &parser->rules[parser->rule_count++];
                gr->lhs = nonterm;
                _expr_2_gr(expr, gr);
            }
            array_deinit(&exprs);
        }
    }
    _fill_symbol_data(parser->rules, parser->rule_count, parser->symbol_data);
    //4. build states
    parser->parse_state_count = _build_states(parser->symbol_data, parser->rules, parser->rule_count, parser->parse_states);
    //5. construct parsing table
    //action: state, terminal and goto: state, nonterm


    parser->g = g;
    return parser;
}

void lr_parser_free(struct lr_parser *parser)
{
    grammar_free(parser->g);
    FREE(parser);
}

struct ast_node *parse_text(struct lr_parser *parser, const char *text)
{
    //driver 
    return 0;
}