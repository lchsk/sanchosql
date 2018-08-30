#ifndef MOCK_PG_CONN_HPP
#define MOCK_PG_CONN_HPP

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../src/pg_conn.hpp"

class MockPostgresConnection : public sancho::PostgresConnection
{
public:
	MockPostgresConnection(const std::shared_ptr<sancho::ConnectionDetails>& conn_details)
		: PostgresConnection::PostgresConnection(conn_details) {};

	MOCK_METHOD0(init_connection, void());
	MOCK_METHOD1(run_query, std::shared_ptr<sancho::QueryResult>(const std::string& query));
	// MOCK_METHOD2(run_query, std::shared_ptr<sancho::QueryResult>(const std::string& query, const std::string& columns_query));

	const std::vector<sancho::PrimaryKey>
    get_primary_key(const std::string& table_name, const std::string& schema_name) const noexcept override {
		std::vector<sancho::PrimaryKey> pk;

		pk.push_back(sancho::PrimaryKey("test_column", "test_int"));

		return pk;
	}
};

#endif
