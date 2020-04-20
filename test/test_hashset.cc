/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "clib/hashset.h"
#include "clib/string.h"


TEST(testHashset, TestAddAndGet)
{
    hashset hs;
    hashset_init(&hs, sizeof(int));
    int i=10; int j=20;
    hashset_add(&hs, &i);
    hashset_add(&hs, &j);
    int k = 0;
    ASSERT_EQ(2, hashset_size(&hs));
    ASSERT_TRUE(hashset_in(&hs, &i));
    ASSERT_TRUE(hashset_in(&hs, &j));
    ASSERT_FALSE(hashset_in(&hs, &k));
    hashset_deinit(&hs);

}
