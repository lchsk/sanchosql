#ifndef PG_QUERIES_HPP
#define PG_QUERIES_HPP

#include <string>

namespace sancho {
namespace db {

const std::string get_columns_query(const std::string& schema_name,
                                    const std::string& table_name);
const std::string get_check_constraints_query(const std::string& schema_name,
                                              const std::string& table_name);
const std::string get_indexes_query(const std::string& schema_name,
                                    const std::string& table_name);
const std::string get_table_stats_query(const std::string& schema_name,
                                        const std::string& table_name);
}
} // namespace sancho

#endif
