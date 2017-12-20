#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../src/model/list_tab_model.hpp"
#include "../src/connection_details.hpp"

#include "mock_pg_conn.hpp"

TEST(CellUpdate, test_simple_tab_model_creation)
{
    const std::shared_ptr<san::ConnectionDetails> conn_details;
    san::SimpleTabModel model(conn_details, "test_table", "test_public");

    EXPECT_EQ(model.get_primary_key().size(), 1);
    EXPECT_EQ(model.get_primary_key()[0].column_name, "test_column");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
