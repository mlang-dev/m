#include "gtest/gtest.h"
#include "util.h"

TEST(testUtil, testIdNameGenerator){
  reset_id_name();
  ASSERT_STREQ("a", get_id_name().c_str());
  ASSERT_STREQ("b", get_id_name().c_str());
  reset_id_name("z");
  ASSERT_STREQ("z", get_id_name().c_str());
  ASSERT_STREQ("aa", get_id_name().c_str());
}

TEST(testUtil, testTwoLetters){
  reset_id_name("az");
  ASSERT_STREQ("az", get_id_name().c_str());
  ASSERT_STREQ("ba", get_id_name().c_str());
  ASSERT_STREQ("bb", get_id_name().c_str());
}

TEST(testUtil, testMoreLetters){
  reset_id_name("zzzy");
  ASSERT_STREQ("zzzy", get_id_name().c_str());
  ASSERT_STREQ("zzzz", get_id_name().c_str());
  ASSERT_STREQ("aaaaa", get_id_name().c_str());
}
