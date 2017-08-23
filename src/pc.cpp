#include <iostream>

#include <pqxx/pqxx>

#include "main_window.hpp"
#include "pg_conn.hpp"


int main (int argc, char *argv[])
{
    std::shared_ptr<PostgresConnection> pc
        = std::make_shared<PostgresConnection>("127.0.0.1",
                                               "postgres",
                                               "",
                                               "",
                                               5434);
    auto app = Gtk::Application::create(argc, argv, "postgres.client");

    MainWindow main_window(pc);

    main_window.insert_tables(pc->get_db_tables());

    return app->run(main_window);
}
