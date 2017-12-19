#ifndef PG_CONN_HPP
#define PG_CONN_HPP

#include <iostream>
#include <sstream>
#include <unordered_map>

#include <pqxx/pqxx>

#include "conn_util.hpp"
#include "query_result.hpp"
#include "connection_details.hpp"
#include "connections.hpp"

namespace san
{
    struct PrimaryKey
    {
        std::string column_name;
        std::string data_type;
    };

    class PostgresConnection {
    public:
        PostgresConnection(const std::shared_ptr<san::ConnectionDetails>& conn_details);
        ~PostgresConnection();

        std::shared_ptr<san::QueryResult> run_query(const std::string& query);

        std::vector<std::string> get_db_tables(const Glib::ustring& schema_name);

        std::vector<std::pair<std::string, std::string>>
        get_table_columns(const std::string& table_name);

        std::vector<std::map<std::string, std::string> >
        get_table_data(const std::string& table_name,
                       const std::vector<std::pair<std::string, std::string>>& columns);

        const std::vector<PrimaryKey>
        get_primary_key(const std::string& table_name, const std::string& schema_name) const;

        std::unique_ptr<std::vector<Glib::ustring>> get_schemas() const;

        void init_connection();

        bool is_open() const {
            return is_open_;
        };

        const std::string& error_message() const {
            return error_message_;
        };

    private:
        void load_oids();

        std::shared_ptr<san::ConnectionDetails> conn_details;

        std::unique_ptr<pqxx::connection> conn;

        std::unordered_map<pqxx::oid, san::OidMapping> oid_names;

        bool is_open_;
        std::string error_message_;
    };
}

#endif
