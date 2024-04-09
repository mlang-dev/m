#include "test.h"
#include <stdio.h>
#include "app/app.h"

#ifdef WASM
#define ENTRY _start
#define EXPORT __attribute__((visibility("default")))
#else
#define ENTRY main
#define EXPORT
#endif

struct test_stats test_stats = { 0, 0 };

int test_array(void);
int test_byte_array(void);
int test_hashset(void);
int test_hashtable(void);
int test_math(void);
int test_queue(void);
int test_stack(void);
int test_string(void);
int test_symbol(void);
int test_symboltable(void);
int test_util(void);
int test_regex(void);

int test_token(void);
int test_lexer(void);
int test_lexer_error(void);
int test_m_lexer(void);
int test_ast(void);
int test_parser_expr(void);
int test_parser(void);
int test_parser_type(void);
int test_parser_struct(void);
int test_parser_variant(void);
int test_parser_error(void);
int test_grammar(void);
int test_analyzer(void);
int test_analyzer_mut(void);
int test_analyzer_errors(void);
int test_analyzer_struct(void);
int test_analyzer_type(void);
int test_analyzer_variant(void);
int test_analyzer_pm(void);
int test_type_size_info(void);
int test_wasm_codegen(void);

void setUp(void){}
void tearDown(void){}


EXPORT int ENTRY(void)
{
  int failures = 0;
  app_init();
  failures += test_array();
  failures += test_byte_array();
  failures += test_hashset();
  failures += test_hashtable();
  failures += test_math();
  failures += test_queue();
  failures += test_stack();
  failures += test_string();
  failures += test_symbol();
  failures += test_symboltable();
  failures += test_util();
  failures += test_regex();

  failures += test_token();
  failures += test_lexer();
  failures += test_lexer_error();
  failures += test_m_lexer();
  failures += test_ast();
  failures += test_parser_expr();
  failures += test_parser();
  failures += test_parser_type();
  failures += test_parser_struct();
  failures += test_parser_variant();
  failures += test_parser_error();
  failures += test_grammar();
  failures += test_analyzer();
  failures += test_analyzer_struct();
  failures += test_analyzer_type();
  failures += test_analyzer_variant();
  failures += test_analyzer_pm();
  failures += test_analyzer_mut();
  failures += test_analyzer_errors();
  failures += test_type_size_info();
  failures += test_wasm_codegen();
  if (!failures)
    printf("%d/%d Unit tests passed !\n", test_stats.total_tests - test_stats.total_failures, test_stats.total_tests);
  else
    printf("Unit tests failures: %d\n", failures);
    
  app_deinit();
  return failures;
}
