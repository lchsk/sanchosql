#ifndef MOCK_PG_CONN_HPP
#define MOCK_PG_CONN_HPP

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../src/pg_conn.hpp"

class MockPostgresConnection : public san::PostgresConnection
{
public:
	MockPostgresConnection(const std::shared_ptr<san::ConnectionDetails>& conn_details)
		: PostgresConnection::PostgresConnection(conn_details) {};

	MOCK_METHOD0(init_connection, void());
	MOCK_METHOD1(run_query, std::shared_ptr<san::QueryResult>(const std::string& query));

	const std::vector<san::PrimaryKey>
    get_primary_key(const std::string& table_name, const std::string& schema_name) const override {
		std::vector<san::PrimaryKey> pk;

		pk.push_back(san::PrimaryKey("test_column", "test_int"));

		return pk;
	}
};

#endif
