#include "connection_details.hpp"

namespace san {
void ConnectionDetails::set_host(const std::string& hostname)
{
    if (hostname == "localhost")
        host = "127.0.0.1";
    else
        host = hostname;
}

const std::string ConnectionDetails::postgres_connection_string()
{
    return postgres_string_(true);
}

const std::string ConnectionDetails::postgres_string_safe()
{
    return postgres_string_(false);
}

const std::string ConnectionDetails::postgres_string_(bool include_password)
{
    std::stringstream conn;

    conn << "hostaddr = " << host << " user = " << user << " password = ";

    if (include_password) {
        conn << password;
    } else {
        conn << "****";
    }

    conn << " dbname = " << dbname << " port = " << port;

    return conn.str();
}
} // namespace san
