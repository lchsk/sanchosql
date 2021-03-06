#include <iostream>

#include "pg_conn.hpp"

namespace sancho {
namespace db {
sancho::db::Connections sancho::db::Connections::ins;

PostgresConnection::PostgresConnection(
    const std::shared_ptr<sancho::db::ConnectionDetails>& conn_details)
    : conn_details(conn_details), is_open_(false), error_message_(""),
      oid_names(std::make_shared<
                std::unordered_map<pqxx::oid, sancho::OidMapping>>()) {}

PostgresConnection::~PostgresConnection() {
    if (!conn)
        return;

    try {
        if (conn->is_open())
            conn->disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Error when disconnecting: " << e.what() << std::endl;
    }
}

std::shared_ptr<sancho::QueryResult>
PostgresConnection::run_query(const sancho::QueryType& query_type,
                              const std::string& query,
                              const std::string& columns_query) {
    g_debug("Executing query: %s", query.c_str());

    if (!columns_query.empty()) {
        g_debug("Executing columns query: %s", columns_query.c_str());
    }

    return sancho::QueryResult::get(*conn, query_type, query, columns_query,
                                    oid_names);
}

std::shared_ptr<sancho::QueryResult>
PostgresConnection::run_query(const sancho::QueryType& query_type,
                              const std::string& query) {
    return run_query(query_type, query, "");
}

std::vector<std::string>
PostgresConnection::get_db_tables(const Glib::ustring& schema_name) const
    noexcept {
    std::vector<std::string> tables;

    const std::string sql = R"(
            SELECT
                table_name
            FROM
                information_schema.tables
            WHERE
                table_schema = $1
                AND table_type = 'BASE TABLE'
            ORDER BY
                table_name ASC
            )";

    auto query_result = sancho::QueryResult::get_prepared_stmt(
        *conn, "get_db_tables", sql, schema_name);

    if (!query_result->success)
        return tables;

    for (auto& row : query_result->as_map()) {
        tables.push_back(row["table_name"]);
    }

    return tables;
}

std::vector<std::string>
PostgresConnection::get_db_views(const Glib::ustring& schema_name) noexcept {
    std::vector<std::string> views;

    const std::string sql_template = R"(
            SELECT
                table_name
            FROM
                information_schema.views
            WHERE
                table_schema = '%1'
            ORDER BY
                table_name ASC
            )";

    const auto sql = Glib::ustring::compose(sql_template, schema_name);

    const std::shared_ptr<sancho::QueryResult> result =
        run_query(sancho::QueryType::NonTransaction, sql);

    if (!result->success)
        return views;

    for (auto& row : result->as_map()) {
        views.push_back(row["table_name"]);
    }

    return views;
}

const std::string PostgresConnection::get_db_view_query(
    const Glib::ustring& schema_name,
    const Glib::ustring& table_name) noexcept {
    const std::string sql_template = R"(
            SELECT
                view_definition
            FROM
                information_schema.views
            WHERE
                table_schema = '%1'
                AND table_name = '%2'
            )";

    const auto sql =
        Glib::ustring::compose(sql_template, schema_name, table_name);

    const std::shared_ptr<sancho::QueryResult> result =
        run_query(sancho::QueryType::NonTransaction, sql);

    if (!result->success)
        return "";

    if (result->size != 1) {
        return "";
    }

    const auto data = result->as_map();

    return data[0].at("view_definition");
}

std::vector<std::string>
PostgresConnection::get_db_triggers(const Glib::ustring& schema_name) noexcept {
    std::vector<std::string> triggers;

    const std::string sql_template = R"(
            SELECT
                trigger_name
            FROM
                information_schema.triggers
            WHERE
                trigger_schema = '%1'
            ORDER BY
                trigger_name ASC
            )";

    const auto sql = Glib::ustring::compose(sql_template, schema_name);

    const std::shared_ptr<sancho::QueryResult> result =
        run_query(sancho::QueryType::NonTransaction, sql);

    if (!result->success)
        return triggers;

    for (auto& row : result->as_map()) {
        triggers.push_back(row["trigger_name"]);
    }

    return triggers;
}

std::vector<std::string> PostgresConnection::get_db_functions(
    const Glib::ustring& schema_name) noexcept {
    std::vector<std::string> functions;

    const std::string sql_template = R"(
            SELECT
                routine_name
            FROM
                information_schema.routines
            WHERE
                specific_schema = '%1'
            ORDER BY
                routine_name ASC
            )";

    const auto sql = Glib::ustring::compose(sql_template, schema_name);

    const std::shared_ptr<sancho::QueryResult> result =
        run_query(sancho::QueryType::NonTransaction, sql);

    if (!result->success)
        return functions;

    for (auto& row : result->as_map()) {
        functions.push_back(row["routine_name"]);
    }

    return functions;
}

