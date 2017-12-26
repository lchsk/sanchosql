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

        static std::shared_ptr<QueryResult>
        get(pqxx::connection& conn,
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
                 const std::string& query,
                 const std::string& columns_query);

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

        void set_status(bool p_success, const Glib::ustring& p_error_message) {
            success = p_success;
            error_message = p_error_message;
        }

        bool success;
        Glib::ustring error_message;

        // Optional query to obtain information about columns
        std::string columns_query;

        std::map<std::string, san::ColumnMetadata> columns_data;

        std::shared_ptr<std::unordered_map<pqxx::oid, san::OidMapping>> oid_names;
    };
}

#endif
