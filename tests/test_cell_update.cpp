// #include <memory>
// #include <map>

// #include "gmock/gmock.h"
// #include "gtest/gtest.h"

// #include "../src/model/list_tab_model.hpp"
// #include "../src/connection_details.hpp"

// #include "mock_pg_conn.hpp"

// using ::testing::Exactly;

// TEST(CellUpdate, test_simple_tab_model_creation)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // EXPECT_EQ(model.get_primary_key().size(), 1);
    // EXPECT_EQ(model.get_primary_key()[0].column_name, "test_column");
// }

// TEST(CellUpdate, test_update_no_pending_changes)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // EXPECT_CALL(*model.get_mock_connection(), run_query("")).Times(Exactly(0));

    // model.accept_changes();

    // EXPECT_EQ(model.map_test.size(), 0);
// }

// TEST(CellUpdate, test_update_1_cell_1_row)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // std::map<Glib::ustring, Glib::ustring> pk {{"id", "1"}};
    // std::map<Glib::ustring, Glib::ustring> values {{"name", "Name"}};

    // model.map_test[pk] = values;

    // const std::string expected_query
        // = "update test_public.test_table set name = 'Name' where id = '1'; commit;";

    // EXPECT_CALL(*model.get_mock_connection(), run_query(expected_query));

    // model.accept_changes();

    // EXPECT_EQ(model.map_test.size(), 0);
// }

// TEST(CellUpdate, test_update_2_cells_1_row)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // std::map<Glib::ustring, Glib::ustring> pk {{"id", "1"}, {"name", "Name"}};
    // std::map<Glib::ustring, Glib::ustring> values {{"city", "City"}};

    // model.map_test[pk] = values;

    // const std::string expected_query
        // = "update test_public.test_table set city = 'City' where id = '1' and name = 'Name'; commit;";

    // EXPECT_CALL(*model.get_mock_connection(), run_query(expected_query));

    // model.accept_changes();

    // EXPECT_EQ(model.map_test.size(), 0);
// }

// TEST(CellUpdate, test_update_2_different_rows)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // std::map<Glib::ustring, Glib::ustring> pk1 {{"id", "1"}, {"name", "Name"}};
    // std::map<Glib::ustring, Glib::ustring> values1 {{"city", "City"}, {"created", "2000-01-01"}, {"is_enabled", "t"}};

    // std::map<Glib::ustring, Glib::ustring> pk2 {{"id", "1"}, {"name", "Name"}, {"test", "Test"}};
    // std::map<Glib::ustring, Glib::ustring> values2 {{"city", "City"}, {"is_enabled", "t"}};

    // model.map_test[pk1] = values1;
    // model.map_test[pk2] = values2;

    // std::stringstream expected_query;

    // expected_query << "update test_public.test_table set city = 'City', created = '2000-01-01', is_enabled = 't'"
                   // << " where id = '1' and name = 'Name'; update test_public.test_table set city = 'City'"
                   // << ", is_enabled = 't' where id = '1' and name = 'Name' and test = 'Test'; commit;";

    // EXPECT_CALL(*model.get_mock_connection(), run_query(expected_query.str()));

    // model.accept_changes();

    // EXPECT_EQ(model.map_test.size(), 0);
// }

// Primary Key change

// TEST(PrimaryKeyUpdate, simple_primary_key)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // model.pk_changes["name"] = std::make_pair<Glib::ustring, Glib::ustring>("old_name", "new_name");
    // model.pk_hist["name"] = "old_name";

    // const std::string expected_query
        // = "update test_public.test_table set name = 'new_name' where name = 'old_name'; commit;";

    // EXPECT_CALL(*model.get_mock_connection(), run_query(expected_query));

    // model.accept_pk_change();

    // EXPECT_EQ(model.map_test.size(), 0);
// }

// TEST(PrimaryKeyUpdate, compound_primary_key_one_changed)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // model.pk_changes["name"] = std::make_pair<Glib::ustring, Glib::ustring>("old_name", "new_name");
    // model.pk_changes["email"] = std::make_pair<Glib::ustring, Glib::ustring>("old_email", "new_email");
    // model.pk_hist["email"] = "old_email";
    // model.pk_hist["name"] = "old_name";

    // const std::string expected_query
        // = "update test_public.test_table set name = 'new_name' where email = 'old_email' and name = 'old_name'; commit;";

    // EXPECT_CALL(*model.get_mock_connection(), run_query(expected_query));

    // model.accept_pk_change();

    // EXPECT_EQ(model.map_test.size(), 0);
// }

// TEST(PrimaryKeyUpdate, compound_primary_key_both_changed)
// {
    // sancho::SimpleTabModel model(std::make_shared<sancho::ConnectionDetails>(), "test_table", "test_public");

    // model.pk_changes["email"] = std::make_pair<Glib::ustring, Glib::ustring>("old_email", "new_email");
    // model.pk_changes["name"] = std::make_pair<Glib::ustring, Glib::ustring>("old_name", "new_name");
    // model.pk_hist["email"] = "old_email";
    // model.pk_hist["name"] = "old_name";

    // Don't allow to change > 1 PK columns at the same time
    // EXPECT_CALL(*model.get_mock_connection(), run_query("")).Times(Exactly(0));

    // model.accept_pk_change();

    // EXPECT_EQ(model.pk_changes.size(), 0);
    // EXPECT_EQ(model.pk_hist.size(), 0);
// }

// int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);

    // return RUN_ALL_TESTS();
// }
