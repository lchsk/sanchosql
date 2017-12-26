#include "query_result.hpp"

namespace san
{
    QueryResult::QueryResult()
        : success(false),
          error_message(Glib::ustring())
    {
    }

    std::shared_ptr<QueryResult>
    QueryResult::get(pqxx::connection& conn,
                     const std::string& query,
                     const std::string& columns_query,
                     std::unordered_map<pqxx::oid, san::OidMapping>& oid_names)
    {
        auto query_result = std::make_shared<QueryResult>();

        // TODO: Check if further error handling is needed

        try {
            query_result->run(conn, query, columns_query, oid_names);
        } catch (const pqxx::sql_error& e) {
            query_result->set_status(false, e.what());
        } catch (const std::exception& e) {
            query_result->set_status(false, e.what());
        }

        return query_result;
    }

    void QueryResult::run(pqxx::connection& conn,
                          const std::string& query,
                          const std::string& columns_query,
                          std::unordered_map<pqxx::oid, san::OidMapping>& oid_names)
    {
        pqxx::nontransaction work(conn);
        pqxx::result result = work.exec(query);
        work.commit();

        std::map<std::string, san::ColumnMetadata> columns_data = get_columns_data(conn, columns_query);

        for (unsigned i = 0; i < result.columns(); i++) {
            std::string char_length;
            bool is_nullable;

            if (IN_MAP(columns_data, result.column_name(i))) {
                char_length = columns_data[result.column_name(i)].character_maximum_length;
                is_nullable = columns_data[result.column_name(i)].is_nullable;
            }

            columns.push_back(san::Column({
                        .oid = result.column_type(i),
                            .column_name = result.column_name(i),
                            .data_type = san::get_data_type(result.column_type(i), oid_names),
                            .char_length = char_length,
                            .is_nullable = is_nullable
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
}
