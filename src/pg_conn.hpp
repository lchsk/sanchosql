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

        PrimaryKey(const std::string& column_name, const std::string& data_type)
            : column_name(column_name),
              data_type(data_type) {}
    };

    class NoConnection : public std::runtime_error
    {
    public:
        NoConnection(const std::string& what) : std::runtime_error(what) {}
    };

    class PostgresConnection {
    public:
        explicit PostgresConnection(const std::shared_ptr<san::ConnectionDetails>& conn_details);
        virtual ~PostgresConnection();

        virtual std::shared_ptr<san::QueryResult> run_query(const std::string& query, const std::string& columns_query);
        virtual std::shared_ptr<san::QueryResult> run_query(const std::string& query);

        std::vector<std::string> get_db_tables(const Glib::ustring& schema_name) const noexcept;

        virtual const std::vector<PrimaryKey>
        get_primary_key(const std::string& table_name, const std::string& schema_name) const noexcept;

        std::unique_ptr<std::vector<Glib::ustring>> get_schemas();

        virtual void init_connection();

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

        bool is_open_;
        std::string error_message_;

        std::shared_ptr<std::unordered_map<pqxx::oid, san::OidMapping>> oid_names;
    };
}

#endif
