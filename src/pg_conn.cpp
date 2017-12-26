#include <iostream>

#include "pg_conn.hpp"

namespace san
{
    san::Connections san::Connections::ins;

    PostgresConnection::PostgresConnection
    (const std::shared_ptr<san::ConnectionDetails>& conn_details) :
        conn_details(conn_details),
        is_open_(false),
        error_message_("")
    {
    }

    PostgresConnection::~PostgresConnection()
    {
        if (! conn) return;

        try {
            if (conn->is_open())
                conn->disconnect();
        } catch (const std::exception& e) {
            std::cerr << "Error when disconnecting: " << e.what() << std::endl;
        }
    }

    std::shared_ptr<san::QueryResult>
    PostgresConnection::run_query(const std::string& query, const std::string& columns_query)
    {
        g_debug("Executing query: %s", query.c_str());

        if (! columns_query.empty()) {
            g_debug("Executing columns query: %s", columns_query.c_str());
        }

        return std::make_shared<san::QueryResult>(*conn, query, columns_query, oid_names);
    }

    std::shared_ptr<san::QueryResult>
    PostgresConnection::run_query(const std::string& query)
    {
        return run_query(query, "");
    }

    std::vector<std::string> PostgresConnection::get_db_tables(const Glib::ustring& schema_name)
    {
        std::vector<std::string> tables;

        pqxx::work work(*conn);

        const std::string sql = R"(
            SELECT
                *
            FROM
                information_schema.tables
            WHERE
                table_schema = $1
            ORDER BY
                table_name ASC
            )";

        conn->prepare("get_db_tables", sql);
        pqxx::result result = work.prepared("get_db_tables")(std::string(schema_name)).exec();

        for (const auto& row : result) {
            tables.push_back(row["table_name"].as<std::string>());
        }

        return tables;
    }

    std::vector<std::pair<std::string, std::string>>
    PostgresConnection::get_table_columns(const std::string& table_name)
    {
        // TODO: Remove if no longer needed
        // Otherwise, modify to use schema

        std::vector<std::pair<std::string, std::string>> columns;

        pqxx::work work(*conn);

        const std::string sql = R"(
            SELECT
                *
            FROM
                information_schema.columns
            WHERE
                table_schema = 'public'
                AND table_name = $1
            ORDER BY
                ordinal_position ASC
            )";

        conn->prepare("get_columns", sql);
        pqxx::result result = work.prepared("get_columns")(table_name).exec();

        for (const auto& row : result) {
            columns.push_back(std::make_pair<std::string, std::string>
                              (row["column_name"].as<std::string>(),
                               row["data_type"].as<std::string>()));
        }

        return columns;
    }

    std::vector<std::map<std::string, std::string> >
    PostgresConnection::get_table_data(
                                       const std::string& table_name,
                                       const std::vector<std::pair<std::string, std::string>>& columns)
    {
        std::vector<std::map<std::string, std::string> > data;

        pqxx::work work(*conn);

        const std::string sql = "select * from " + table_name;

        pqxx::result result = work.exec(sql);

        for (const auto& row : result) {
            std::map<std::string, std::string> v;

            for (const auto& col_name : columns) {
                try {
                    v[col_name.first] = row[col_name.first].as<std::string>();
                } catch (const std::exception&) {
                    v[col_name.first] = "null";
                }
            }

            data.push_back(v);
        }

        return data;
    }

    std::unique_ptr<std::vector<Glib::ustring>> PostgresConnection::get_schemas() const
    {
        std::unique_ptr<std::vector<Glib::ustring>> schemas = std::make_unique<std::vector<Glib::ustring>>();

        pqxx::work work(*conn);

        const std::string sql = R"(SELECT nspname FROM pg_catalog.pg_namespace;)";

        pqxx::result result = work.exec(sql);

        for (const auto& row : result) {
            schemas->push_back(row["nspname"].as<std::string>());
        }

        std::sort(schemas->begin(), schemas->end());

        return std::move(schemas);
    }

    const std::vector<PrimaryKey>
    PostgresConnection::get_primary_key(const std::string& table_name, const std::string& schema_name) const
    {
        std::vector<PrimaryKey> data;

        pqxx::work work(*conn);

        const std::string sql = R"(
            SELECT
                a.attname as column_name,
                format_type(a.atttypid, a.atttypmod) AS data_type
            FROM pg_index i
            JOIN pg_attribute a ON a.attrelid = i.indrelid
                AND a.attnum = ANY(i.indkey)
            WHERE i.indrelid = $1::regclass
                AND i.indisprimary;)";

        conn->prepare("get_primary_key", sql);
        const std::string id = schema_name + "." + table_name;
        pqxx::result result = work.prepared("get_primary_key")(id).exec();

        for (const auto& row : result) {
            data.push_back(PrimaryKey({
                .column_name=row["column_name"].as<std::string>(),
                .data_type=row["data_type"].as<std::string>()
            }));
        }

        return data;
    }

    void PostgresConnection::init_connection()
    {
        g_debug("Initiating postgres connection: %s", conn_details->postgres_string_safe().c_str());

        try {
            conn = std::make_unique<pqxx::connection>(conn_details->postgres_connection_string());

            error_message_ = "";
            is_open_ = conn->is_open();

            load_oids();
        } catch (const std::exception& e) {
            g_warning("Postgres connection failed: %s", e.what());

            error_message_ = e.what();
            is_open_ = false;

            throw san::NoConnection(e.what());
        }
    }

    void PostgresConnection::load_oids()
    {
        pqxx::work work(*conn);

        const std::string sql = R"(
        select
            distinct udt_name, data_type, t.oid
        from
            information_schema.columns c
        join pg_type t
            on t.typname = c.udt_name
    )";

        pqxx::result result = work.exec(sql);

        for (const auto& row : result) {
            oid_names[row["oid"].as<pqxx::oid>()] = san::OidMapping({
                    .oid=row["oid"].as<pqxx::oid>(),
                        .udt_name=row["udt_name"].as<std::string>(),
                        .data_type=row["data_type"].as<std::string>(),
                        });
        }
    }
}
