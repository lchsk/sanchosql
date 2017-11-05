#include <map>

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

TEST(Util, trim)
{
    EXPECT_EQ(san::util::trim("abc"), "abc");
    EXPECT_EQ(san::util::trim(" abc "), "abc");
    EXPECT_EQ(san::util::trim(" a b c "), "a b c");
    EXPECT_EQ(san::util::trim("  "), "");
    EXPECT_EQ(san::util::trim(""), "");
}

TEST(Util, is_empty)
{
    EXPECT_TRUE(san::util::is_empty(""));
    EXPECT_TRUE(san::util::is_empty("  "));
    EXPECT_FALSE(san::util::is_empty("abc"));
    EXPECT_FALSE(san::util::is_empty("    abc "));
}

TEST(Util, in_map_macro)
{
    std::map<std::string, int> m { {"abc", 1} };

    EXPECT_TRUE(IN_MAP(m, "abc"));
    EXPECT_FALSE(IN_MAP(m, "test"));
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}