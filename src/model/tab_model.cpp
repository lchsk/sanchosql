#include "tab_model.hpp"

TabModel::TabModel(const std::shared_ptr<ConnectionDetails>& conn_details) :
	conn_details(conn_details),
	connection(std::make_unique<PostgresConnection>(conn_details))
{
}
