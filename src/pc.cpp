#include <iostream>

#include <pqxx/pqxx>

#include "main_window.hpp"
#include "pg_conn.hpp"


int main (int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "sancho");

    san::MainWindow main_window;

    return app->run(main_window);
}
