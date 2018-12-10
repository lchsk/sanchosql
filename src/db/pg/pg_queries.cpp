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

const std::string get_check_constraints_query(const std::string& schema_name,
                                              const std::string& table_name)
{
    std::stringstream query;

    query << R"(
        select
            pg_get_constraintdef(c.oid) as check_constraint,
            c.conname as check_name
        from
            pg_constraint c
        join
            pg_namespace n on n.oid = c.connamespace
        where

        )";

    query << "n.nspname = '" << schema_name << "'"
          << " and conrelid::regclass::text = '" << table_name << "';";

    return query.str();
}

  const std::string get_indexes_query(const std::string& schema_name,
                                      const std::string& table_name)
  {
    std::stringstream query;

    query << R"(
        select
            indexname as index_name,
            indexdef as index_definition
        from
            pg_indexes
        where
    )";

    query << "schemaname = '" << schema_name << "'"
          << " and tablename = '" << table_name << "';";

    return query.str();
  }

}
} // namespace sancho
