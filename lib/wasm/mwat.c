#include "wasm/mwat.h"
#include "clib/symbol.h"
#include "clib/string.h"
#include "codegen/wat_codegen.h"
#include "parser/m_grammar.h"
#include "parser/earley_parser.h"
#include "parser/amodule.h"

const char * fun_def = "run()=";

const char *parse_exp_as_module(const char *expr)
{
    symbols_init();
    wat_codegen_init();
    string m_code;
    string_init_chars2(&m_code, fun_def, strlen(fun_def));
    string_add_chars2(&m_code, expr, strlen(expr));
    struct earley_parser *parser = earley_parser_new(get_m_grammar());
    const char *m_text = string_get(&m_code);
    struct ast_node *ast = parse(parser, m_text);
    struct amodule mod;
    mod.root_ast = ast;
    mod.code = m_text;
    string wat_mod_code = wat_emit(mod);
    const char *data = string_cstr(&wat_mod_code);
    string_deinit(&wat_mod_code);
    string_deinit(&m_code);
    symbols_deinit();
    return data;
}
