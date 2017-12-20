GTEST_FLAGS = `pkg-config --cflags --libs gtest`
GMOCK_FLAGS = `pkg-config --cflags --libs gmock` -lgmock
GTK_FLAGS = `pkg-config --cflags --libs gtkmm-3.0`
PQXX_FLAGS = `pkg-config --cflags --libs libpqxx`

##############################
#                            #
#        List of tests       #
#                            #
##############################

TEST_UTIL = test_util
TEST_CELL_UPDATE = test_cell_update

##############################

.PHONY: all clean check

TESTS = $(TEST_UTIL) $(TEST_CELL_UPDATE)

all: $(TESTS) check

clean:
	rm -f $(TESTS) *.o

check:
	$(foreach var, $(TESTS), ./$(var);)

test_util:
	$(CXX) test_util.cpp ../src/util.cpp $(GTEST_FLAGS) $(GTK_FLAGS) -o $(TEST_UTIL)

test_cell_update:
	$(CXX) test_cell_update.cpp ../src/util.cpp ../src/conn_util.cpp ../src/query_result.cpp ../src/connection_details.cpp ../src/connections.cpp ../src/pg_conn.cpp ../src/model/list_tab_model.cpp $(GTEST_FLAGS) $(GMOCK_FLAGS) $(GTK_FLAGS) $(PQXX_FLAGS) -o $(TEST_CELL_UPDATE)
