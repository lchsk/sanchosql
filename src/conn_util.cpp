#include "conn_util.hpp"

const std::string get_data_type(
    const pqxx::oid oid,
    std::unordered_map<pqxx::oid, OidMapping>& oid_names)
{
    if (oid_names.find(oid) == oid_names.end()) {
        return std::to_string(oid);
    } else {
        return oid_names[oid].data_type;
    }
}
