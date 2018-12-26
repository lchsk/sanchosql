#ifndef CONNECTIONS_HPP
#define CONNECTIONS_HPP

#include <map>
#include <memory>

#include <glibmm.h>

#include "connection_details.hpp"
#include "../../string.hpp"

namespace sancho {
    namespace db{
class Connections {
  public:
    Connections();

    std::shared_ptr<sancho::db::ConnectionDetails> &end() { return conn_end; }

    std::shared_ptr<sancho::db::ConnectionDetails> &
    find_connection(const Glib::ustring &connection_name);

    void add(const Glib::ustring &name, const std::string &host,
             const std::string &user, const std::string &password,
             const std::string &dbname, const std::string &port, const std::string &sslmode,
             bool save_password);

    static Connections *instance() { return &ins; }

    const std::map<Glib::ustring, std::shared_ptr<sancho::db::ConnectionDetails>> &
    get_connections() const {
        return connections;
    };

    const unsigned size() const { return connections.size(); }

    std::shared_ptr<sancho::db::ConnectionDetails> &
    get(const Glib::ustring &conn_name);

    bool exists(const Glib::ustring &conn_name) const {
        return IN_MAP(connections, conn_name);
    };

    void remove(const Glib::ustring &conn_name);

    bool can_update_conn_details(const Glib::ustring &old_conn_name,
                                 const Glib::ustring &new_conn_name,
                                 const std::string &host,
                                 const std::string &user,
                                 const std::string &password,
                                 const std::string &dbname,
                                 const std::string &port, const std::string &sslmode, bool save_password);

    void update_conn(const Glib::ustring &old_conn_name,
                     const Glib::ustring &new_conn_name,
                     const std::string &host, const std::string &user,
                     const std::string &password, const std::string &dbname,
                     const std::string &port, const std::string &sslmode, bool save_password);

    bool any_fields_empty(const Glib::ustring &host, const Glib::ustring &port,
                          const Glib::ustring &db, const Glib::ustring &user,
                          const Glib::ustring &connection_name) const;

    void init_connections();
    void save_connections();

    Glib::ustring CONN_PATH = "";

    std::shared_ptr<sancho::db::ConnectionDetails> current_connection;

  private:
    void open_conn_file();
    void load_connections();
    const Glib::ustring get_conn_value(const Glib::ustring &,
                                       const Glib::ustring &) const;
    const bool get_conn_value_bool(const Glib::ustring &,
                                   const Glib::ustring &) const;

    // Returned if the actual connection was not found
    std::shared_ptr<sancho::db::ConnectionDetails> conn_end;

    std::map<Glib::ustring, std::shared_ptr<sancho::db::ConnectionDetails>>
        connections;

    static Connections ins;

    // TODO: Move it somewhere else
    Glib::KeyFile conn_file;
};
} // namespace sancho
}

#endif
