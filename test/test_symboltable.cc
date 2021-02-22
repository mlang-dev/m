/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "gtest/gtest.h"

#include <string.h>

#include "clib/generic.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/util.h"
#include "type.h"
#include "clib/symbol.h"
#include "clib/symboltable.h"


TEST(testSymboltable, TestSymboltableSameKeyMultipleValues)
{
    symbols_init();
    symboltable st;
    symboltable_init(&st);
    symbol symbol = to_symbol("hello");
    type_oper* op1 = create_nullary_type(TYPE_INT);
    symboltable_push(&st, symbol, op1);
    ASSERT_EQ(op1, symboltable_get(&st, symbol));
    type_oper* op2 = create_nullary_type(TYPE_DOUBLE);
    symboltable_push(&st, symbol, op2);
    ASSERT_EQ(op2, symboltable_get(&st, symbol));
    symboltable_pop(&st, symbol);
    ASSERT_EQ(op1, symboltable_get(&st, symbol));
    symboltable_pop(&st, symbol);
    ASSERT_EQ(NULL, symboltable_get(&st, symbol));

    //over pop, still fine
    symboltable_pop(&st, symbol);
    ASSERT_EQ(NULL, symboltable_get(&st, symbol));
    symboltable_deinit(&st);
    type_exp_free((type_exp*)op1);
    type_exp_free((type_exp*)op2);
    symbols_deinit();
}
