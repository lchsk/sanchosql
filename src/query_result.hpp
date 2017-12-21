#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include <unordered_map>

#include <pqxx/pqxx>

#include "conn_util.hpp"
#include "util.hpp"

namespace san
{
    struct QueryResult
    {
        QueryResult(pqxx::connection& conn,
                    const std::string& query,
                    const std::string& columns_query,
                    std::unordered_map<pqxx::oid, san::OidMapping>& oid_names);

        std::vector<san::Column> columns;
        std::vector<std::vector<std::string> > data;

        const std::map<std::string, san::ColumnMetadata>
        get_columns_data(pqxx::connection& conn, const std::string& columns_query) const  {
            pqxx::nontransaction work(conn);
            pqxx::result result = work.exec(columns_query);
            work.commit();

            std::map<std::string, san::ColumnMetadata> columns;

            for (const auto& row : result) {
                const auto column_name = row["column_name"].as<std::string>();

                const auto character_maximum_length = row["character_maximum_length"].is_null() ? "" : row["character_maximum_length"].as<std::string>();
                bool is_nullable = false;

                if (! row["is_nullable"].is_null() && row["is_nullable"].as<std::string>() == "YES") {
                    is_nullable = true;
                }

                columns[column_name] = san::ColumnMetadata({
                        .character_maximum_length = character_maximum_length,
                            .is_nullable = is_nullable
                            });

            }

            return columns;
        }
    };
}

#endif
