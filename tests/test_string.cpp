#include <map>

#include "gtest/gtest.h"

#include "../src/string.hpp"

TEST(Util, contains_only_numbers)
{
    EXPECT_TRUE(san::string::contains_only_numbers("123"));
    EXPECT_TRUE(san::string::contains_only_numbers(""));
    EXPECT_FALSE(san::string::contains_only_numbers("abc0"));
    EXPECT_FALSE(san::string::contains_only_numbers("1abc"));
}

TEST(Util, replace_all)
{
    EXPECT_EQ(san::string::replace_all("abc def", "def", "abc"), "abc abc");
}

TEST(Util, quote_sql)
{
    EXPECT_EQ(san::string::escape_sql("test"), "test");
    EXPECT_EQ(san::string::escape_sql("test's"), "test''s");
    EXPECT_EQ(san::string::escape_sql("test's test's"), "test''s test''s");
}

TEST(Util, prepare_sql_value)
{
    EXPECT_EQ(san::string::prepare_sql_value("Test's\\n"), "'Test''s\n'");
    EXPECT_EQ(san::string::prepare_sql_value("\\ttest\\n"), "'\ttest\n'");
}

TEST(Util, trim)
{
    EXPECT_EQ(san::string::trim("abc"), "abc");
    EXPECT_EQ(san::string::trim(" abc "), "abc");
    EXPECT_EQ(san::string::trim(" a b c "), "a b c");
    EXPECT_EQ(san::string::trim("  "), "");
    EXPECT_EQ(san::string::trim(""), "");
}

TEST(Util, is_empty)
{
    EXPECT_TRUE(san::string::is_empty(""));
    EXPECT_TRUE(san::string::is_empty("  "));
    EXPECT_FALSE(san::string::is_empty("abc"));
    EXPECT_FALSE(san::string::is_empty("    abc "));
}

TEST(Util, in_map_macro)
{
    std::map<std::string, int> m { {"abc", 1} };

    EXPECT_TRUE(IN_MAP(m, "abc"));
    EXPECT_FALSE(IN_MAP(m, "test"));
}

TEST(Query, get_query)
{
    const std::vector<std::pair<Glib::ustring, std::string>> data = {
        {"sel|ect * from test;", "select * from test;"},
        {"select * from tes|t", "select * from test;"},
        {"s|elect * from test;", "select * from test;"},
        {"select * from test; select |1 from a", "select 1 from a;"},
        {"select * |from test; select 1 from a", "select * from test;"},

        {"select * from test|", "select * from test;"},

        {"select * from test;|", "select * from test;"},

        {"select * from test;;|", "select * from test;"},

        {"select * from test;| select 1 from test;", "select 1 from test;"},
        {"select * from test;|select 1 from test;", "select 1 from test;"},
        {"select * from test;|select 1 from test", "select 1 from test;"},
        {"select * from test|;select 1 from test", "select * from test;"},

        {"|select 1 from test;", "select 1 from test;"},

        {"| select 1 from test;", "select 1 from test;"},

        {";|select 1 from test;", "select 1 from test;"},
        {" ; | select 1 from test;", "select 1 from test;"},

        {"", ""},
        {"   ", ""},
        {" test ", "test"},
        {" | ", ""},
        {"; ", ";"},
        {";;; ", ";;;"},
        {";;|; ", ";;;"},

        {"                                                                                                                  ", ""},
        {"                     \n                                                                                             ", ""},
        {"  |                                                                                                                ", ""},
    };

    for (const auto& query : data) {
        const Glib::ustring::size_type point = query.first.find("|");
        const Glib::ustring text = san::string::replace_all(query.first, "|", "");

        EXPECT_EQ(std::string(san::string::get_query(text, point)), query.second);
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
