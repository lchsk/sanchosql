#include "pg_conn.hpp"

#include <iostream>

Connections Connections::ins;

PostgresConnection::PostgresConnection
(const std::shared_ptr<ConnectionDetails>& conn_details) :
    conn_details(conn_details)
{
    try {
        conn = std::make_unique<pqxx::connection>(conn_details->postgres_string());

        if (conn->is_open()) {
            std::cout << "Connection open" << std::endl;
        } else {
            std::cerr << "Cannot open database" << std::endl;

            throw std::exception();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;

        throw e;
    }
}
