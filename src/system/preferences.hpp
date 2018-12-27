#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <string>

#include <glibmm.h>

namespace sancho {
namespace system {

  struct Preferences {
    Preferences();

    void init();
    void save_values_to_file();

    // Settings - editor
    bool show_line_numbers = true;
    bool add_default_comment = true;
    bool show_whitespace = false;
    bool highlight_current_line = true;
    std::string color_scheme = "oblivion";

  private:
    bool open_preferences_file();
    void load_values_from_file();

    void load_editor_settings();

    Glib::KeyFile file;

    const std::string PREFERENCES_PATH;
  };

}
} // namespace sancho

#endif
