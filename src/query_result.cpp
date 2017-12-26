#include "query_result.hpp"

namespace san
{
    QueryResult::QueryResult()
        : success(false),
          error_message(Glib::ustring()),
          oid_names(std::make_shared<std::unordered_map<pqxx::oid, san::OidMapping>>())
    {
    }

    std::shared_ptr<QueryResult>
    QueryResult::get_prepared_stmt(const pqxx::prepare::invocation& prepared_stmt)
    {
        auto query_result = std::make_shared<QueryResult>();

        // TODO: Check if further error handling is needed

        try {
            query_result->run_prepared_stmt(prepared_stmt);
            query_result->set_status(true, "");
        } catch (const pqxx::sql_error& e) {
            query_result->set_status(false, e.what());
        } catch (const std::exception& e) {
            query_result->set_status(false, e.what());
        }

        return query_result;
    }

    void QueryResult::handle_results(const pqxx::result& result)
    {
        bool use_columns = columns_data.size();

        for (unsigned i = 0; i < result.columns(); i++) {
            std::string char_length = std::string();
            bool is_nullable = false;

            if (use_columns && IN_MAP(columns_data, result.column_name(i))) {
                char_length = columns_data[result.column_name(i)].character_maximum_length;
                is_nullable = columns_data[result.column_name(i)].is_nullable;
            }

            columns.push_back(san::Column({
                        .oid = result.column_type(i),
                            .column_name = result.column_name(i),
                            .data_type = san::get_data_type(result.column_type(i), *oid_names),
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

    void QueryResult::run_prepared_stmt(const pqxx::prepare::invocation& prepared_stmt)
    {
        handle_results(prepared_stmt.exec());
    }

    std::shared_ptr<QueryResult>
    QueryResult::get(pqxx::connection& conn,
                     const std::string& query,
                     const std::string& columns_query,
                     std::shared_ptr<std::unordered_map<pqxx::oid, san::OidMapping>>& oid_names)
    {
        auto query_result = std::make_shared<QueryResult>();
        query_result->oid_names = oid_names;
        query_result->columns_query = columns_query;

        // TODO: Check if further error handling is needed

        try {
            query_result->columns_data = query_result->get_columns_data(conn, columns_query);
            query_result->run(conn, query, columns_query);
            query_result->set_status(true, "");
        } catch (const pqxx::sql_error& e) {
            query_result->set_status(false, e.what());
        } catch (const std::exception& e) {
            query_result->set_status(false, e.what());
        }

        return query_result;
    }

    void QueryResult::run(pqxx::connection& conn,
                          const std::string& query,
                          const std::string& columns_query)
    {
        pqxx::nontransaction work(conn);
        pqxx::result result = work.exec(query);
        work.commit();

        handle_results(result);
    }
}
