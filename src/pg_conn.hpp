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

private:
    const std::string host;
    const std::string user;
    const std::string password;
    const std::string dbname;
    const unsigned port;

    std::unique_ptr<pqxx::connection> conn;
};

#endif
