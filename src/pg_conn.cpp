#include "pg_conn.hpp"

#include <iostream>
#include <sstream>


PostgresConnection::PostgresConnection(const std::string host,
                                       const std::string user,
                                       const std::string password,
                                       const std::string dbname,
                                       const unsigned port) :
    host(host),
    user(user),
    password(password),
    dbname(dbname),
    port(port)
{
    std::stringstream c;

    c << "hostaddr = " << host
      << " user = " << user
      << " password = " << password
      << " dbname = " << dbname
      << " port = " << port;

    try {
        conn = std::make_unique<pqxx::connection>(c.str());

        if (! conn->is_open()) {
            std::cerr << "Cannot open database" << std::endl;

            throw std::exception();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;

        throw e;
    }
}
