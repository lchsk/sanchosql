#ifndef CONNECTION_DETAILS_HPP
#define CONNECTION_DETAILS_HPP

#include <sstream>

#include <glibmm.h>

namespace san
{
    struct ConnectionDetails
    {
        const std::string postgres_string();

        void set_host(const std::string& hostname);

        Glib::ustring name;
        std::string host;
        std::string user;
        std::string password;
        std::string dbname;
        std::string port;

        std::unique_ptr<std::vector<Glib::ustring>> schemas;
    };
}

#endif
