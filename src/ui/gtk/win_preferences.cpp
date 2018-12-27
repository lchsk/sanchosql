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

    // Settings

    builder->get_widget("check_set_line_numbers", check_set_line_numbers);
    builder->get_widget("check_add_default_comment", check_add_default_comment);
    builder->get_widget("check_show_whitespace", check_show_whitespace);
    builder->get_widget("check_highlight_current_line", check_highlight_current_line);
    builder->get_widget("combo_color_scheme", combo_color_scheme);
    builder->get_widget("check_indent_with_spaces", check_indent_with_spaces);
    builder->get_widget("spin_tab_width", spin_tab_width);

    signal_show().connect(sigc::mem_fun(*this, &PreferencesWindow::on_win_show));
    signal_hide().connect(sigc::mem_fun(*this, &PreferencesWindow::on_win_hide));
    btn_close->signal_clicked().connect(
        sigc::mem_fun(*this, &PreferencesWindow::on_btn_close_clicked));
    btn_apply->signal_clicked().connect(
        sigc::mem_fun(*this, &PreferencesWindow::on_btn_apply_clicked));
    set_title("SanchoSQL - preferences");

    show_all_children();
}

  void PreferencesWindow::set_preferences(sancho::system::Preferences* pref)
  {
    preferences = pref;
  }

  void PreferencesWindow::set_color_schemes(const std::vector<std::string>& schemes)
  {
    for (const auto& scheme : schemes) {
      combo_color_scheme->append(scheme);
    }
  }

  void PreferencesWindow::init()
  {
    check_set_line_numbers->set_active(preferences->show_line_numbers);
    check_add_default_comment->set_active(preferences->add_default_comment);
    check_show_whitespace->set_active(preferences->show_whitespace);
    check_highlight_current_line->set_active(preferences->highlight_current_line);

    combo_color_scheme->set_active_text(preferences->color_scheme);

    check_indent_with_spaces->set_active(preferences->indent_with_spaces);
    spin_tab_width->set_value(preferences->tab_width);
  }

void PreferencesWindow::on_win_show() { notebook_tabs->set_current_page(0); }

void PreferencesWindow::on_win_hide() {}

void PreferencesWindow::on_btn_close_clicked() { hide(); }
void PreferencesWindow::on_btn_apply_clicked()
{
  preferences->show_line_numbers = check_set_line_numbers->get_active();
  preferences->add_default_comment = check_add_default_comment->get_active();
  preferences->show_whitespace = check_show_whitespace->get_active();
  preferences->highlight_current_line = check_highlight_current_line->get_active();

  preferences->color_scheme = combo_color_scheme->get_active_text();

  preferences->indent_with_spaces = check_indent_with_spaces->get_active();
  preferences->tab_width = spin_tab_width->get_value();

  preferences->save_values_to_file();

  hide();
}

} // namespace sancho
}
}
