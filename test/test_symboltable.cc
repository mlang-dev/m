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
    symbols symbols;
    symboltable st;
    hashtable_init(&symbols);
    symboltable_init(&st);
    symbol symbol = to_symbol(&symbols, "hello");
    type_oper* op1 = create_nullary_type(TYPE_INT);
    symboltable_add(&st, symbol, op1);
    ASSERT_EQ(op1, symboltable_get(&st, symbol));
    type_oper* op2 = create_nullary_type(TYPE_DOUBLE);
    symboltable_add(&st, symbol, op2);
    ASSERT_EQ(op2, symboltable_get(&st, symbol));
    symboltable_deinit(&st);
    hashtable_deinit(&symbols);
    type_exp_free((type_exp*)op1);
    type_exp_free((type_exp*)op2);
}
