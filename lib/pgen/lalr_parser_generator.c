/*
 * lalr_parser_generator.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement an LALR parser generator, taking a EBNF grammar text and generate a parsing table
 * for parser to consume
 */
#include "lexer/pgen_token.h"
#include "pgen/lalr_parser_generator.h"
#include "clib/stack.h"
#include "clib/util.h"
#include "pgen/grammar.h"
#include "parser/node_type.h"
#include <assert.h>

#define END_OF_RULE 0xff

link_list_append_data_fn(index_list, index_list_entry, u16)
link_list_append_data_fn(parse_item_list, parse_item_list_entry, struct parse_item)

bool _exists(struct index_list *dst, u16 data)
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

int _append_data(struct index_list *dst, u16 data)
{
    int n = 0;
    if (!_exists(dst, data)) {
        index_list_append_data(dst, data);
        n++;
    }
    return n;
}

void _semantic_action_2_rule_action(struct semantic_action *sa, struct rule_action *ra)
{
    ra->node_type = (u8)symbol_to_node_type(sa->action);
    for(u8 i = 0; i < sa->exp_item_index_count; i++){
        ra->item_index[i] = sa->exp_item_index[i];
    }
    ra->item_index_count = sa->exp_item_index_count;
}

/*expand bracket in grammar*/
void _expr_2_gr(symbol nonterm, struct expr *expr, struct parse_rule *pr)
{
    struct expr_item *ei;
    pr->symbol_count = 0;
    for(size_t i=0; i < array_size(&expr->items); i++){
        if(i >= MAX_SYMBOLS_RULE){
            printf("warning: too many symbols in a rule: %zu for %s, only %d symbols are allowed.\n", array_size(&expr->items), string_get(nonterm), MAX_SYMBOLS_RULE);
            exit(1);
        }
        ei = array_get(&expr->items, i);
        pr->rhs[pr->symbol_count++] = get_symbol_index(ei->sym);
    }
    _semantic_action_2_rule_action(&expr->action, &pr->action);
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
            ei->sym = array_get_ptr(&symbols, i);
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

void _init_parse_item(struct parse_item *item, u8 rule, u8 dot)
{
    item->rule = rule;
    item->dot = dot;
    _init_index_list(&item->lookaheads);
}

bool _is_nullable(u16 *symbols, u8 symbol_count, struct rule_symbol_data *symbol_data)
{
    //all symbol are nullable
    for(u8 i = 0; i < symbol_count; i ++){
        if(!symbol_data[symbols[i]].is_nullable) {
            return false;
        }
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
            if ((rule->symbol_count == 1 && rule->rhs[0] == TOKEN_EPSILON) || 
                (!symbol_data[rule->lhs].is_nullable && _is_nullable(rule->rhs, rule->symbol_count, symbol_data))) {
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
    u16 i, j, symbol_index;
    struct rule_symbol_data *sd;
    //put $ EOF into the start symbol's follow set
    index_list_append_data(&symbol_data[rules[0].lhs].follow_list, TOKEN_EOF);
    do {
        change_count = 0;
        for (i = 0; i < rule_count; i++) {
            rule = &rules[i];
            for (j = 0; j < rule->symbol_count; j++) {
                symbol_index = rule->rhs[j];
                if(is_terminal(symbol_index)) continue;
                sd = &symbol_data[symbol_index];
                change_count += _append_first_set_to(&sd->follow_list, rule, j + 1, symbol_data);
                //
                if(j == rule->symbol_count - 1 || _is_nullable(&rule->rhs[j+1], rule->symbol_count -1 - j, symbol_data)){
                    change_count += _append_list(&sd->follow_list, &symbol_data[rule->lhs].follow_list);
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

bool _eq_parse_item(struct parse_item *item1, struct parse_item *item2)
{
    return item1->rule == item2->rule && item1->dot == item2->dot;
}

struct parse_item *_find_parse_item(struct parse_item_list *items, struct parse_item *item)
{
    struct parse_item_list_entry *entry;
    list_foreach(entry, items){
        if(_eq_parse_item(&entry->data, item))
            return &entry->data;
    }
    return 0;
}

u8 _add_parse_item(struct parse_item_list *items, struct parse_item item)
{
    if (_find_parse_item(items, &item))
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
            u16 symbol_index = rule->rhs[entry->data.dot];
            if(!is_terminal(symbol_index)){//non terminal
                struct index_list *nt_rules = &symbol_data[symbol_index].rule_list;
                list_foreach(rule_entry, nt_rules){
                    _init_parse_item(&item, rule_entry->data, 0);
                    items_added += _add_parse_item(items, item);
                }
            }
        }
    }
    printf("items added: %d\n", items_added);
    return state;
}

struct parse_state _goto(struct rule_symbol_data *symbol_data, struct parse_rule *rules, struct parse_state state, u16 rule_symbol)
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
            _init_index_list(&item.lookaheads);
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

bool _exists_in_array(u16 *array, u8 size, u16 match)
{
    for (u8 i = 0; i < size; i++){
        if (array[i] == match)
            return true;
    }
    return false;
}

u16 _build_states(struct rule_symbol_data *symbol_data, struct parse_rule *rules, u16 rule_count, struct parse_state *states, struct parser_action (*parsing_table)[MAX_STATES][MAX_GRAMMAR_SYMBOLS])
{
    u16 i, state_count = 0;
    struct parse_item item;
    struct parse_state *state;
    _init_parse_item(&item, 0, 0);
    _init_parse_item_list(&states[state_count].items);
    parse_item_list_append_data(&states[state_count].items, item);
    states[state_count].kernel_item_count = 1;
    states[state_count] = _closure(symbol_data, rules, states[state_count]);
    state_count++;
    struct parse_item_list_entry *entry;
    struct parse_rule *rule;
    // iterate each rule to get unique symbol to create new state
    #define UNIQUE_SYMBOLS  128
    u16 visited_symbols[UNIQUE_SYMBOLS];
    for (i = 0; i < state_count; i++) {
        state = &states[i];

        u8 visited_count = 0;
        list_foreach(entry, &state->items){
            rule = &rules[entry->data.rule];
            if(entry->data.dot >= rule->symbol_count) {
                continue;
            }
            u16 x = rule->rhs[entry->data.dot]; 
            if(_exists_in_array(visited_symbols, visited_count, x)){
                continue;
            }
            assert(visited_count < UNIQUE_SYMBOLS);
            visited_symbols[visited_count++] = x;
            struct parse_state next_state = _goto(symbol_data, rules, *state, x);
            int existing_state_index = _find_state(states, state_count, &next_state);
                // if not in the states, then closure the state and add it to states
            struct parser_action pa;
            pa.code = is_terminal(x) ? S : G;
            if (existing_state_index < 0) {
                pa.state_index = state_count;
                assert(state_count < MAX_STATES);
                states[state_count++] = _closure(symbol_data, rules, next_state);
            } else {
                pa.state_index = existing_state_index;
            }
            (*parsing_table)[i][x] = pa;
        }
    }
    return state_count;
}

void _convert_grammar_rules_to_parse_rules(struct grammar *g, struct lalr_parser_generator *pg)
{
    struct parse_rule *gr;
    struct expr *rule_expr, *expr;
    struct rule *rule;
    u16 nonterm;
    size_t i, j, k;
    pg->rule_count = 0;
    for (i = 0; i < (u16)array_size(&g->rules); i++) {
        rule = array_get_ptr(&g->rules, i);
        nonterm = get_symbol_index(rule->nonterm);
        for (j = 0; j < array_size(&rule->exprs); j++) {
            rule_expr = array_get(&rule->exprs, j);
            struct array exprs;
            array_init_free(&exprs, sizeof(struct expr), (free_fun)expr_deinit);
            _expand_expr(rule_expr, &exprs);
            for (k = 0; k < array_size(&exprs); k++) {
                expr = array_get(&exprs, k);
                assert(pg->rule_count < MAX_RULES);
                gr = &pg->parse_rules[pg->rule_count++];
                gr->lhs = nonterm;
                _expr_2_gr(rule->nonterm, expr, gr);
            }
            array_deinit(&exprs);
        }
    }
    printf("rules count: %d\n", pg->rule_count);
}

void _complete_parsing_table(struct rule_symbol_data *symbol_data, struct parser_action (*parsing_table)[MAX_STATES][MAX_GRAMMAR_SYMBOLS], u16 state_count, struct parse_state *states, struct parse_rule *rules)
{
    struct parse_state *state;
    struct parser_action *action;
    struct parse_item_list_entry *entry;
    struct parse_item *item;
    struct parse_rule *rule;
    struct index_list *lookahead_list;
    struct index_list_entry *la_entry;
    int shift_reduce_conflicts = 0;
    int reduce_reduce_conflicts = 0;
    for(u16 i=0; i < state_count; i++){
        state = &states[i];
        list_foreach(entry, &state->items){
            item = &entry->data;
            rule = &rules[item->rule];
            if(item->dot == rule->symbol_count && item->rule > 0){/*except the augumented one*/
                /*SLR: we do reduction here. get follow set of the rule's nonterm symbol, for each
                symbol in follow set we do reduction
                lookahead_list = &symbol_data[rules[item->rule].lhs].follow_list;
                */
                /*LALR: we do reduction here. get lookahead set of the rule's nonterm symbol, for each
                symbol in lookahead set we do reduction*/
                lookahead_list = &item->lookaheads;
                list_foreach(la_entry, lookahead_list)
                {
                    action = &(*parsing_table)[i][la_entry->data];
                    /**/
                    if (action->code == S){
                        printf("warning: There is a shift/reduce conflict in the grammar. ");
                        printf("state: %d terminal: %s, shift to: %d, overrided reduction rule: %d(%s) \n", i, string_get(get_symbol_by_index(la_entry->data)), action->state_index, item->rule, string_get(get_symbol_by_index(rules[item->rule].lhs)));
                        shift_reduce_conflicts++;
                    } else if (action->code == R){
                        printf("warning: There is a reduce/reduce conflict in the grammar. ");
                        printf("state: %d terminal: %s, reduction rule: %d(%s), new reduction rule: %d(%s), taken rule: %d \n", i, string_get(get_symbol_by_index(la_entry->data)), action->rule_index, string_get(get_symbol_by_index(rules[action->rule_index].lhs)), item->rule, string_get(get_symbol_by_index(rules[item->rule].lhs)), action->rule_index < item->rule ? action->rule_index : item->rule);
                        if(item->rule < action->rule_index){
                            action->rule_index = item->rule;
                        }
                        reduce_reduce_conflicts ++;
                    }else{
                        action->code = R;
                        action->rule_index = item->rule;
                    }
                }
            }
            else if(item->dot == rule->symbol_count && item->rule == 0){/*the augumented one*/
                action = &(*parsing_table)[i][TOKEN_EOF];
                action->code = A;                
            }
        }
    }
    if (shift_reduce_conflicts) {
        printf("warning: total shift/reduce conflicts: %d\n", shift_reduce_conflicts);
    }
    if (reduce_reduce_conflicts) {
        printf("warning: total reduce/reduce conflicts: %d\n", reduce_reduce_conflicts);
    }
}

u16 _get_augmented_symbol_index(struct lalr_parser_generator *pg, u16 symbol_index, u16 from_state, u16 to_state)
{
    u64 key = (u64)symbol_index << 32 | (u64)from_state << 16 | to_state;
    u16 *old_index = (u16 *)hashtable_get_v(&pg->augmented_symbol_map, &key);
    if (old_index){
        return *old_index;
    }
    u16 index = pg->total_symbol_count ++;
    assert(pg->total_symbol_count <= MAX_GRAMMAR_SYMBOLS);
    hashtable_set_v(&pg->augmented_symbol_map, &key, &index);
    return index;
}

void _compute_augmented_rule(struct lalr_parser_generator *pg)
{
    //go through all SLR states
    struct parse_state *state;
    struct parse_item_list_entry *entry;
    struct parse_item *item;
    struct parse_rule *rules = pg->parse_rules;
    struct parse_rule *rule;
    struct parser_action *pa;
    struct parse_rule *new_rule;
    struct parser_action (*parsing_table)[MAX_STATES][MAX_GRAMMAR_SYMBOLS] = (struct parser_action (*)[MAX_STATES][MAX_GRAMMAR_SYMBOLS])pg->parsing_table;
    u16 state_index;
    pg->augmented_rule_count = 0;
    int x = 0;
    for (u16 i = 0; i < pg->parse_state_count; i++)
    {
        state = &pg->parse_states[i];
        //for each parse item
        x = -1;
        list_foreach(entry, &state->items){
            x++;
            state_index = i;
            item = &entry->data;
            if (item->dot > 0)
                continue;
            rule = &rules[item->rule];
            // for any item with A->.w
            assert(pg->augmented_rule_count < MAX_AUGMENTED_RULES);
            pg->augmented_rules[pg->augmented_rule_count++] = *rule;
            new_rule = &pg->augmented_rules[pg->augmented_rule_count-1];
            for (u8 j = 0; j < rule->symbol_count; j++){
                u16 symbol = rule->rhs[j];
                pa = &(*parsing_table)[state_index][symbol];
                if (is_terminal(symbol)) {
                    assert(pa->code == S);
                } else {
                    assert(pa->code == G);
                    //annotate new nonterminal symbol: (symbol, state_index, pa->state_index)
                    new_rule->rhs[j] = _get_augmented_symbol_index(pg, symbol, state_index, pa->state_index);
                }
                state_index = pa->state_index;
            }
            if(item->rule > 0){
                pa = &(*parsing_table)[i][rule->lhs];
                assert(pa->code == G);
                //annotate new left hand's nonterminal: (rule->lhs, i, pa->state_index)
                new_rule->lhs = _get_augmented_symbol_index(pg, rule->lhs, i, pa->state_index);
            }else{
                printf("skipped augmenting the first rule.\n");
            }
        }
    }
    printf("augmented rules count: %d\n", pg->augmented_rule_count);
}

void print_followlist(u16 symbol_index, u16 from_state, u16 to_state, struct index_list *src)
{
    printf("symbol: %d, from state: %d, to_state: %d: ", symbol_index, from_state, to_state);
    struct index_list_entry *entry;
    list_foreach(entry, src)
    {
        printf("%s,", string_get(get_symbol_by_index(entry->data)));
    }
    printf("\n");
}

void _propagate_lookahead(struct lalr_parser_generator *pg)
{
    // go through all SLR states
    struct parse_state *state;
    struct parse_item_list_entry *entry;
    struct parse_item *item;
    struct parse_rule *rules = pg->parse_rules;
    struct parse_rule *rule;
    struct parser_action *pa;
    struct parse_item complete_item;
    struct parser_action (*parsing_table)[MAX_STATES][MAX_GRAMMAR_SYMBOLS] = (struct parser_action (*)[MAX_STATES][MAX_GRAMMAR_SYMBOLS])pg->parsing_table;
    u16 lhs_nonterm;
    u16 state_index;
    for (u16 i = 0; i < pg->parse_state_count; i++) {
        state = &pg->parse_states[i];
        // for each parse item, lhs->x.yz
        list_foreach(entry, &state->items)
        {
            state_index = i;
            item = &entry->data;
            if (item->dot > 0)
                continue;
            //here only handles the closured (non-kernel item): lhs -> .xyz
            rule = &rules[item->rule];
            complete_item = *item;
            complete_item.dot = rule->symbol_count;
            // for any item with A->.w
            // get the final state for this parsing item
            for (u8 j = 0; j < rule->symbol_count; j++){
                pa = &(*parsing_table)[state_index][rule->rhs[j]];
                assert(pa->code == S || pa->code == G);
                state_index = pa->state_index;
            }
            if(item->rule > 0){
                pa = &(*parsing_table)[i][rule->lhs];
                assert(pa->code == G);
                //augmenting current nonterm @ left as current state and next state on the nonterm
                lhs_nonterm = _get_augmented_symbol_index(pg, rule->lhs, i, pa->state_index);
            }else{
                lhs_nonterm = rule->lhs;
            }
            //state_index is the final state of the parsing rule
            item = _find_parse_item(&pg->parse_states[state_index].items, &complete_item);
            assert(item);
            _append_list(&item->lookaheads, &pg->symbol_data[lhs_nonterm].follow_list);
            /*debug*/
            // if(state_index == 34 && rule->lhs == 73 && item->rule == 54){
            //     //final states
            //     print_followlist(rule->lhs, i, pa->state_index, &pg->symbol_data[lhs_nonterm].follow_list);
            // }
        }
    }
}



struct lalr_parser_generator *lalr_parser_generator_new(const char *grammar_text)
{
    size_t i,j;
    struct lalr_parser_generator *pg;
    MALLOC(pg, sizeof(*pg));
    CALLOC(pg->symbol_data, MAX_GRAMMAR_SYMBOLS, sizeof(*pg->symbol_data));
    CALLOC(pg->parsing_table, MAX_STATES * (MAX_GRAMMAR_SYMBOLS), sizeof(*pg->parsing_table));
    //1. initialize parsing table and symbol data
    //row: state index, col: symbol index
    struct parser_action (*parsing_table)[MAX_STATES][MAX_GRAMMAR_SYMBOLS] = (struct parser_action (*)[MAX_STATES][MAX_GRAMMAR_SYMBOLS])pg->parsing_table;
    for(i=0; i < MAX_STATES; i++){
        for(j=0; j < MAX_GRAMMAR_SYMBOLS; j++){
            (*parsing_table)[i][j].code = E;
            (*parsing_table)[i][j].state_index = 0;
        }
    }
    for (i = 0; i < get_symbol_count(); i++) {
        _init_index_list(&pg->symbol_data[i].first_list);
        _init_index_list(&pg->symbol_data[i].follow_list);
        _init_index_list(&pg->symbol_data[i].rule_list);
        pg->symbol_data[i].is_nullable = false;
    }
    hashtable_init_with_size(&pg->augmented_symbol_map, sizeof(u64), sizeof(u16));

    //2. registering non-term symbols with integer
    struct grammar *g = grammar_parse(grammar_text);
    pg->g = g;
    struct rule *rule;
    for(i = 0; i < array_size(&g->rules); i++){
        rule = array_get_ptr(&g->rules, i);
        u16 index = register_grammar_nonterm(rule->nonterm); //register new non-term symbol
        assert(i + TERMINAL_COUNT == index);
    }
    pg->total_symbol_count = get_symbol_count();
    //3. convert grammar to replace symbol with index:
    //all grammar symbol: non-terminal or terminal (token) 
    //has an integer of index representing itself
    _convert_grammar_rules_to_parse_rules(g, pg);

    //4. calculate production rule's nonterm's first set, follow set
    _fill_rule_symbol_data(pg->parse_rules, pg->rule_count, pg->symbol_data);

    //5. build states
    pg->parse_state_count = _build_states(pg->symbol_data, pg->parse_rules, pg->rule_count, pg->parse_states, parsing_table);

    // 6. compute augmented grammar rules
    _compute_augmented_rule(pg);

    // 7. compute augmented symbol's follow set
    _fill_rule_symbol_data(pg->augmented_rules, pg->augmented_rule_count, pg->symbol_data);

    // 8. propagate the lookahead set
    _propagate_lookahead(pg);
    // 9. construct parsing table
    //  action: state, terminal and goto: state, nonterm
    _complete_parsing_table(pg->symbol_data, parsing_table, pg->parse_state_count, pg->parse_states, pg->parse_rules);

    return pg;
}

void lalr_parser_generator_free(struct lalr_parser_generator *pg)
{
    grammar_free(pg->g);
    hashtable_deinit(&pg->augmented_symbol_map);
    FREE(pg->symbol_data);
    FREE(pg->parsing_table);
    FREE(pg);
}
