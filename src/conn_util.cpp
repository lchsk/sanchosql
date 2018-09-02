#include "conn_util.hpp"

namespace sancho {
const std::string
get_data_type(const pqxx::oid oid,
              std::unordered_map<pqxx::oid, sancho::OidMapping> &oid_names) {
    if (oid_names.find(oid) == oid_names.end()) {
        return std::to_string(oid);
    } else {
        return oid_names.at(oid).data_type;
    }
}
} // namespace sancho
