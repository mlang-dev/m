#include <stdio.h>
#include "parser/lalr_parser_generator.h"
#include "lexer/init.h"

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

#define source_parsing_rules_initializer "const struct parse_rule m_parsing_rules[PARSING_RULE_COUNT] = {\n"
#define source_parsing_table_initializer "const struct parser_action m_parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT] = {\n"
#define source_data_initializer_end  "};\n"

const char *parsing_actions[] = {
    "E",
    "S",
    "R",
    "A",
    "G"
};

const char *read_grammar(const char *grammar_path)
{
    char *buffer = 0;
    long length;
    FILE *f = fopen(grammar_path, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length+1);
        if (buffer) {
            fread(buffer, 1, length, f);
        }
        buffer[length] = '\0';
        fclose(f);
    }
    return buffer;
}

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

void print_rule(FILE *f, struct parse_rule *rule)
{
    // struct parse_rule {
    /*
    u16 lhs; // non terminal symbol index
    u16 rhs[MAX_SYMBOLS_RULE]; // right hand side of production rule
    u8 symbol_count; // right side of
    struct semantic_action action;
    */
    //{ 0, { 0, 0, 0, 0, 0, 0, 0 }, 0, { 0, { 0, 0, 0, 0, 0 }, 0 } }
    int i;
    fprintf(f, "{%d,{", rule->lhs); // lhs
    for (i = 0; i < MAX_SYMBOLS_RULE; i++) { // rhs
        fprintf(f, "%d,", rule->rhs[i]);
    }
    fprintf(f, "},");
    fprintf(f, "%d,", rule->symbol_count); // symbol count
    // rule action
    fprintf(f, "{%d,", rule->action.node_type);
    fprintf(f, "{"); //index array
    for(i=0; i< MAX_SYMBOLS_RULE; i++){
        fprintf(f, "%d,", rule->action.item_index[i]);
    }
    fprintf(f, "},"); //end of index array
    fprintf(f, "%d", rule->action.item_index_count); //total index
    fprintf(f, "}"); //end of action
    fprintf(f, "},"); //end of rule   
}


void print_parsing_table_row(FILE *f, struct parser_action *actions, u16 action_count)
{
    fprintf(f, "{");
    for (u16 i = 0; i < action_count; i++) {
        fprintf(f, "{%s,%d},", parsing_actions[actions[i].code], actions[i].state_index);
    }
    fprintf(f, "},");
}

int write_to_source_file(struct lalr_parser_generator *pg, const char *source_path)
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
    fprintf(f, source_parsing_rules_initializer);
    //print all rule data
    for(i = 0; i < pg->rule_count; i++){
        //    /*rule 0*/ { 0, { 0, 0, 0, 0, 0, 0, 0 }, 0, { 0, { 0, 0, 0, 0, 0 }, 0 } }
        fprintf(f, "  /*rule %d*/ ", i); // comments
        rule = &pg->parsing_rules[i];
        print_rule(f, rule);
        fprintf(f, "\n");
    }
    fprintf(f, source_data_initializer_end);
    fprintf(f, "\n");
    fprintf(f, source_parsing_table_initializer);
    //print parsing table
    for (i = 0; i < pg->parse_state_count; i++) {
        // print one state
        fprintf(f, "  /*state %d*/ ", i);
        //print row
        print_parsing_table_row(f, pg->parsing_table[i], get_symbol_count());
        fprintf(f, "\n");
    }
    fprintf(f, source_data_initializer_end);
    fclose(f);
    return 0;
}

int generate_files(const char *grammar_path, const char *header_path, const char *source_path)
{
    frontend_init();
    printf("parsing grammar file %s ...\n", grammar_path);
    const char *grammar = read_grammar(grammar_path);
    struct lalr_parser_generator *pg = lalr_parser_generator_new(grammar);
    printf("generating %s ...\n", header_path);
    write_to_header_file(pg, header_path);
    printf("generating %s ...\n", source_path);
    write_to_source_file(pg, source_path);
    lalr_parser_generator_free(pg);
    free((void*)grammar);
    frontend_deinit();
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc != 4){
        printf("pgen usage is: pgen grammar path path1(.h) path2(.c)\n");
    }
    printf("welcome to pgen!\n");
    return generate_files(argv[1], argv[2], argv[3]);
}
