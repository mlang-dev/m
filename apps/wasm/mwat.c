#include "app/mwat.h"
#include "clib/symbol.h"
#include "clib/string.h"
#include "codegen/wasm_codegen.h"
#include "parser/m_grammar.h"
#include "parser/parser.h"

const char * wat_module_start = "(module ";
const char * wat_module_end = ")\n";
const char * fun_def = "run()=";
const char *parse_exp_as_module(const char *expr)
{
    symbols_init();
    wasm_codegen_init();
    string m_code;
    string_init_chars2(&m_code, fun_def, strlen(fun_def));
    string_add_chars2(&m_code, expr, strlen(expr));
    struct parser *parser = parser_new(get_m_grammar());
    char *m_text = to_c_str(&m_code);
    struct ast_node *ast = parse(parser, m_text);
    string wat_fun_code = generate(ast, m_text);
    string wat_mod_code;
    string_init_chars2(&wat_mod_code, wat_module_start, strlen(wat_module_start));
    string_add2(&wat_mod_code, &wat_fun_code);
    string_add_chars2(&wat_mod_code, wat_module_end, strlen(wat_module_end));
    symbols_deinit();
    return 0;
}