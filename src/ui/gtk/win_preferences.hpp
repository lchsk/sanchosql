#ifndef PREFERENCES_WINDOW_HPP
#define PREFERENCES_WINDOW_HPP

#include <gtkmm.h>

#include "../../system/preferences.hpp"

namespace sancho {
namespace ui {
namespace gtk {

class PreferencesWindow : public Gtk::Window {
  public:
    PreferencesWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);
    virtual ~PreferencesWindow(){};

  void init();
  void set_preferences(sancho::system::Preferences*);

  private:
    void on_win_show();
    void on_win_hide();
    void on_btn_close_clicked();
    void on_btn_apply_clicked();

    Glib::RefPtr<Gtk::Builder> builder;

    sancho::system::Preferences* preferences;

    Gtk::Notebook *notebook_tabs;
    Gtk::Box *box_editor;
    Gtk::Button *btn_apply;
    Gtk::Button *btn_close;

  // Settings
  Gtk::CheckButton *check_set_line_numbers;
};
} // namespace sancho
}
}

#endif
