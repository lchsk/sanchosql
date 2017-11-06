#include <iostream>

#include "connections.hpp"

namespace san
{
    Connections::Connections()
    {
        open_conn_file();

        conn = std::make_shared<san::ConnectionDetails>();

        conn->host = "127.0.0.1";
        conn->user = "sancho";
        conn->password = "sancho";
        conn->dbname = "sancho";
        conn->port = "5432";
    }

    void Connections::open_conn_file()
    {
        try {
            conn_file.load_from_file(CONN_PATH);
        } catch (const Glib::Error& ex) {
            std::cerr << "Unable to load connections file: " << ex.what() << std::endl;

            return;
        }

        load_connections();
    }

    const Glib::ustring Connections::get_conn_value(const Glib::ustring& group,
                                                    const Glib::ustring& key) const
    {
        try {
            return conn_file.get_value(group, key);
        } catch (const Glib::KeyFileError& ex) {
            // Pass
        }

        return "";
    }

    void Connections::load_connections()
    {
        connections.clear();

        for (int i = 1; i <= 1000; i++) {
            const Glib::ustring group = Glib::ustring::compose("conn_%1", i);

            if (! conn_file.has_group(group))
                return;

            const Glib::ustring name = get_conn_value(group, "name");
            const Glib::ustring host = get_conn_value(group, "host");
            const Glib::ustring user = get_conn_value(group, "user");
            const Glib::ustring password = get_conn_value(group, "password");
            const Glib::ustring dbname = get_conn_value(group, "dbname");
            const Glib::ustring port = get_conn_value(group, "port");

            if (san::util::is_empty(name)) continue;

            add(name, host, user, password, dbname, port);
        }
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
