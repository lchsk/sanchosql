

#include "preferences.hpp"

#include "files.hpp"

namespace sancho{
  namespace system{

    Preferences::Preferences()
      : PREFERENCES_PATH(g_build_filename(sancho::files::create_config_dir().c_str(), "preferences.ini", NULL))
    {
    }

    void Preferences::init()
    {
      if (open_preferences_file()) {
        load_values_from_file();
      }
    }

bool Preferences::open_preferences_file()
{
    try {
        file.load_from_file(PREFERENCES_PATH);

        return true;
    } catch (const Glib::Error &ex) {
      g_warning("Cannot load %s", PREFERENCES_PATH.c_str());
    }

    return false;
}

    void Preferences::load_values_from_file()
    {
      const auto group_editor = "editor";

      if (file.has_group(group_editor)) {
        show_line_numbers = file.get_boolean(group_editor, "show_line_numbers");
      }
    }

  }}
