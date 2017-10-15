#ifndef TAB_MODEL_HPP
#define TAB_MODEL_HPP

#include "../pg_conn.hpp"

class TabModel
{
public:
	TabModel(const std::shared_ptr<ConnectionDetails>& conn_details);

	PostgresConnection& conn() const {
		return *connection;
	}

private:
	std::shared_ptr<ConnectionDetails> conn_details;
	std::unique_ptr<PostgresConnection> connection;
};

#endif
