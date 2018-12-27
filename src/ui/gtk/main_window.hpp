#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <unordered_map>

#include <gtkmm.h>

#include "../../db/model/list_tab_model.hpp"
#include "../../db/model/query_tab_model.hpp"
#include "../../db/pg/pg_conn.hpp"
#include "../../system/preferences.hpp"
#include "common_ui.hpp"
#include "tab.hpp"
#include "win_new_connection.hpp"
#include "win_preferences.hpp"
#include "win_table_info.hpp"

namespace sancho {
namespace ui {
namespace gtk {
enum class BrowserItemType { Header, Table, View, Trigger, Function, Sequence };

class MainMenu {
    class ImageMenuItem {
      public:
        ImageMenuItem() : b1(nullptr), l1(nullptr), i1(nullptr) {
            b1 = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 6));
            l1 = Gtk::manage(new Gtk::AccelLabel);
        }

        void set_icon(const std::string& icon_name) {
            i1 = Gtk::manage(new Gtk::Image);
            i1->set_from_icon_name(icon_name,
                                   Gtk::BuiltinIconSize::ICON_SIZE_MENU);
        }

        void set_text(const std::string& text) {
            l1->set_text(text);
            l1->set_use_underline();
            l1->set_xalign(0.0);
        }

        void finish() {
            if (i1) {
                b1->add(*i1);
            }

            b1->pack_end(*l1, true, true, 0);
        }

        Gtk::Box* b1;
        Gtk::AccelLabel* l1;
        Gtk::Image* i1;
    };

  public:
    MainMenu() {
        group = Gtk::AccelGroup::create();

        menu = Gtk::manage(new Gtk::MenuBar);
        menu_item_file = Gtk::manage(new Gtk::MenuItem);
        menu_item_help = Gtk::manage(new Gtk::MenuItem);
        menu_file = Gtk::manage(new Gtk::Menu);
        menu_help = Gtk::manage(new Gtk::Menu);
        // menu_help->set_reserve_toggle_size(false);

        menu_item_file->set_label("_File");
        menu_item_file->set_use_underline();
        menu->append(*menu_item_file);

        menu_item_help->set_label("_Help");
        menu_item_help->set_use_underline();
        menu->append(*menu_item_help);

        menu_file->set_reserve_toggle_size(false);
        menu_item_file->set_submenu(*menu_file);

        menu_item_help->set_submenu(*menu_help);
        // menu_item_help->set_always_show_image();
        menu_file->set_reserve_toggle_size(false);

        connections_mi = std::make_unique<ImageMenuItem>();
        connections_mi->set_text("_Connections");
        connections_mi->set_icon("system-file-manager");
        connections_mi->finish();

        editor_mi = std::make_unique<ImageMenuItem>();
        editor_mi->set_text("_SQL Editor");
        editor_mi->set_icon("accessories-text-editor");
        editor_mi->finish();

        preferences_mi = std::make_unique<ImageMenuItem>();
        preferences_mi->set_text("_Preferences");
        preferences_mi->finish();

        menu_item_preferences =
            Gtk::manage(new Gtk::MenuItem(*preferences_mi->b1));

        menu_item_preferences->add_accelerator("activate", group, GDK_KEY_p,
                                               Gdk::ModifierType::CONTROL_MASK,
                                               Gtk::ACCEL_VISIBLE);

        preferences_mi->l1->set_accel_widget(*menu_item_preferences);

        menu_item_connections =
            Gtk::manage(new Gtk::MenuItem(*connections_mi->b1));
        menu_file->append(*menu_item_connections);

        menu_item_connections->add_accelerator("activate", group, GDK_KEY_n,
                                               Gdk::ModifierType::CONTROL_MASK,
                                               Gtk::ACCEL_VISIBLE);

        connections_mi->l1->set_accel_widget(*menu_item_connections);

        menu_item_sql_editor = Gtk::manage(new Gtk::MenuItem(*editor_mi->b1));
        menu_file->append(*menu_item_sql_editor);

        menu_item_sql_editor->add_accelerator("activate", group, GDK_KEY_e,
                                              Gdk::ModifierType::CONTROL_MASK,
                                              Gtk::ACCEL_VISIBLE);

        editor_mi->l1->set_accel_widget(*menu_item_sql_editor);

        menu_file->append(*menu_item_preferences);

        menu_item_separator = Gtk::manage(new Gtk::SeparatorMenuItem);
        menu_file->append(*menu_item_separator);

        quit_mi = std::make_unique<ImageMenuItem>();
        quit_mi->set_text("_Quit");
        quit_mi->finish();

        menu_item_quit = Gtk::manage(new Gtk::MenuItem(*quit_mi->b1));
        menu_file->append(*menu_item_quit);

        menu_item_quit->add_accelerator("activate", group, GDK_KEY_q,
                                        Gdk::ModifierType::CONTROL_MASK,
                                        Gtk::ACCEL_VISIBLE);

        quit_mi->l1->set_accel_widget(*menu_item_quit);

        about_mi = std::make_unique<ImageMenuItem>();
        about_mi->set_text("_About");
        about_mi->finish();

        menu_item_about = Gtk::manage(new Gtk::MenuItem(*about_mi->b1));
        menu_help->append(*menu_item_about);
    }

