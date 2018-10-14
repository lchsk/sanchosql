#include "connection_details.hpp"

namespace sancho {
    namespace db{
void ConnectionDetails::set_host(const std::string &hostname) {
    host = hostname;
}

const std::string ConnectionDetails::postgres_connection_string() {
    return postgres_string_(true);
}

const std::string ConnectionDetails::postgres_string_safe() {
    return postgres_string_(false);
}

const std::string ConnectionDetails::postgres_string_(bool include_password) {
    std::stringstream conn;

    conn << "host = " << host << " user = " << user << " password = ";

    if (include_password) {
        conn << password;
    } else {
        conn << "****";
    }

    conn << " dbname = " << dbname << " port = " << port << " sslmode = " << sslmode;

    return conn.str();
}
} // namespace sancho
}
