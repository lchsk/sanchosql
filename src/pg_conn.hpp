#ifndef PG_CONN_HPP
#define PG_CONN_HPP

#include <iostream>
#include <sstream>
#include <unordered_map>

#include <pqxx/pqxx>

struct OidMapping
{
    pqxx::oid oid;
    std::string udt_name;
    std::string data_type;
};

struct Column
{
    pqxx::oid oid;
    std::string column_name;
    std::string data_type;
};

const std::string get_data_type(pqxx::oid oid, std::unordered_map<pqxx::oid, OidMapping>& oid_names);

struct QueryResult
{
    QueryResult(pqxx::connection& conn,
                const std::string& query,
                std::unordered_map<pqxx::oid, OidMapping>& oid_names)
    {
        pqxx::work work(conn);
        pqxx::result result = work.exec(query);

        for (unsigned i = 0; i < result.columns(); i++) {
            columns.push_back(Column({
                        .oid = result.column_type(i),
                            .column_name = result.column_name(i),
                            .data_type = get_data_type(result.column_type(i), oid_names)
                            }));
        }

        for (const auto& row : result) {
            std::vector<std::string> row_data;

            for (unsigned i = 0; i < result.columns(); i++) {
                if (row[i].is_null()) {
                    row_data.push_back("null");
                } else {
                    row_data.push_back(row[i].as<std::string>());
                }
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
    ~PostgresConnection();

    std::shared_ptr<QueryResult> run_query(const std::string& query);

    std::vector<std::string> get_db_tables();

    std::vector<std::pair<std::string, std::string>>
    get_table_columns(const std::string& table_name);

    std::vector<std::map<std::string, std::string> >
    get_table_data(const std::string& table_name,
                   const std::vector<std::pair<std::string, std::string>>& columns);

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

    std::unordered_map<pqxx::oid, OidMapping> oid_names;

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
