#include "connections.hpp"

namespace san
{
    Connections::Connections()
    {
        conn = std::make_shared<san::ConnectionDetails>();

        conn->host = "127.0.0.1";
        conn->user = "sancho";
        conn->password = "sancho";
        conn->dbname = "sancho";
        conn->port = "5432";

        add("sancho", "localhost", "sancho", "sancho", "sancho", "5432");
    }

    void Connections::add(const Glib::ustring& name,
                          const std::string& host,
                          const std::string& user,
                          const std::string& password,
                          const std::string& dbname,
                          const std::string& port)
    {
        auto conn = std::make_shared<san::ConnectionDetails>();

        conn->name = name;
        conn->host = host;
        conn->user = user;
        conn->password = password;
        conn->dbname = dbname;
        conn->port = port;

        connections[name] = conn;
    }
}
