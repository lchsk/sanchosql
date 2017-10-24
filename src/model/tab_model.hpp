#ifndef TAB_MODEL_HPP
#define TAB_MODEL_HPP

#include "../pg_conn.hpp"

class TabModel
{
public:
	TabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details);

	san::PostgresConnection& conn() const {
		return *connection;
	}

	std::string get_query() const {
		return "select * from " + table_name + " offset " + std::to_string(offset) + " limit " + std::to_string(limit);
	}

	std::string table_name;

	unsigned limit;
	unsigned offset;

private:
	std::shared_ptr<san::ConnectionDetails> conn_details;
	std::unique_ptr<san::PostgresConnection> connection;
};

#endif
