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

    void Connections::update_conn(
        const Glib::ustring& old_conn_name,
        const Glib::ustring& new_conn_name,
        const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& dbname,
        const std::string& port)
    {
        if (! exists(old_conn_name))
            return;

        auto conn = get(old_conn_name);

        connections.erase(connections.find(old_conn_name));

        conn->name = new_conn_name;
        conn->host = host;
        conn->user = user;
        conn->password = password;
        conn->dbname = dbname;
        conn->port = port;

        connections[new_conn_name] = conn;
    }

    bool Connections::any_fields_empty(
        const Glib::ustring& host,
        const Glib::ustring& port,
        const Glib::ustring& db,
        const Glib::ustring& user,
        const Glib::ustring& connection_name) const
    {
        if (san::util::is_empty(host) ||
            san::util::is_empty(port) ||
            san::util::is_empty(db) ||
            san::util::is_empty(user) ||
            san::util::is_empty(connection_name))
            // no password - it can be empty
            return true;

        return false;
    }

    std::shared_ptr<san::ConnectionDetails>&
    Connections::get(const Glib::ustring& conn_name)
    {
        return connections.at(conn_name);
    }

    bool Connections::can_update_conn_details(
        const Glib::ustring& old_conn_name,
        const Glib::ustring& new_conn_name,
        const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& dbname,
        const std::string& port)
    {
        if (any_fields_empty(host, port, dbname, user, new_conn_name))
            return false;

        if (! exists(old_conn_name))
            return false;

        if (exists(new_conn_name))
            return false;

        const auto& conn = connections[old_conn_name];

        if (old_conn_name != new_conn_name) return true;

        if (conn->host != host ||
            conn->user != user ||
            conn->password != password ||
            conn->dbname != dbname ||
            conn->port != port) return true;

        return false;
    }
}
