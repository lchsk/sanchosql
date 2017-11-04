#include "gtest/gtest.h"

#include "../src/util.hpp"

TEST(Util, contains_only_numbers)
{
    EXPECT_TRUE(san::util::contains_only_numbers("123"));
    EXPECT_TRUE(san::util::contains_only_numbers(""));
    EXPECT_FALSE(san::util::contains_only_numbers("abc0"));
    EXPECT_FALSE(san::util::contains_only_numbers("1abc"));
}

TEST(Util, replace_all)
{
    EXPECT_EQ(san::util::replace_all("abc def", "def", "abc"), "abc abc");
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
