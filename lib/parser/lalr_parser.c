/*
 * lalr_parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement a LALR parser, taking a EBNF grammar text and parse text 
 * into ast for the grammar
 */
#include "parser/lalr_parser.h"
#include "clib/stack.h"
#include "clib/util.h"
#include "parser/grammar.h"
#include <assert.h>

