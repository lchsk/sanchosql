GTEST_FLAGS = `pkg-config --cflags --libs gtest`
GTK_FLAGS = `pkg-config --cflags --libs gtkmm-3.0`

##############################
#                            #
#        List of tests       #
#                            #
##############################

TEST_UTIL = test_util

##############################

.PHONY: all clean check

TESTS = $(TEST_UTIL)

all: $(TESTS) check

clean:
	rm -f $(TESTS) *.o

check:
	$(foreach var, $(TESTS), ./$(var);)

test_util:
	$(CXX) test_util.cpp ../src/util.cpp $(GTEST_FLAGS) $(GTK_FLAGS) -o $(TEST_UTIL)
