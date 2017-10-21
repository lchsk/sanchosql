#ifndef PG_CONN_HPP
#define PG_CONN_HPP

#include <iostream>
#include <sstream>
#include <unordered_map>

#include <pqxx/pqxx>

struct OidMapping
{
    int oid;
    std::string udt_name;
    std::string data_type;
};

struct Column
{
    int oid;
    std::string column_name;
    std::string data_type;
};

struct QueryResult
{
    QueryResult(pqxx::connection& conn, const std::string& query)
    {
        pqxx::work work(conn);
        pqxx::result result = work.exec(query);

        for (int i = 0; i < result.columns(); i++) {
            columns.push_back(Column({
                        .oid = result.column_type(i),
                            .column_name = result.column_name(i),
                            .data_type = "hehe"
                            }));
        }

        for (const auto& row : result) {
            // std::cout << row[0].table() << std::endl;

            // std::map<std::string, std::string> v;

            std::vector<std::string> row_data;

            for (int i = 0; i < result.columns(); i++) {
                // std::cout << row[i].as<std::string>();

                if (row[i].is_null()) {
                    row_data.push_back("null");
                } else {
                    row_data.push_back(row[i].as<std::string>());
                }
                // try {
                    // v[col_name.first] = row[col_name.first].as<std::string>();
                // } catch (const std::exception&) {
                    // v[col_name.first] = "null";
                // }
            }

            data.push_back(row_data);
        }
    }

    std::vector<Column> columns;
    std::vector<std::vector<std::string> > data;
};

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

    void set_host(const std::string& hostname) {
        if (hostname == "localhost")
            host = "127.0.0.1";
        else
            host = hostname;
    }

    std::string name;
    std::string host;
    std::string user;
    std::string password;
    std::string dbname;
    std::string port;
};

class PostgresConnection {
public:
    PostgresConnection(const std::shared_ptr<ConnectionDetails>& conn_details);

    ~PostgresConnection()
    {
        if (! conn) return;

        try {
            if (conn->is_open())
                conn->disconnect();
        } catch (const std::exception& e) {
            std::cerr << "Error when disconnecting: " << e.what() << std::endl;
        }
    }

    std::shared_ptr<QueryResult> run_query(const std::string& query)
    {
        std::vector<std::map<std::string, std::string> > data;

        try {
            std::shared_ptr<QueryResult> res
                = std::make_shared<QueryResult>(*conn, query);

            // pqxx::result result = work.exec(query);

            // for (int i = 0; i < result.columns(); i++) {
                // std::cout << "name: " << result.column_name(i)
                          // << "\t type: " << result.column_type(i)
                          // << "\t table: " << result.table(i)
                          // << "\t col: " << result.table_column(i)
                          // << std::endl;
            // }

            // for (const auto& row : result) {
                // std::cout << row[0].table() << std::endl;
                // std::map<std::string, std::string> v;

                // for (const auto& col_name : columns) {
                    // try {
                        // v[col_name.first] = row[col_name.first].as<std::string>();
                    // } catch (const std::exception&) {
                        // v[col_name.first] = "null";
                    // }
                // }

                // data.push_back(v);
            // }

            return res;

            // std::cout << result.columns() << std::endl;
            // std::cout << result.size() << std::endl;
        } catch (const pqxx::pqxx_exception& e) {
            std::cout << "EXCEPTION: " << e.base().what() << std::endl;
        }
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

    const std::string get_data_type(int oid);

    void init_connection();

    bool is_open() const {
        return is_open_;
    };

    const std::string& error_message() const {
        return error_message_;
    };

private:
    void load_oids();

    std::shared_ptr<ConnectionDetails> conn_details;

    std::unique_ptr<pqxx::connection> conn;

    std::unordered_map<int, OidMapping> oid_names;

    bool is_open_;
    std::string error_message_;
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
        conn->port = "5432";
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
