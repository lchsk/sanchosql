#include "query_result.hpp"

namespace san {
QueryResult::QueryResult()
    : success(false), error_message(Glib::ustring()), inserted_empty_row(false),
      size(0), affected_rows(0), show_results(false),
      oid_names(
          std::make_shared<std::unordered_map<pqxx::oid, san::OidMapping>>()),
      query_type(san::QueryType::None)
{
}

std::shared_ptr<QueryResult>
QueryResult::get_prepared_stmt(pqxx::connection& conn, const std::string& name,
                               const std::string& query, const std::string& arg)
{
    auto query_result = std::make_shared<QueryResult>();

    // TODO: Check if further error handling is needed

    try {
        pqxx::nontransaction work(conn);
        conn.prepare(name, query);
        auto prepared = work.prepared(name)(arg);

        query_result->run_prepared_stmt(prepared);

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
    size = result.size();
    affected_rows = result.affected_rows();

    show_results = result.columns();

    bool use_columns = columns_data.size();

    for (unsigned i = 0; i < result.columns(); i++) {
        std::string char_length = std::string();
        bool is_nullable = false;

        if (use_columns && IN_MAP(columns_data, result.column_name(i))) {
            char_length =
                columns_data[result.column_name(i)].character_maximum_length;
            is_nullable = columns_data[result.column_name(i)].is_nullable;
        }

        columns.push_back(
            san::Column(result.column_type(i), result.column_name(i),
                        san::get_data_type(result.column_type(i), *oid_names),
                        char_length, is_nullable));
    }

    for (const auto& row : result) {
        std::vector<std::string> row_data;

        for (unsigned i = 0; i < result.columns(); i++) {
            if (row[i].is_null()) {
                // Treat empty string as an SQL NULL value
                row_data.push_back("");
            } else {
                const std::string value = row[i].as<std::string>();

                if (value.empty()) {
                    row_data.push_back(san::string::EMPTY_DB_STRING);
                } else {
                    row_data.push_back(value);
                }
            }
        }

        data.push_back(row_data);
    }
}

void QueryResult::run_prepared_stmt(
    const pqxx::prepare::invocation& prepared_stmt)
{
    handle_results(prepared_stmt.exec());
}

std::shared_ptr<QueryResult> QueryResult::get(
    pqxx::connection& conn, const san::QueryType& query_type,
    const std::string& query, const std::string& columns_query,
    std::shared_ptr<std::unordered_map<pqxx::oid, san::OidMapping>>& oid_names)
{
    auto query_result = std::make_shared<QueryResult>(query_type);
    query_result->oid_names = oid_names;
    query_result->columns_query = columns_query;

    // TODO: Check if further error handling is needed

    try {
        query_result->columns_data =
            query_result->get_columns_data(conn, columns_query);
        query_result->run(conn, query_type, query, columns_query);
        query_result->set_status(true, "");
    } catch (const pqxx::sql_error& e) {
        query_result->set_status(false, e.what());
    } catch (const std::exception& e) {
        query_result->set_status(false, e.what());
    }

    return query_result;
}

void QueryResult::run(pqxx::connection& conn, const san::QueryType& query_type,
                      const std::string& query,
                      const std::string& columns_query)
{
    if (query_type == san::QueryType::Transaction) {
        transaction = std::make_unique<pqxx::work>(conn);
        handle_results(transaction->exec(query));
    } else if (query_type == san::QueryType::NonTransaction) {
        pqxx::nontransaction work(conn);
        handle_results(work.exec(query));
    } else {
        throw std::invalid_argument("Invalid query type");
    }
}
} // namespace san
