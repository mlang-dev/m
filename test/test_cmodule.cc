/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for cmodule
 */
#include "tutil.h"
#include "cmodule.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testCModule, teststdio)
{
    struct array funs = parse_c_file("/usr/include/stdio.h");

    ASSERT_EQ(64, array_size(&funs));
}
