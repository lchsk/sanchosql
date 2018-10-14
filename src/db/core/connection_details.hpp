#ifndef CONNECTION_DETAILS_HPP
#define CONNECTION_DETAILS_HPP

#include <sstream>

#include <glibmm.h>

namespace sancho {
    namespace db{
class ConnectionDetails {
  public:
    const std::string postgres_connection_string();
    const std::string postgres_string_safe();

    void set_host(const std::string &hostname);

    Glib::ustring name;
    std::string host;
    std::string user;
    std::string password;
    std::string dbname;
    std::string port;
    std::string sslmode;
    bool save_password;

    std::unique_ptr<std::vector<Glib::ustring>> schemas;

  private:
    const std::string postgres_string_(bool include_password);
};
} // namespace sancho
}

#endif
