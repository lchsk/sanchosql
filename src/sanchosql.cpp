#include <iostream>

#if defined(SANCHO_OS_UNIX) || defined(SANCHO_OS_MAC)
#include <signal.h>
#endif
#include <pqxx/pqxx>

#include "main_window.hpp"
#include "pg_conn.hpp"
#include "config.hpp"

template <typename T_ArgType>
static bool get_arg_value(const Glib::RefPtr<Glib::VariantDict>& options,
                          const Glib::ustring& arg_name, T_ArgType& arg_value)
{
    arg_value = T_ArgType();

    return options->lookup_value(arg_name, arg_value);
}

int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                    Glib::RefPtr<Gtk::Application>& app)
{
    app->activate();

    const auto options = command_line->get_options_dict();

    if (! options) {
        g_debug("No options");
    }

    bool flag_version = false;
    get_arg_value(options, "version", flag_version);

    if (flag_version) {
        std::cout << "SanchoSQL " << san::config::current_version << std::endl;

        app->quit();
    }

    return 0;
}

int main (int argc, char *argv[])
{
    g_debug("Detected system: %s", SANCHO_OS);

    // If pqxx breaks the connection we might get a SIGPIPE signal
    // Ignoring it here because it leads to program termination
    #if defined(SANCHO_OS_UNIX) || defined(SANCHO_OS_MAC)
    signal(SIGPIPE, SIG_IGN);
    #endif

    auto app = Gtk::Application::create(
        argc, argv, "com.sanchosql",
        Gio::APPLICATION_HANDLES_COMMAND_LINE | Gio::APPLICATION_NON_UNIQUE);

    app->signal_command_line().connect(sigc::bind(sigc::ptr_fun(&on_command_line), app), false);

    app->add_main_option_entry(
        Gio::Application::OptionType::OPTION_TYPE_BOOL, "version", 'v',
        "Show version and quit");

    #ifdef SANCHO_OS_WINDOWS
    Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
    settings->property_gtk_theme_name() = "win32";
    g_debug("Changed theme to win32");
    g_debug("Current theme: %s", settings->property_gtk_theme_name().get_value().c_str());
    #endif

    san::MainWindow main_window;

    return app->run(main_window);
}
