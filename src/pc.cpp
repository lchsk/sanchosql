#include <iostream>

#include <pqxx/pqxx>

#include "main_window.hpp"
#include "pg_conn.hpp"


int main (int argc, char *argv[])
{
    PostgresConnection pc("127.0.0.1", "", "", "", 5434);
    std::vector<std::string> tables = pc.get_db_tables();

    auto app = Gtk::Application::create(argc, argv, "Postgres Client");

    MainWindow main_window;

    return app->run(main_window);
}
