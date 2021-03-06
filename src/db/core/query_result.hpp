#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include <unordered_map>

#include <pqxx/pqxx>

#include "../../string.hpp"
#include "conn_util.hpp"

namespace sancho {
struct QueryResult {
    QueryResult();
    QueryResult(const sancho::QueryType& query_type) : query_type(query_type){};

    ~QueryResult();

    static std::shared_ptr<QueryResult> get();
    static std::shared_ptr<QueryResult> get(const bool success);

    static std::shared_ptr<QueryResult>
    get(pqxx::connection& conn, const sancho::QueryType& query_type,
        const std::string& query, const std::string& columns_query,
        std::shared_ptr<std::unordered_map<pqxx::oid, sancho::OidMapping>>&
            oid_names);

    std::vector<sancho::db::Column> columns;
    std::vector<std::vector<std::string>> data;

    // Return data where each row is a map column name -> value
    std::vector<std::map<std::string, std::string>> as_map() const;

    static std::shared_ptr<QueryResult>
    get_prepared_stmt(pqxx::connection& conn, const std::string& name,
                      const std::string& query, const std::string& arg);

    void run_prepared_stmt(const pqxx::prepare::invocation& prepared_stmt);

    void handle_results(const pqxx::result&);

    void run(pqxx::connection& conn, const sancho::QueryType& query_type,
             const std::string& query, const std::string& columns_query);

    const std::map<std::string, sancho::db::ColumnMetadata>
    get_columns_data(pqxx::connection& conn,
                     const std::string& columns_query) const;

    void set_status(bool p_success, const Glib::ustring& p_error_message);

    void commit() noexcept;
    void rollback() noexcept;

    const std::string get_message() const;

    bool success;
    Glib::ustring error_message;
    bool inserted_empty_row;

    // Optional query to obtain information about columns
    std::string columns_query;

    pqxx::result::size_type size;

    // Only for INSERT, UPDATE, DELETE
    pqxx::result::size_type affected_rows;

    // True if we need to show results of the query.
    // It does not necessarily mean we have > 1 row - it means that it was
    // some kind of SELECT query that didn't error.
    bool show_results;

    std::map<std::string, sancho::db::ColumnMetadata> columns_data;

    std::shared_ptr<std::unordered_map<pqxx::oid, sancho::OidMapping>>
        oid_names;

    std::unique_ptr<pqxx::work> transaction;
    const sancho::QueryType query_type;
};
} // namespace sancho

#endif
