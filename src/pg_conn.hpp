#ifndef PG_CONN_HPP
#define PG_CONN_HPP

#include <pqxx/pqxx>

class PostgresConnection {
public:
    PostgresConnection(const std::string host,
                       const std::string user,
                       const std::string password,
                       const std::string dbname,
                       const unsigned port);

    ~PostgresConnection()
    {
        conn->disconnect();
    }

    std::vector<std::string> get_db_tables()
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

    std::vector<std::string> get_table_columns(const std::string& table_name)
    {
        std::vector<std::string> columns;

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
                column_name ASC
            )";

        conn->prepare("get_columns", sql);
        pqxx::result result = work.prepared("get_columns")(table_name).exec();

        for (const auto& row : result) {
            columns.push_back(row["column_name"].as<std::string>());
        }

        return columns;
    }


private:
    const std::string host;
    const std::string user;
    const std::string password;
    const std::string dbname;
    const unsigned port;

    std::unique_ptr<pqxx::connection> conn;
};

#endif
