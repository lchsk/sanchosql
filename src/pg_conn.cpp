#include "pg_conn.hpp"

#include <iostream>

Connections Connections::ins;

PostgresConnection::PostgresConnection
(const std::shared_ptr<ConnectionDetails>& conn_details) :
    conn_details(conn_details),
    is_open_(false),
    error_message_("")
{
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
        oid_names[row["oid"].as<int>()] = OidMapping({
            .oid=row["oid"].as<int>(),
            .udt_name=row["udt_name"].as<std::string>(),
            .data_type=row["data_type"].as<std::string>(),
        });
    }
}

const std::string get_data_type(int oid, std::unordered_map<int, OidMapping>& oid_names)
{
    if (oid_names.find(oid) == oid_names.end()) {
        return std::to_string(oid);
    } else {
        return oid_names[oid].data_type;
    }
}
