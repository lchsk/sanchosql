#include <iostream>

#include "connections.hpp"

namespace sancho {
Connections::Connections()
{
    conn_end = std::make_shared<sancho::ConnectionDetails>();

    conn_end->host = "";
    conn_end->user = "";
    conn_end->password = "";
    conn_end->dbname = "";
    conn_end->port = "";
    conn_end->save_password = false;
}

void Connections::init_connections() { open_conn_file(); }

void Connections::open_conn_file()
{
    try {
        conn_file.load_from_file(CONN_PATH);
    } catch (const Glib::Error& ex) {
        std::cerr << "Cannot load " << CONN_PATH << std::endl;

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
        g_warning("Cannot find %s.%s", group.c_str(), key.c_str());
    }

    return "";
}

const bool Connections::get_conn_value_bool(const Glib::ustring& group,
                                            const Glib::ustring& key) const
{
    try {
        return conn_file.get_boolean(group, key);
    } catch (const Glib::KeyFileError& ex) {
        g_warning("Cannot find %s.%s", group.c_str(), key.c_str());
    }

    return false;
}

void Connections::load_connections()
{
    connections.clear();

    int i = 1;

    while (true) {
        const Glib::ustring group = Glib::ustring::compose("conn_%1", i);

        if (!conn_file.has_group(group))
            return;

        const Glib::ustring name = get_conn_value(group, "name");
        const Glib::ustring host = get_conn_value(group, "host");
        const Glib::ustring user = get_conn_value(group, "user");
        const Glib::ustring password = get_conn_value(group, "password");
        const Glib::ustring dbname = get_conn_value(group, "dbname");
        const Glib::ustring port = get_conn_value(group, "port");
        const bool save_password = get_conn_value_bool(group, "save_password");

        if (sancho::string::is_empty(name))
            continue;

        add(name, host, user, password, dbname, port, save_password);

        i++;
    }
}

void Connections::save_connections()
{
    for (int i = 1; i <= 1000; i++) {
        const Glib::ustring group = Glib::ustring::compose("conn_%1", i);

        if (conn_file.has_group(group))
            conn_file.remove_group(group);
        else
            break;
    }

    Glib::TimeVal time;
    time.assign_current_time();

    const Glib::ustring current_datetime = time.as_iso8601();

    conn_file.set_string("sancho", "last_updated", current_datetime);
    conn_file.set_comment(
        "sancho", "last_updated",
        "This file is automatically generated. Please do not change it!");

    int i = 1;

    for (const auto& conn_details : connections) {
        const Glib::ustring group = Glib::ustring::compose("conn_%1", i);
        const Glib::ustring password = conn_details.second->save_password
                                           ? conn_details.second->password
                                           : "";

        conn_file.set_string(group, "name", conn_details.second->name);
        conn_file.set_string(group, "host", conn_details.second->host);
        conn_file.set_string(group, "user", conn_details.second->user);
        conn_file.set_string(group, "password", password);
        conn_file.set_string(group, "dbname", conn_details.second->dbname);
        conn_file.set_string(group, "port", conn_details.second->port);
        conn_file.set_boolean(group, "save_password",
                              conn_details.second->save_password);

        i++;
    }

    conn_file.save_to_file(CONN_PATH);
}

void Connections::add(const Glib::ustring& name, const std::string& host,
                      const std::string& user, const std::string& password,
                      const std::string& dbname, const std::string& port,
                      bool save_password)
{
    auto conn = std::make_shared<sancho::ConnectionDetails>();

    conn->name = name;
    conn->host = host;
    conn->user = user;
    conn->password = password;
    conn->dbname = dbname;
    conn->port = port;
    conn->save_password = save_password;

    connections[name] = conn;
}

void Connections::update_conn(const Glib::ustring& old_conn_name,
                              const Glib::ustring& new_conn_name,
                              const std::string& host, const std::string& user,
                              const std::string& password,
                              const std::string& dbname,
                              const std::string& port, bool save_password)
{
    if (!exists(old_conn_name))
        return;

    auto conn = get(old_conn_name);

    connections.erase(connections.find(old_conn_name));

    conn->name = new_conn_name;
    conn->host = host;
    conn->user = user;
    conn->password = password;
    conn->dbname = dbname;
    conn->port = port;
    conn->save_password = save_password;

    connections[new_conn_name] = conn;
}

bool Connections::any_fields_empty(const Glib::ustring& host,
                                   const Glib::ustring& port,
                                   const Glib::ustring& db,
                                   const Glib::ustring& user,
                                   const Glib::ustring& connection_name) const
{
    if (sancho::string::is_empty(host) || sancho::string::is_empty(port) ||
        sancho::string::is_empty(db) || sancho::string::is_empty(user) ||
        sancho::string::is_empty(connection_name))
        // no password - it can be empty
        return true;

    return false;
}

std::shared_ptr<sancho::ConnectionDetails>&
Connections::get(const Glib::ustring& conn_name)
{
    return connections.at(conn_name);
}

bool Connections::can_update_conn_details(
    const Glib::ustring& old_conn_name, const Glib::ustring& new_conn_name,
    const std::string& host, const std::string& user,
    const std::string& password, const std::string& dbname,
    const std::string& port, bool save_password)
{
    if (any_fields_empty(host, port, dbname, user, new_conn_name))
        return false;

    if (!exists(old_conn_name))
        return false;

    if (old_conn_name != new_conn_name && exists(new_conn_name))
        return false;

    const auto& conn = connections[old_conn_name];

    if (old_conn_name != new_conn_name)
        return true;

    if (conn->host != host || conn->user != user ||
        conn->password != password || conn->save_password != save_password ||
        conn->dbname != dbname || conn->port != port)
        return true;

    return false;
}

    std::shared_ptr<sancho::ConnectionDetails>&
    Connections::find_connection(const Glib::ustring& connection_name)
    {
        if (!exists(connection_name))
            return end();

        return connections[connection_name];
    }

    void Connections::remove(const Glib::ustring& conn_name)
    {
        if (!exists(conn_name))
            return;

        connections.erase(connections.find(conn_name));
    }


} // namespace sancho
