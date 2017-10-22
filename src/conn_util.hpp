#ifndef CONN_UTIL_HPP
#define CONN_UTIL_HPP

#include <pqxx/pqxx>

#include <unordered_map>

struct OidMapping
{
    pqxx::oid oid;
    std::string udt_name;
    std::string data_type;
};

struct Column
{
    pqxx::oid oid;
    std::string column_name;
    std::string data_type;
};

const std::string get_data_type(
    const pqxx::oid oid,
    std::unordered_map<pqxx::oid, OidMapping>& oid_names);

#endif
