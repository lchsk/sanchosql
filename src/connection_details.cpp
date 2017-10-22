#include "connection_details.hpp"

void ConnectionDetails::set_host(const std::string& hostname)
{
    if (hostname == "localhost")
        host = "127.0.0.1";
    else
        host = hostname;
}

const std::string ConnectionDetails::postgres_string()
{
    std::stringstream conn;

    conn << "hostaddr = " << host
         << " user = " << user
         << " password = " << password
         << " dbname = " << dbname
         << " port = " << port;

    return conn.str();
}
