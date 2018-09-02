#include "query_result.hpp"

namespace sancho {
QueryResult::QueryResult()
    : success(false), error_message(Glib::ustring()), inserted_empty_row(false),
      size(0), affected_rows(0), show_results(false),
      oid_names(std::make_shared<
                std::unordered_map<pqxx::oid, sancho::OidMapping>>()),
      query_type(sancho::QueryType::None) {}

QueryResult::~QueryResult() {
    // In theory that should never happen but if the transaction is left
    // unfinished, explicitly roll it back.
    if (query_type == sancho::QueryType::Transaction && transaction) {
        g_debug("Destroying sancho::QueryResult - rolling back");
        rollback();
    }
}

std::shared_ptr<QueryResult> QueryResult::get() {
    return std::make_shared<QueryResult>();
}

std::shared_ptr<QueryResult> QueryResult::get(const bool success) {
    auto result = std::make_shared<QueryResult>();
    result->success = success;

    return result;
}

std::vector<std::map<std::string, std::string>> QueryResult::as_map() const {
    std::vector<std::map<std::string, std::string>> v;

    for (unsigned i = 0; i < data.size(); i++) {
        std::map<std::string, std::string> m;

        for (unsigned j = 0; j < columns.size(); j++) {
            m[columns[j].get_column_name()] = data[i][j];
        }

        v.push_back(m);
    }

    return v;
}

    const std::map<std::string, sancho::db::ColumnMetadata>
QueryResult::get_columns_data(pqxx::connection &conn,
                              const std::string &columns_query) const {
    // TODO: Make sure only SELECT can be run here
    pqxx::nontransaction work(conn);
    pqxx::result result = work.exec(columns_query);
    work.commit();

    std::map<std::string, sancho::db::ColumnMetadata> columns;

    // TODO: Handle cases when 'row' does not contain the columns mentioned
    // below
    for (const auto &row : result) {
        const auto column_name = row["column_name"].as<std::string>();

        const auto character_maximum_length =
            row["character_maximum_length"].is_null()
                ? ""
                : row["character_maximum_length"].as<std::string>();
        bool is_nullable = false;

        if (!row["is_nullable"].is_null() &&
            row["is_nullable"].as<std::string>() == "YES") {
            is_nullable = true;
        }

        columns.emplace(column_name, sancho::db::ColumnMetadata(character_maximum_length, is_nullable));
    }

    return columns;
}

void QueryResult::set_status(bool p_success,
                             const Glib::ustring &p_error_message) {
    success = p_success;
    error_message = p_error_message;
}

void QueryResult::commit() noexcept {
    if (query_type == sancho::QueryType::Transaction && transaction) {
        try {
            transaction->commit();
            transaction.reset();
            g_assert(transaction == nullptr);
            g_debug("Transaction committed");
        } catch (const std::exception &e) {
            g_warning("Commit failed: %s", e.what());
            set_status(
                false,
                Glib::ustring("Commit failed, attempting to rollback: ") +
                    Glib::ustring(e.what()));

            rollback();
        }
    }
}

void QueryResult::rollback() noexcept {
    if (query_type == sancho::QueryType::Transaction && transaction) {
        try {
            transaction->abort();
            transaction.reset();
            g_assert(transaction == nullptr);
            g_debug("Transaction rolled back");
        } catch (const std::exception &e) {
            g_warning("Rollback failed: %s", e.what());
            set_status(false,
                       Glib::ustring("Rollback failed: ") +
                           Glib::ustring(e.what()));
        }
    }
}

const std::string QueryResult::get_message() const {
    if (!success)
        return error_message;

    std::stringstream message;
    message << "Query ";

    if (show_results) {
        message << "returned " << size;

        if (size == 1) {
            message << " row";
        } else {
            message << " rows";
        }
    } else {
        message << "affected " << affected_rows;

        if (affected_rows == 1) {
            message << " row";
        } else {
            message << " rows";
        }
    }

    message << "\n";

    return message.str();
}

std::shared_ptr<QueryResult>
QueryResult::get_prepared_stmt(pqxx::connection &conn, const std::string &name,
                               const std::string &query,
                               const std::string &arg) {
    auto query_result = std::make_shared<QueryResult>();

    // TODO: Check if further error handling is needed

    try {
        pqxx::nontransaction work(conn);
        conn.prepare(name, query);
        auto prepared = work.prepared(name)(arg);

        query_result->run_prepared_stmt(prepared);

        query_result->set_status(true, "");
    } catch (const pqxx::sql_error &e) {
        query_result->set_status(false, e.what());
    } catch (const std::exception &e) {
        query_result->set_status(false, e.what());
    }

    return query_result;
}

void QueryResult::handle_results(const pqxx::result &result) {
    size = result.size();
    affected_rows = result.affected_rows();

    show_results = result.columns();

    bool use_columns = columns_data.size();

    for (unsigned i = 0; i < result.columns(); i++) {
        std::string char_length = std::string();
        bool is_nullable = false;

        if (use_columns && IN_MAP(columns_data, result.column_name(i))) {
            char_length =
                columns_data.at(result.column_name(i)).get_character_maximum_length();
            is_nullable = columns_data.at(result.column_name(i)).get_is_nullable();
        }

        columns.push_back(sancho::db::Column(
            result.column_name(i),
            sancho::get_data_type(result.column_type(i), *oid_names),
            char_length, is_nullable));
    }

    for (const auto &row : result) {
        std::vector<std::string> row_data;

        for (unsigned i = 0; i < result.columns(); i++) {
            if (row[i].is_null()) {
                // Treat empty string as an SQL NULL value
                row_data.push_back("");
            } else {
                const std::string value = row[i].as<std::string>();

                if (value.empty()) {
                    row_data.push_back(sancho::string::EMPTY_DB_STRING);
                } else {
                    row_data.push_back(value);
                }
            }
        }

        data.push_back(row_data);
    }
}

void QueryResult::run_prepared_stmt(
    const pqxx::prepare::invocation &prepared_stmt) {
    handle_results(prepared_stmt.exec());
}

std::shared_ptr<QueryResult> QueryResult::get(
    pqxx::connection &conn, const sancho::QueryType &query_type,
    const std::string &query, const std::string &columns_query,
    std::shared_ptr<std::unordered_map<pqxx::oid, sancho::OidMapping>>
        &oid_names) {
    auto query_result = std::make_shared<QueryResult>(query_type);
    query_result->oid_names = oid_names;
    query_result->columns_query = columns_query;

    // TODO: Check if further error handling is needed

    try {
        query_result->columns_data =
            query_result->get_columns_data(conn, columns_query);
        query_result->run(conn, query_type, query, columns_query);
        query_result->set_status(true, "");
    } catch (const pqxx::sql_error &e) {
        query_result->set_status(false, e.what());
    } catch (const std::exception &e) {
        query_result->set_status(false, e.what());
    }

    return query_result;
}

void QueryResult::run(pqxx::connection &conn,
                      const sancho::QueryType &query_type,
                      const std::string &query,
                      const std::string &columns_query) {
    if (query_type == sancho::QueryType::Transaction) {
        transaction = std::make_unique<pqxx::work>(conn);
        handle_results(transaction->exec(query));
    } else if (query_type == sancho::QueryType::NonTransaction) {
        pqxx::nontransaction work(conn);
        handle_results(work.exec(query));
    } else {
        throw std::invalid_argument("Invalid query type");
    }
}
} // namespace sancho