    Glib::RefPtr<Gtk::AccelGroup> group;

    std::unique_ptr<ImageMenuItem> connections_mi;
    std::unique_ptr<ImageMenuItem> editor_mi;
    std::unique_ptr<ImageMenuItem> preferences_mi;
    std::unique_ptr<ImageMenuItem> quit_mi;
    std::unique_ptr<ImageMenuItem> about_mi;

    Gtk::MenuBar* menu;
    Gtk::MenuItem* menu_item_file;
    Gtk::MenuItem* menu_item_help;
    Gtk::Menu* menu_file;
    Gtk::Menu* menu_help;
    Gtk::MenuItem* menu_item_about;
    Gtk::MenuItem* menu_item_connections;
    Gtk::MenuItem* menu_item_preferences;
    Gtk::MenuItem* menu_item_sql_editor;
    Gtk::SeparatorMenuItem* menu_item_separator;
    Gtk::MenuItem* menu_item_quit;
};

class MainWindow : public Gtk::Window {
  public:
    MainWindow();
    virtual ~MainWindow(){};

  protected:
    class BrowserModel : public Gtk::TreeModel::ColumnRecord {
      public:
        BrowserModel() {
            add(object_name);
            add(type);
        }

        Gtk::TreeModelColumn<Glib::ustring> object_name;
        Gtk::TreeModelColumn<BrowserItemType> type;
    };

    class ConnectionsModel : public Gtk::TreeModel::ColumnRecord {
      public:
        ConnectionsModel() {
            add(name);
            add(host);
            add(dbname);
            add(user);
        }

        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<Glib::ustring> host;
        Gtk::TreeModelColumn<Glib::ustring> dbname;
        Gtk::TreeModelColumn<Glib::ustring> user;
    };

  private:
    BrowserModel browser_model;
    ConnectionsModel connections_model;
    sancho::ui::gtk::NewConnectionWindow* win_connections;
    sancho::ui::gtk::TableInfoWindow* win_table_info;
    sancho::ui::gtk::PreferencesWindow* win_preferences;

    std::unique_ptr<sancho::system::Preferences> preferences;

    Glib::RefPtr<Gtk::AccelGroup> group;
    MainMenu main_menu;

    Glib::ustring get_selected_query(const Glib::RefPtr<Gsv::Buffer>& buffer);

    sancho::db::SimpleTabModel&
    get_simple_tab_model(sancho::ui::gtk::TabWindow*);
    sancho::db::QueryTabModel& get_query_tab_model(sancho::ui::gtk::TabWindow*);

    sancho::ui::gtk::SimpleTab& get_simple_tab(sancho::ui::gtk::TabWindow*);
    sancho::ui::gtk::QueryTab& get_query_tab(sancho::ui::gtk::TabWindow*);

    Gtk::ScrolledWindow* get_current_swindow();

    sancho::ui::gtk::TabType get_tab_type(sancho::ui::gtk::TabWindow* win);

    void cellrenderer_validated_on_edited(const Glib::ustring& path_string,
                                          const Glib::ustring& new_text,
                                          sancho::ui::gtk::SimpleTab* tab,
                                          sancho::db::SimpleTabModel* model,
                                          const std::string& column_name);
    void cellrenderer_validated_on_editing_started(
        Gtk::CellEditable* cell_editable, const Glib::ustring& path_string,
        sancho::ui::gtk::SimpleTab* tab, sancho::db::SimpleTabModel* model,
        const std::string& column_name);

    void on_connection_changed();
    void on_schema_changed();
    void on_win_connections_hide();

    typedef guint GdkKeyCode;

    bool check_mod_binding(GdkEventKey* key_event, GdkKeyCode mod_key,
                           GdkKeyCode key);

    bool on_key_press_event(GdkEventKey* key_event);

    std::shared_ptr<sancho::db::PostgresConnection>
    connect(const std::shared_ptr<sancho::db::ConnectionDetails>& conn_details);

    void on_primary_key_warning_clicked(const Glib::ustring table_name);

    void show_warning(const Glib::ustring& primary,
                      const Glib::ustring& secondary = Glib::ustring());

