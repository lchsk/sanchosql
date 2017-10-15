#ifndef PG_CONN_HPP
#define PG_CONN_HPP

#include <iostream>
#include <sstream>

#include <pqxx/pqxx>

struct ConnectionDetails
{
    const std::string postgres_string()
    {
        std::stringstream conn;

        conn << "hostaddr = " << host
          << " user = " << user
          << " password = " << password
          << " dbname = " << dbname
          << " port = " << port;

        return conn.str();
    }

    std::string name;
    std::string host;
    std::string user;
    std::string password;
    std::string dbname;
    unsigned port;
};

class PostgresConnection {
public:
    PostgresConnection(const std::shared_ptr<ConnectionDetails>& conn_details);

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

    std::vector<std::pair<std::string, std::string>>
    get_table_columns(const std::string& table_name)
    {
        // std::vector<std::string> columns;
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
            // std::cout << row["data_type"];
            // columns.push_back(row["column_name"].as<std::string>());
            columns.push_back(std::make_pair<std::string, std::string>
                              (row["column_name"].as<std::string>(),
                               row["data_type"].as<std::string>()));
        }

        return columns;
    }

    std::vector<std::map<std::string, std::string> >
    get_table_data(const std::string& table_name,
                   // const std::vector<std::string>& columns)
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

private:
    std::shared_ptr<ConnectionDetails> conn_details;

    std::unique_ptr<pqxx::connection> conn;
};

class Connections
{
public:
    Connections() {
        conn = std::make_shared<ConnectionDetails>();

        conn->host = "127.0.0.1";
        conn->user = "sancho";
        conn->password = "sancho";
        conn->dbname = "sancho";
        conn->port = 5432;
    }

    std::shared_ptr<ConnectionDetails>& connection() {
        return conn;
    }

    static Connections* instance() { return &ins; }

private:
    std::shared_ptr<ConnectionDetails> conn;
    static Connections ins;
};


#endif
