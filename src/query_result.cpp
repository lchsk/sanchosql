#include "query_result.hpp"

QueryResult::QueryResult(pqxx::connection& conn,
                const std::string& query,
                std::unordered_map<pqxx::oid, OidMapping>& oid_names)
    {
        pqxx::work work(conn);
        pqxx::result result = work.exec(query);

        for (unsigned i = 0; i < result.columns(); i++) {
            columns.push_back(Column({
                        .oid = result.column_type(i),
                            .column_name = result.column_name(i),
                            .data_type = get_data_type(result.column_type(i), oid_names)
                            }));
        }

        for (const auto& row : result) {
            std::vector<std::string> row_data;

            for (unsigned i = 0; i < result.columns(); i++) {
                if (row[i].is_null()) {
                    row_data.push_back("null");
                } else {
                    row_data.push_back(row[i].as<std::string>());
                }
            }

            data.push_back(row_data);
        }
    }