std::vector<std::string> PostgresConnection::get_db_sequences(
    const Glib::ustring& schema_name) noexcept {
    std::vector<std::string> sequences;

    const std::string sql_template = R"(
            SELECT
                sequence_name
            FROM
                information_schema.sequences
            WHERE
                sequence_schema = '%1'
            ORDER BY
                sequence_name ASC
            )";

    const auto sql = Glib::ustring::compose(sql_template, schema_name);

    const std::shared_ptr<sancho::QueryResult> result =
        run_query(sancho::QueryType::NonTransaction, sql);

    if (!result->success)
        return sequences;

    for (auto& row : result->as_map()) {
        sequences.push_back(row["sequence_name"]);
    }

    return sequences;
}

const std::string PostgresConnection::get_db_function_definition(
    const Glib::ustring& schema_name,
    const Glib::ustring& routine_name) noexcept {
    const std::string sql_template = R"(
            SELECT
                routine_definition
            FROM
                information_schema.routines
            WHERE
                routine_schema = '%1'
                AND routine_name = '%2'
            )";

    const auto sql =
        Glib::ustring::compose(sql_template, schema_name, routine_name);

    const std::shared_ptr<sancho::QueryResult> result =
        run_query(sancho::QueryType::NonTransaction, sql);

    if (!result->success)
        return "";

    if (result->size != 1) {
        return "";
    }

    const auto data = result->as_map();

    return data[0].at("routine_definition");
}

std::unique_ptr<std::vector<Glib::ustring>> PostgresConnection::get_schemas() {
    std::unique_ptr<std::vector<Glib::ustring>> schemas =
        std::make_unique<std::vector<Glib::ustring>>();

    const std::string sql = R"(SELECT nspname FROM pg_catalog.pg_namespace;)";

    // TODO: Remove oid_names - it is not used here
    auto query_result = sancho::QueryResult::get(
        *conn, sancho::QueryType::NonTransaction, sql, "", oid_names);

    if (!query_result->success)
        return std::move(schemas);

    for (auto& row : query_result->as_map()) {
        schemas->push_back(row["nspname"]);
    }

    std::sort(schemas->begin(), schemas->end());

    return std::move(schemas);
}

const std::vector<PrimaryKey>
PostgresConnection::get_primary_key(const std::string& table_name,
                                    const std::string& schema_name) const
    noexcept {
    std::vector<PrimaryKey> data;

    const std::string sql = R"(
            SELECT
                a.attname as column_name,
                format_type(a.atttypid, a.atttypmod) AS data_type
            FROM pg_index i
            JOIN pg_attribute a ON a.attrelid = i.indrelid
                AND a.attnum = ANY(i.indkey)
            WHERE i.indrelid = $1::regclass
                AND i.indisprimary;)";

    const std::string id = schema_name + "." + table_name;

    auto query_result = sancho::QueryResult::get_prepared_stmt(
        *conn, "get_primary_key", sql, id);

    if (!query_result->success)
        return data;

    for (auto& row : query_result->as_map()) {
        data.push_back(PrimaryKey(row["column_name"], row["data_type"]));
    }

    return data;
}

void PostgresConnection::init_connection() {
    g_debug("Initiating postgres connection: %s",
            conn_details->postgres_string_safe().c_str());

    try {
        conn = std::make_unique<pqxx::connection>(
            conn_details->postgres_connection_string());

        error_message_ = "";
        is_open_ = conn->is_open();

        load_oids();
    } catch (const std::exception& e) {
        g_warning("Postgres connection failed: %s", e.what());

        error_message_ = e.what();
        is_open_ = false;

        throw sancho::db::NoConnection(e.what());
    }
}

void PostgresConnection::load_oids() {
    const std::string sql = R"(
            SELECT
                DISTINCT udt_name, data_type, t.oid
            FROM
                information_schema.columns c
            JOIN pg_type t
                on t.typname = c.udt_name
        )";

    auto query_result = sancho::QueryResult::get(
        *conn, sancho::QueryType::NonTransaction, sql, "", oid_names);

    if (!query_result->success)
        return;

    for (auto& row : query_result->as_map()) {
        const pqxx::oid oid = std::atoi(row["oid"].c_str());

        oid_names->emplace(
            oid, sancho::OidMapping(oid, row["udt_name"], row["data_type"]));
    }
}
} // namespace sancho
}
