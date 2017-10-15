#include "pg_conn.hpp"

#include <iostream>

Connections Connections::ins;

PostgresConnection::PostgresConnection
(const std::shared_ptr<ConnectionDetails>& conn_details) :
    conn_details(conn_details),
    is_open_(false),
    error_message_("")
{
}

void PostgresConnection::init_connection()
{
    std::cout << "Trying to connect to: " << conn_details->postgres_string() << std::endl;

    try {
        conn = std::make_unique<pqxx::connection>(conn_details->postgres_string());

        error_message_ = "";
        is_open_ = conn->is_open();
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;

        error_message_ = e.what();
        is_open_ = false;
    }
}
