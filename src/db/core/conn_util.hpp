#ifndef CONN_UTIL_HPP
#define CONN_UTIL_HPP

#include <pqxx/pqxx>

#include <unordered_map>

namespace sancho {
class OidMapping {
  public:
    OidMapping(const pqxx::oid& oid, const std::string& udt_name,
               const std::string& data_type)
        : oid(oid), udt_name(udt_name), data_type(data_type) {}

    pqxx::oid oid;
    std::string udt_name;
    std::string data_type;
};

namespace db {
class Column {
  public:
    Column(const std::string& column_name, const std::string& data_type,
           const std::string& char_length, bool is_nullable);

    const std::string& get_column_name() const noexcept;
    const std::string& get_data_type() const noexcept;
    const std::string& get_char_length() const noexcept;
    bool is_nullable() const noexcept;

  private:
    const std::string m_column_name;
    const std::string m_data_type;
    const std::string m_char_length;
    const bool m_is_nullable;
};

class ColumnMetadata {
  public:
    ColumnMetadata(const std::string& character_maximum_length,
                   bool is_nullable);

    const std::string& get_character_maximum_length() const noexcept;
    bool get_is_nullable() const noexcept;

  private:
    std::string m_character_maximum_length;
    bool m_is_nullable;
};
}

const std::string
get_data_type(const pqxx::oid oid,
              std::unordered_map<pqxx::oid, sancho::OidMapping>& oid_names);

enum class QueryType { Transaction, NonTransaction, None };
} // namespace sancho

#endif
