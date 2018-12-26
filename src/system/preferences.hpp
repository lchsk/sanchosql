#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <string>

#include <glibmm.h>

namespace sancho {
namespace system {

  struct Preferences {
    Preferences();

    void init();

    // Settings

    bool show_line_numbers = true;

  private:
    bool open_preferences_file();
    void load_values_from_file();
    Glib::KeyFile file;

    const std::string PREFERENCES_PATH;
  };

}
} // namespace sancho

#endif
