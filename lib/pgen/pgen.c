#include <stdio.h>
#include "pgen/lalr_parser_generator.h"
//#include "parser/m_parsing_table.h"
#include <assert.h>
#include "clib/symbol.h"
#include "lexer/pgen_token.h"
#include "parser/node_type.h"
#include "sema/type.h"
#include "clib/util.h"
#include "app/error.h"
#include "app/app.h"


#define header_comment_template  "/*\n"\
                              " * parsing table for parser\n"\
                              " * DO NOT edit this file, this file is generated by lalr parser generator\n"\
                              " * \n"\
                              " * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>\n"\
                              " *     \n"\
                              " */    \n"

#define header_file_template    "#define PARSING_RULE_COUNT %d\n"\
                                "#define PARSING_STATE_COUNT %d\n"\
                                "#define PARSING_SYMBOL_COUNT %d\n"

#define source_header_template   "#define M_PARSING_INITIALIZER 1\n"\
                                 "#include \"parser/m_parsing_table.h\"\n\n"

#define source_parsing_symbols_initializer "const char *m_parsing_symbols[PARSING_SYMBOL_COUNT] = {\n"
#define source_parsing_rules_initializer "const struct parse_rule m_parsing_rules[PARSING_RULE_COUNT] = {\n"
#define source_parsing_states_initializer "struct parse_state_items m_parsing_states[PARSING_STATE_COUNT] = {\n"
#define source_parsing_table_initializer "const struct parser_action m_parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT] = {\n"
#define source_data_initializer_end  "};\n"

const char *parsing_actions[] = {
    "E",
    "S",
    "R",
    "A",
    "G"
};

int write_to_header_file(struct lalr_parser_generator *pg, const char *header_path)
{
    FILE *f = fopen(header_path, "w");
    if(f == NULL){
        printf("Error opening header file: %s\n", header_path);
        exit(1);
    }
    fprintf(f, header_comment_template);
    fprintf(f, header_file_template, pg->rule_count, pg->parse_state_count, get_symbol_count());
    fclose(f);
    return 0;
}

string to_rule_string(struct parse_rule *rule, int dot)
{
    string str;
    string_init(&str);
    string_add(&str, get_symbol_by_index(rule->lhs));
    string_add_chars(&str, " = ");
    for (int i = 0; i < rule->symbol_count; i++) { // rhs
        if (dot == i){
            string_add_chars(&str, ".");
        }
        string_add(&str, get_symbol_by_index(rule->rhs[i]));
        if(i < rule->symbol_count - 1){
            string_add_chars(&str, " ");
        }
    }
    if(dot == rule->symbol_count){
        string_add_chars(&str, ".");
    }
    return str;
}

void print_rule(FILE * f, struct parse_rule * rule)
{
    // struct parse_rule {
    /*
    const char *rule_grammar;
    u16 lhs; // non terminal symbol index
    u16 rhs[MAX_SYMBOLS_RULE]; // right hand side of production rule
    u8 symbol_count; // right side of
    struct semantic_action action;
    */
    //{ 0, { 0, 0, 0, 0, 0, 0, 0 }, 0, { 0, { 0, 0, 0, 0, 0 }, 0 } }
    int i;
    string rule_string = to_rule_string(rule, -1);
    fprintf(f, "{\"%s\", %d,{", string_get(&rule_string), rule->lhs); // lhs
    string_deinit(&rule_string);
    for (i = 0; i < MAX_SYMBOLS_RULE; i++) { // rhs
        fprintf(f, "%d,", rule->rhs[i]);
    }
    fprintf(f, "},");
    fprintf(f, "%d,", rule->symbol_count); // symbol count
    // rule action
    fprintf(f, "{%d,", rule->action.node_type);
    fprintf(f, "{"); // index array
    for (i = 0; i < MAX_SYMBOLS_RULE; i++) {
        fprintf(f, "%d,", rule->action.item_index[i]);
    }
    fprintf(f, "},"); // end of index array
    fprintf(f, "%d", rule->action.item_index_count); // total index
    fprintf(f, "}"); // end of action
    fprintf(f, "},"); // end of rule
}