    void on_menu_file_popup_generic(Gtk::ScrolledWindow* window,
                                    sancho::ui::gtk::SimpleTab* tab,
                                    sancho::db::SimpleTabModel* model);

    bool on_list_press(GdkEventButton* button_event,
                       sancho::ui::gtk::SimpleTab* tab,
                       sancho::db::SimpleTabModel* model);

    void on_browser_refresh_clicked();

    std::shared_ptr<sancho::db::ConnectionDetails>& find_current_connection();

    std::shared_ptr<sancho::db::PostgresConnection> handle_connect();

    bool on_browser_button_released(GdkEventButton* button_event);

    void on_results_column_clicked(Gtk::ScrolledWindow*, Gtk::TreeViewColumn*);
    void on_tab_close_button_clicked(Gtk::ScrolledWindow*);
    bool on_tab_button_released(GdkEventButton* button_event,
                                Gtk::ScrolledWindow*);
    void on_reload_table_clicked(Gtk::ScrolledWindow*);
    void on_reset_filtering_clicked(sancho::ui::gtk::TabWindow*);
    void on_table_info_clicked(sancho::ui::gtk::TabWindow*);
    void on_insert_row_clicked(Gtk::ScrolledWindow*);
    void on_prev_results_page_clicked(Gtk::ScrolledWindow*);
    void on_next_results_page_clicked(Gtk::ScrolledWindow*);
    void on_browser_row_activated(const Gtk::TreeModel::Path& path,
                                  Gtk::TreeViewColumn* column);
    void on_open_sql_editor_clicked();
    void on_show_view_query_clicked(sancho::ui::gtk::SimpleTab*);
    void on_submit_query_clicked(Gtk::ScrolledWindow*,
                                 Glib::RefPtr<Gsv::Buffer>&);
    void on_submit_query_all_clicked(Gtk::ScrolledWindow*,
                                     Glib::RefPtr<Gsv::Buffer>&);
    void on_explain_query_clicked(Gtk::ScrolledWindow*,
                                  Glib::RefPtr<Gsv::Buffer>&);

    void on_btn_accept_changes_clicked(sancho::ui::gtk::SimpleTab* tab,
                                       sancho::db::SimpleTabModel* model);

    void load_list_results(Gtk::ScrolledWindow*);
    void load_query_results(Gtk::ScrolledWindow*);

    void handle_results_sort(const sancho::db::SimpleTabModel* model,
                             Gtk::TreeViewColumn* sorted_col);

    void on_action_file_new();
    void on_action_preferences();
    void on_action_file_quit();
    void on_action_file_about();

    void refresh_connections_list();
    void refresh_browser(const std::shared_ptr<sancho::db::PostgresConnection>&,
                         const std::string& filter = "");
    void on_show_table_info_clicked();
    void on_show_function_definition_clicked();
    void refresh_tree_connections();
    void reset_browser();

    Glib::RefPtr<Gtk::Builder> res_builder;
    Glib::RefPtr<Gio::SimpleActionGroup> menu_group;

    Gtk::Box main_box;
    Gtk::TreeView browser;
    Gtk::Box box_browser_filter;
    Gtk::Entry entry_browser_filter;

    // Browser - header popup
    Gtk::Menu popup_browser_header;
    Gtk::MenuItem* popup_item_table_info;
    Glib::ustring selected_object_name;

    // Browser - functions
    Gtk::Menu popup_function;
    Gtk::MenuItem* popup_item_show_function_definition;

    // List view popup
    Gtk::Menu popup_browser_table;
    Gtk::MenuItem* popup_item_refresh_browser;

    Gtk::Box box_main_pane;

    Gtk::Box* box_dashboard;
    Gtk::TreeView* tree_connections;
    Glib::RefPtr<Gtk::ListStore> store_connections;
    Gtk::Button* btn_dashboard_connections;

    Glib::RefPtr<Gtk::TreeStore> browser_store;
    Gtk::ScrolledWindow browser_scrolled_window;
    Gtk::ScrolledWindow notebook_scrolled_window;
    Gtk::HPaned paned;
    Gtk::Notebook notebook;

    Gtk::Box box_browser;
    // List of connections
    Gtk::ComboBoxText combo_connections;
    Gtk::ComboBoxText combo_schemas;
    Gtk::Label label_connections;
    Gtk::Label label_schemas;

    std::unordered_map<Gtk::ScrolledWindow*,
                       std::shared_ptr<sancho::db::AbstractTabModel>>
        tab_models;

    std::unordered_map<Gtk::ScrolledWindow*,
                       std::shared_ptr<sancho::ui::gtk::AbstractTab>>
        tabs;
};
} // namespace sancho
}
}

#endif
