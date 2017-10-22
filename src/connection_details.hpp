#ifndef CONNECTION_DETAILS_HPP
#define CONNECTION_DETAILS_HPP

#include <sstream>

namespace san
{
    struct ConnectionDetails
    {
        const std::string postgres_string();

        void set_host(const std::string& hostname);

        std::string name;
        std::string host;
        std::string user;
        std::string password;
        std::string dbname;
        std::string port;
    };
}

#endif