void print_state_string(FILE *f, struct parse_rule *rules, struct parse_state *state)
{
    struct parse_item_list_entry *entry;
    struct parse_item *item;
    //struct index_list_entry *la_entry;
    u8 i = 0;
    fprintf(f, "{%d, {", state->kernel_item_count);
    list_foreach(entry, &state->items)
    {
        assert(state->kernel_item_count < MAX_KERNEL_ITEMS);
        if (i >= state->kernel_item_count)
            break;
        item = &entry->data;
        string rule_str = to_rule_string(&rules[item->rule], item->dot);
        fprintf(f, "{%d, %d, \"%s\"}, ", item->rule, item->dot, string_get(&rule_str));
        string_deinit(&rule_str);
        i++;
    }
    fprintf(f, "}},\n");
}

void print_parsing_table_row(FILE * f, struct parser_action * actions, u16 action_count)
{
    fprintf(f, "{");
    for (u16 i = 0; i < action_count; i++) {
        fprintf(f, "{%s,%d},", parsing_actions[actions[i].code], actions[i].state_index);
    }
    fprintf(f, "},");
}

int write_to_source_file(struct lalr_parser_generator * pg, const char *source_path)
{
    u16 i;
    struct parse_rule *rule;
    FILE *f = fopen(source_path, "w");
    if (f == NULL) {
        printf("Error opening source file: %s\n", source_path);
        exit(1);
    }
    fprintf(f, header_comment_template);
    fprintf(f, source_header_template);
    fprintf(f, source_parsing_symbols_initializer);
    symbol sym;
    for (i = 0; i < get_symbol_count(); i++) {
        sym = get_symbol_by_index(i);
        fprintf(f, "  /*symbol %3d*/ ", i); // comments
        fprintf(f, "\"%s\",\n", string_cstr(sym));
    }
    fprintf(f, source_data_initializer_end);
    fprintf(f, "\n");
    fprintf(f, source_parsing_rules_initializer);
    // print all rule data
    fprintf(f, "/* all rules */\n");
    for (i = 0; i < pg->rule_count; i++) {
        //    /*rule 0*/ { 0, { 0, 0, 0, 0, 0, 0, 0 }, 0, { 0, { 0, 0, 0, 0, 0 }, 0 } }
        fprintf(f, "  /*rule %3d*/ ", i); // comments
        rule = &pg->parse_rules[i];
        print_rule(f, rule);
        fprintf(f, "\n");
    }
    fprintf(f, source_data_initializer_end);
    fprintf(f, "\n");

    fprintf(f, source_parsing_states_initializer);
    for (i = 0; i < pg->parse_state_count; i++) {
        fprintf(f, "  /*state %3d*/ ", i); // comments
        print_state_string(f, pg->parse_rules, &pg->parse_states[i]);
    }
    fprintf(f, source_data_initializer_end);
    fprintf(f, "\n");

    fprintf(f, source_parsing_table_initializer);
    for (i = 0; i < pg->parse_state_count; i++) {
        // print one state
        fprintf(f, "  /*state %3d*/ ", i);
        // print row
        print_parsing_table_row(f, pg->parsing_table[i], get_symbol_count());
        fprintf(f, "\n");
    }
    fprintf(f, source_data_initializer_end);
    fclose(f);
    return 0;
}

void pgen_init()
{
    app_init();
    node_type_init();
    pgen_token_init();
}

void pgen_deinit()
{
    pgen_token_deinit();
    node_type_deinit();
    app_deinit();
}

int generate_files(const char *grammar_path, const char *header_path, const char *source_path)
{
    pgen_init();
    printf("parsing grammar file %s ...\n", grammar_path);
    const char *grammar = read_text_file(grammar_path);
    struct lalr_parser_generator *pg = lalr_parser_generator_new(grammar);
    printf("generating %s ...\n", header_path);
    write_to_header_file(pg, header_path);
    printf("generating %s ...\n", source_path);
    write_to_source_file(pg, source_path);
    lalr_parser_generator_free(pg);
    free((void *)grammar);
    pgen_deinit();
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("pgen usage is: pgen grammar path path1(.h) path2(.c)\n");
    }
    printf("welcome to pgen!\n");
    return generate_files(argv[1], argv[2], argv[3]);
}
