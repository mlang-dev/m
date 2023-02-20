#include "test.h"
#include <stdio.h>

#ifdef WASM
#define ENTRY _start
#define EXPORT __attribute__((visibility("default")))
#else
#define ENTRY main
#define EXPORT
#endif

struct test_stats test_stats = { 0, 0 };

int test_array();
int test_byte_array();
int test_hashset();
int test_hashtable();
int test_math();
int test_queue();
int test_stack();
int test_string();
int test_symbol();
int test_symboltable();
int test_util();
int test_regex();

int test_token();
int test_lexer();
int test_lexer_error();
int test_m_lexer();
int test_ast();
int test_parser_expr();
int test_parser();
int test_parser_record();
int test_parser_tuple();
int test_parser_variant();
int test_parser_error();
int test_grammar();
int test_analyzer();
int test_analyzer_mut();
int test_analyzer_errors();
int test_analyzer_record();
int test_analyzer_variant();
int test_analyzer_pm();
int test_type_size_info();
int test_wasm_codegen();

void setUp(){}
void tearDown(){}


EXPORT int ENTRY()
{
  int failures = 0;
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
  failures += test_parser_record();
  failures += test_parser_tuple();
  failures += test_parser_variant();
  failures += test_parser_error();
  failures += test_grammar();
  failures += test_analyzer();
  failures += test_analyzer_record();
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
  return failures;
}
