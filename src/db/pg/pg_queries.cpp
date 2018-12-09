#include <sstream>

#include "pg_queries.hpp"

namespace sancho {
namespace db {

const std::string get_columns_query(const std::string& schema_name,
									const std::string& table_name) {
    std::stringstream query;

    query << "select * from information_schema.columns where table_name = "
          << "'" << table_name << "'";

    if (schema_name != "") {
        query << " and table_schema = "
              << "'" << schema_name << "'";
    }

    query << " order by ordinal_position asc";

    return query.str();
}

};
} // namespace sancho
