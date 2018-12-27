#ifndef PREFERENCES_WINDOW_HPP
#define PREFERENCES_WINDOW_HPP

#include <gtkmm.h>

#include "../../system/preferences.hpp"

namespace sancho {
namespace ui {
namespace gtk {

class PreferencesWindow : public Gtk::Window {
  public:
    PreferencesWindow(BaseObjectType* cobject,
                      const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~PreferencesWindow(){};

    void init();
    void set_preferences(sancho::system::Preferences*);
    void set_color_schemes(const std::vector<std::string>& schemes);

  private:
    void on_win_show();
    void on_win_hide();
    void on_btn_close_clicked();
    void on_btn_apply_clicked();

    Glib::RefPtr<Gtk::Builder> builder;

    sancho::system::Preferences* preferences;

    Gtk::Notebook* notebook_tabs;
    Gtk::Box* box_editor;
    Gtk::Button* btn_apply;
    Gtk::Button* btn_close;

    // Settings
    Gtk::CheckButton* check_set_line_numbers;
    Gtk::CheckButton* check_add_default_comment;
    Gtk::CheckButton* check_show_whitespace;
    Gtk::CheckButton* check_highlight_current_line;
    Gtk::ComboBoxText* combo_color_scheme;

    Gtk::CheckButton* check_indent_with_spaces;
    Gtk::SpinButton* spin_tab_width;
};
} // namespace sancho
}
}

#endif
