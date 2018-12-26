#include "win_preferences.hpp"

namespace sancho {
namespace ui {
namespace gtk {
PreferencesWindow::PreferencesWindow(BaseObjectType *cobject,
									 const Glib::RefPtr<Gtk::Builder> &builder)
    : Gtk::Window(cobject), builder(builder) {

    builder->get_widget("notebook_tabs", notebook_tabs);
    builder->get_widget("box_editor", box_editor);
    builder->get_widget("btn_apply", btn_apply);
    builder->get_widget("btn_close", btn_close);

    signal_show().connect(sigc::mem_fun(*this, &PreferencesWindow::on_win_show));
    signal_hide().connect(sigc::mem_fun(*this, &PreferencesWindow::on_win_hide));
    btn_close->signal_clicked().connect(
        sigc::mem_fun(*this, &PreferencesWindow::on_btn_close_clicked));
    set_title("SanchoSQL - preferences");

    show_all_children();
}

void PreferencesWindow::on_win_show() { notebook_tabs->set_current_page(0); }

void PreferencesWindow::on_win_hide() {}

void PreferencesWindow::on_btn_close_clicked() { hide(); }

} // namespace sancho
}
}
