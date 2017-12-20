#include <memory>
#include <map>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../src/model/list_tab_model.hpp"
#include "../src/connection_details.hpp"

#include "mock_pg_conn.hpp"

TEST(CellUpdate, test_simple_tab_model_creation)
{
    san::SimpleTabModel model(std::make_shared<san::ConnectionDetails>(), "test_table", "test_public");

    EXPECT_EQ(model.get_primary_key().size(), 1);
    EXPECT_EQ(model.get_primary_key()[0].column_name, "test_column");
}

TEST(CellUpdate, test_update_1_cell_1_row)
{
    san::SimpleTabModel model(std::make_shared<san::ConnectionDetails>(), "test_table", "test_public");

    std::map<Glib::ustring, Glib::ustring> pk {{"id", "1"}};
    std::map<Glib::ustring, Glib::ustring> values {{"name", "Name"}};

    model.map_test[pk] = values;

    const std::string expected_query
        = "update test_public.test_table set name = 'Name' where id = '1'; commit;";

    EXPECT_CALL(*model.get_mock_connection(), run_query(expected_query));

    model.accept_changes();

    EXPECT_EQ(model.map_test.size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
