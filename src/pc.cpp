#include <iostream>

#include <pqxx/pqxx>

#include "main_window.hpp"
#include "pg_conn.hpp"


int main (int argc, char *argv[])
{
    g_debug("Detected system: %s", SANCHO_OS);

    auto app = Gtk::Application::create(argc, argv, "sancho.sql");

    #ifdef SANCHO_OS_WINDOWS
    Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
    settings->property_gtk_theme_name() = "win32";
    g_debug("Changed theme to win32");
    g_debug("Current theme: %s", settings->property_gtk_theme_name().get_value().c_str());
    #endif

    san::MainWindow main_window;

    return app->run(main_window);
}
