#ifndef CONNECTIONS_HPP
#define CONNECTIONS_HPP

#include <memory>
#include <map>

#include <glibmm.h>

#include "connection_details.hpp"
#include "util.hpp"

namespace san
{
    class Connections
    {
    public:
        Connections();

        std::shared_ptr<san::ConnectionDetails>& connection() {
            return conn;
        }

        void add(const Glib::ustring& name,
                 const std::string& host,
                 const std::string& user,
                 const std::string& password,
                 const std::string& dbname,
                 const std::string& port);

        static Connections* instance() { return &ins; }

        const std::map<Glib::ustring, std::shared_ptr<san::ConnectionDetails>>&
        get_connections() const {
            return connections;
        };

        bool exists(const Glib::ustring& conn_name) const {
            return IN_MAP(connections, conn_name);
        };

    private:
        std::shared_ptr<san::ConnectionDetails> conn;
        std::map<Glib::ustring,
                 std::shared_ptr<san::ConnectionDetails>> connections;

        static Connections ins;
    };
}

#endif
