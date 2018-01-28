#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include <unordered_map>

#include <pqxx/pqxx>

#include "conn_util.hpp"
#include "string.hpp"

namespace san
{
    struct QueryResult
    {
        QueryResult();
        QueryResult(const san::QueryType& query_type) : query_type(query_type) {};

        ~QueryResult() {
            // In theory that should never happen but if the transaction is left
            // unfinished, explicitly roll it back.
            if (query_type == san::QueryType::Transaction && transaction) {
                g_debug("Destroying san::QueryResult - rolling back");
                rollback();
            }
        }

        static std::shared_ptr<QueryResult>
        get() {
            return std::make_shared<QueryResult>();
        }

        static std::shared_ptr<QueryResult>
        get(const bool success) {
            auto result = std::make_shared<QueryResult>();
            result->success = success;

            return result;
        }

        static std::shared_ptr<QueryResult>
        get(pqxx::connection& conn,
            const san::QueryType& query_type,
            const std::string& query,
            const std::string& columns_query,
            std::shared_ptr<std::unordered_map<pqxx::oid, san::OidMapping>>& oid_names);

        std::vector<san::Column> columns;
        std::vector<std::vector<std::string> > data;

        // Return data where each row is a map column name -> value
        std::vector<std::map<std::string, std::string>>
        as_map() const {
            std::vector<std::map<std::string, std::string>> v;

            for (unsigned i = 0; i < data.size(); i++) {
                std::map<std::string, std::string> m;

                for (unsigned j = 0; j < columns.size(); j++) {
                    m[columns[j].column_name] = data[i][j];
                }

                v.push_back(m);
            }

            return v;
        }

        static std::shared_ptr<QueryResult>
        get_prepared_stmt(pqxx::connection& conn,
                          const std::string& name,
                          const std::string& query,
                          const std::string& arg);

        void run_prepared_stmt(const pqxx::prepare::invocation& prepared_stmt);

        void handle_results(const pqxx::result&);

        void run(pqxx::connection& conn,
                 const san::QueryType& query_type,
                 const std::string& query,
                 const std::string& columns_query);

        const std::map<std::string, san::ColumnMetadata>
        get_columns_data(pqxx::connection& conn, const std::string& columns_query) const  {
			// TODO: Make sure only SELECT can be run here
            pqxx::nontransaction work(conn);
            pqxx::result result = work.exec(columns_query);
            work.commit();

            std::map<std::string, san::ColumnMetadata> columns;

            // TODO: Handle cases when 'row' does not contain the columns mentioned below
            for (const auto& row : result) {
                const auto column_name = row["column_name"].as<std::string>();

                const auto character_maximum_length = row["character_maximum_length"].is_null() ? "" : row["character_maximum_length"].as<std::string>();
                bool is_nullable = false;

                if (! row["is_nullable"].is_null() && row["is_nullable"].as<std::string>() == "YES") {
                    is_nullable = true;
                }

                columns[column_name] = san::ColumnMetadata(character_maximum_length, is_nullable);
            }

            return columns;
        }

        void set_status(bool p_success, const Glib::ustring& p_error_message) {
            success = p_success;
            error_message = p_error_message;
        }

        void commit() noexcept {
            if (query_type == san::QueryType::Transaction && transaction) {
                try {
                    transaction->commit();
                    transaction.reset();
                    g_assert(transaction == nullptr);
                    g_debug("Transaction committed");
                } catch (const std::exception& e) {
                    g_warning("Commit failed: %s", e.what());
                    set_status(false, Glib::ustring("Commit failed, attempting to rollback: ") + Glib::ustring(e.what()));

                    rollback();
                }
            }
        }

        void rollback() noexcept {
            if (query_type == san::QueryType::Transaction && transaction) {
                try {
                    transaction->abort();
                    transaction.reset();
                    g_assert(transaction == nullptr);
                    g_debug("Transaction rolled back");
                } catch (const std::exception& e) {
                    g_warning("Rollback failed: %s", e.what());
                    set_status(false, Glib::ustring("Rollback failed: ") + Glib::ustring(e.what()));
                }
            }
        }

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

        std::map<std::string, san::ColumnMetadata> columns_data;

        std::shared_ptr<std::unordered_map<pqxx::oid, san::OidMapping>> oid_names;

        std::unique_ptr<pqxx::work> transaction;
        const san::QueryType query_type;
    };
}

#endif
