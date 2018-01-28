#ifndef CONN_UTIL_HPP
#define CONN_UTIL_HPP

#include <pqxx/pqxx>

#include <unordered_map>

namespace san {
class OidMapping {
  public:
    OidMapping(const pqxx::oid& oid, const std::string& udt_name,
               const std::string& data_type)
        : oid(oid), udt_name(udt_name), data_type(data_type)
    {
    }

    // TODO: Refactor so it's not needed
    OidMapping() {}

    pqxx::oid oid;
    std::string udt_name;
    std::string data_type;
};

class Column {
  public:
    Column(const pqxx::oid& oid, const std::string& column_name,
           const std::string& data_type, const std::string& char_length,
           bool is_nullable)
        : oid(oid), column_name(column_name), data_type(data_type),
          char_length(char_length), is_nullable(is_nullable)
    {
    }

    pqxx::oid oid;
    std::string column_name;
    std::string data_type;
    std::string char_length;
    bool is_nullable;
};

class ColumnMetadata {
  public:
    ColumnMetadata(const std::string& character_maximum_length,
                   bool is_nullable)
        : character_maximum_length(character_maximum_length),
          is_nullable(is_nullable){};

    // TODO: Refactor so it's not needed
    ColumnMetadata() {}

    std::string character_maximum_length;
    bool is_nullable;
};

const std::string
get_data_type(const pqxx::oid oid,
              std::unordered_map<pqxx::oid, san::OidMapping>& oid_names);

enum class QueryType { Transaction, NonTransaction, None };
} // namespace san

#endif
