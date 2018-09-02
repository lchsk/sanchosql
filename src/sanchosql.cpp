#include <iostream>

#if defined(SANCHO_OS_UNIX) || defined(SANCHO_OS_MAC)
#include <signal.h>
#endif
#include <pqxx/pqxx>

#include "config.hpp"
#include "ui/gtk/main_window.hpp"
#include "db/pg/pg_conn.hpp"

template <typename T_ArgType>
static bool get_arg_value(const Glib::RefPtr<Glib::VariantDict> &options,
                          const Glib::ustring &arg_name, T_ArgType &arg_value) {
    arg_value = T_ArgType();

    return options->lookup_value(arg_name, arg_value);
}

int on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &options) {
    bool flag_version = false;
    get_arg_value(options, "version", flag_version);

    if (flag_version) {
        std::cout << "SanchoSQL " << sancho::config::current_version
                  << std::endl;

        // -1 to continue default option processing
        // 0 to exit with success
        // > 0 for failures
        return EXIT_SUCCESS;
    }

    return -1;
}

int main(int argc, char *argv[]) {
    g_debug("Detected system: %s", SANCHO_OS);

// If pqxx breaks the connection we might get a SIGPIPE signal
// Ignoring it here because it leads to program termination
#if defined(SANCHO_OS_UNIX) || defined(SANCHO_OS_MAC)
    signal(SIGPIPE, SIG_IGN);
#endif

    auto app = Gtk::Application::create(argc, argv, "com.sanchosql",
                                        Gio::APPLICATION_NON_UNIQUE);

    app->add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_BOOL,
                               "version", 'v', "Show version and quit");

    app->signal_handle_local_options().connect(
        sigc::ptr_fun(&on_handle_local_options), false);

#ifdef SANCHO_OS_WINDOWS
    Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
    settings->property_gtk_theme_name() = "win32";
    g_debug("Changed theme to win32");
    g_debug("Current theme: %s",
            settings->property_gtk_theme_name().get_value().c_str());
#endif

    sancho::ui::gtk::MainWindow main_window;

    return app->run(main_window);
}
