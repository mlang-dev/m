#include "codegen/wasm_codegen.h"
#include <stdio.h>

#ifdef WASM
#define ENTRY _start
#define EXPORT __attribute__((visibility("default")))
#else
#define ENTRY main
#define EXPORT
#endif

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
int test_m_lexer();
int test_ast();
int test_lr_parser_expr();
int test_lr_parser();
int test_grammar();
int test_wat_codegen();
int test_wasm_codegen();
int test_mwat();

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
  failures += test_m_lexer();
  failures += test_ast();
  failures += test_lr_parser_expr();
  failures += test_lr_parser();
  failures += test_grammar();
  failures += test_wat_codegen();
  #ifndef WASM
  failures += test_wasm_codegen();
  #endif
  failures += test_mwat();
  
  if (!failures)
    printf("Unit tests passed !\n");
  else
    printf("Unit tests failures: %d\n", failures);
  return failures;
}
