#ifndef PREFERENCES_WINDOW_HPP
#define PREFERENCES_WINDOW_HPP

#include <gtkmm.h>

namespace sancho {
namespace ui {
namespace gtk {

class PreferencesWindow : public Gtk::Window {
  public:
    PreferencesWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);
    virtual ~PreferencesWindow(){};

  private:
    void on_win_show();
    void on_win_hide();
    void on_btn_close_clicked();

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::Notebook *notebook_tabs;
    Gtk::Box *box_editor;
    Gtk::Button *btn_apply;
    Gtk::Button *btn_close;
};
} // namespace sancho
}
}

#endif
