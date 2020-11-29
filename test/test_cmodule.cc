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
    size_t funs_size = array_size(&funs);
    ASSERT_GE(100, funs_size);
    ASSERT_LE(50, funs_size);
}
