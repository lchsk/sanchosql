#include <iostream>

#include <pqxx/pqxx>

#include "main_window.hpp"
#include "pg_conn.hpp"


int main (int argc, char *argv[])
{
    PostgresConnection pc("127.0.0.1", "", "", "", 5434);
    auto app = Gtk::Application::create(argc, argv, "postgres.client");

    MainWindow main_window;

    main_window.insert_tables(pc.get_db_tables());

    return app->run(main_window);
}
