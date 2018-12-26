#include <string>

#include <gtkmm.h>

#include "files.hpp"

namespace sancho {
namespace files {
const std::string create_config_dir() {
    const Glib::ustring config_home = Glib::get_user_config_dir();
    const std::string path(
        g_build_filename(config_home.c_str(), "sanchosql", NULL));

    const gint result = g_mkdir_with_parents(path.c_str(), 0755);

    if (result == 0) {
        g_debug("Config directory OK");
    } else {
        g_warning("Could not create config directory: %d", result);
    }

    return path;
}

const std::string get_connections_file_path() {
    const std::string path = sancho::files::create_config_dir();
    return std::string(g_build_filename(path.c_str(), "connections.ini", NULL));
}
}
}
