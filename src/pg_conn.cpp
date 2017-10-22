#include <iostream>

#include "pg_conn.hpp"

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

std::shared_ptr<QueryResult>
PostgresConnection::run_query(const std::string& query)
{
    return std::make_shared<QueryResult>(*conn, query, oid_names);
}

std::vector<std::string> PostgresConnection::get_db_tables()
{
    std::vector<std::string> tables;

    pqxx::work work(*conn);

    const std::string sql = R"(
            SELECT
                *
            FROM
                information_schema.tables
            WHERE
                table_schema = 'public'
            ORDER BY
                table_name ASC
            )";

    pqxx::result result = work.exec(sql);

    for (const auto& row : result) {
        tables.push_back(row["table_name"].as<std::string>());
    }

    return tables;
}

std::vector<std::pair<std::string, std::string>>
PostgresConnection::get_table_columns(const std::string& table_name)
{
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

void PostgresConnection::init_connection()
{
    std::cout << "Trying to connect to: " << conn_details->postgres_string() << std::endl;

    try {
        conn = std::make_unique<pqxx::connection>(conn_details->postgres_string());

        error_message_ = "";
        is_open_ = conn->is_open();

        load_oids();
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;

        error_message_ = e.what();
        is_open_ = false;
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
