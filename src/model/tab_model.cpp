#include "tab_model.hpp"

TabModel::TabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details) :
	limit(DEFAULT_LIMIT),
	offset(DEFAULT_OFFSET),
	conn_details(conn_details),
	connection(std::make_unique<san::PostgresConnection>(conn_details))
{
	connection->init_connection();
}
