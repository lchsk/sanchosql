#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <unordered_map>

#include <gtkmm.h>

#include "pg_conn.hpp"
#include "model/list_tab_model.hpp"
#include "model/query_tab_model.hpp"
#include "tab.hpp"
#include "win_new_connection.hpp"

namespace san
{
    enum class BrowserItemType { Header, Table };

    class MainMenu
    {
      class ImageMenuItem
      {
      public:
        ImageMenuItem() : b1(nullptr), l1(nullptr), i1(nullptr)
        {
          b1 = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 6));
          l1 = Gtk::manage(new Gtk::AccelLabel);
        }

        void set_icon(const std::string& icon_name)
        {
          i1 = Gtk::manage(new Gtk::Image);
          i1->set_from_icon_name(icon_name, Gtk::BuiltinIconSize::ICON_SIZE_MENU);
        }

        void set_text(const std::string& text)
        {
          l1->set_text(text);
          l1->set_use_underline();
          l1->set_xalign(0.0);
        }

        void finish()
        {
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
      MainMenu()
      {
        group = Gtk::AccelGroup::create();

        menu = Gtk::manage(new Gtk::MenuBar);
        menu_item_file = Gtk::manage(new Gtk::MenuItem);
        menu_item_help = Gtk::manage(new Gtk::MenuItem);
        menu_file = Gtk::manage(new Gtk::Menu);
        menu_help = Gtk::manage(new Gtk::Menu);
        menu_item_about = Gtk::manage(new Gtk::MenuItem);

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
        connections_mi->set_icon("network-server");
        connections_mi->finish();

        editor_mi = std::make_unique<ImageMenuItem>();
        editor_mi->set_text("_SQL Editor");
        editor_mi->set_icon("accessories-text-editor");
        editor_mi->finish();

        menu_item_connections = Gtk::manage(new Gtk::MenuItem(*connections_mi->b1));
        menu_file->append(*menu_item_connections);

        menu_item_connections
            ->add_accelerator("activate", group,
                              GDK_KEY_c, Gdk::ModifierType::CONTROL_MASK,
                              Gtk::ACCEL_VISIBLE);

        connections_mi->l1->set_accel_widget(*menu_item_connections);

        menu_item_sql_editor = Gtk::manage(new Gtk::MenuItem(*editor_mi->b1));
        menu_file->append(*menu_item_sql_editor);

        menu_item_sql_editor
            ->add_accelerator("activate", group,
                              GDK_KEY_e, Gdk::ModifierType::CONTROL_MASK,
                              Gtk::ACCEL_VISIBLE);

        editor_mi->l1->set_accel_widget(*menu_item_sql_editor);

        menu_item_separator = Gtk::manage(new Gtk::SeparatorMenuItem);
        menu_file->append(*menu_item_separator);

        quit_mi = std::make_unique<ImageMenuItem>();
        quit_mi->set_text("_Quit");
        quit_mi->finish();

        menu_item_quit = Gtk::manage(new Gtk::MenuItem(*quit_mi->b1));
        menu_file->append(*menu_item_quit);

        menu_item_quit
            ->add_accelerator("activate", group,
                              GDK_KEY_q, Gdk::ModifierType::CONTROL_MASK,
                              Gtk::ACCEL_VISIBLE);

        quit_mi->l1->set_accel_widget(*menu_item_quit);
      }

      Glib::RefPtr<Gtk::AccelGroup> group;

      std::unique_ptr<ImageMenuItem> connections_mi;
      std::unique_ptr<ImageMenuItem> editor_mi;
      std::unique_ptr<ImageMenuItem> quit_mi;

      Gtk::MenuBar* menu;
      Gtk::MenuItem* menu_item_file;
      Gtk::MenuItem* menu_item_help;
      Gtk::Menu* menu_file;
      Gtk::Menu* menu_help;
      Gtk::MenuItem* menu_item_about;
      Gtk::MenuItem* menu_item_connections;
      Gtk::MenuItem* menu_item_sql_editor;
      Gtk::SeparatorMenuItem* menu_item_separator;
      Gtk::MenuItem* menu_item_quit;
    };

    class MainWindow : public Gtk::Window
    {
    public:
        MainWindow();
        virtual ~MainWindow() {};
    protected:
        class BrowserModel : public Gtk::TreeModel::ColumnRecord
        {
        public:
            BrowserModel()
            {
                add(table);
                add(type);
            }

            Gtk::TreeModelColumn<Glib::ustring> table;
            Gtk::TreeModelColumn<san::BrowserItemType> type;
        };

        class ConnectionsModel : public Gtk::TreeModel::ColumnRecord
        {
        public:
            ConnectionsModel()
            {
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
        san::NewConnectionWindow* win_connections;

      Glib::RefPtr<Gtk::AccelGroup> group;
      MainMenu main_menu;

        san::SimpleTabModel& get_simple_tab_model(Gtk::ScrolledWindow*);
        san::QueryTabModel& get_query_tab_model(Gtk::ScrolledWindow*);

        san::SimpleTab& get_simple_tab(Gtk::ScrolledWindow*);
        san::QueryTab& get_query_tab(Gtk::ScrolledWindow*);

        void cellrenderer_validated_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text, san::SimpleTab* tab, san::SimpleTabModel* model, const std::string& column_name);
        // void cellrenderer_validated_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path_string, const Glib::ustring& new_text, san::SimpleTab* tab, san::SimpleTabModel* model, const std::string& column_name) {};
        void cellrenderer_validated_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path_string, san::SimpleTab* tab, san::SimpleTabModel* model, const std::string& column_name);
        // void cellrenderer_validated_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path) {};

        void on_connection_changed();
        void on_schema_changed();
        void on_win_connections_hide();

        typedef guint GdkKeyCode;

        bool check_mod_binding(GdkEventKey* key_event,
                               GdkKeyCode mod_key,
                               GdkKeyCode key) {
            return ((key_event->keyval == key) &&
                    (key_event->state & (GDK_SHIFT_MASK |
                                         GDK_CONTROL_MASK |
                                         GDK_MOD1_MASK)) == mod_key);
        }

		bool on_key_press_event(GdkEventKey* key_event) {
            if (check_mod_binding(key_event, GDK_CONTROL_MASK, GDK_KEY_c)) {
                on_action_file_new();

                return true;
            } else if (check_mod_binding(key_event, GDK_CONTROL_MASK, GDK_KEY_e)) {
                on_open_sql_editor_clicked();

                return true;
            } else if (check_mod_binding(key_event, GDK_CONTROL_MASK, GDK_KEY_q)) {
                on_action_file_quit();

                return true;
            }

            return Gtk::Window::on_key_press_event(key_event);
		}

        std::shared_ptr<san::PostgresConnection> connect(const std::shared_ptr<san::ConnectionDetails>& conn_details) {
            std::shared_ptr<san::PostgresConnection> pc
                = std::make_shared<san::PostgresConnection>(conn_details);
            pc->init_connection();

            return pc;
        }

        void on_primary_key_warning_clicked(const Glib::ustring table_name) {
            Gtk::MessageDialog dialog(*this, "Table \"" + table_name + "\" doesn't a have primary key", false /* use_markup */, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
            dialog.set_modal();
            dialog.set_secondary_text("Updating values in the editor will be disabled");

            dialog.run();
        }

        void show_warning(const Glib::ustring& primary, const Glib::ustring& secondary = Glib::ustring()) {
            Gtk::MessageDialog dialog(*this, primary, false /* use_markup */, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
            dialog.set_modal();

            if (! secondary.empty()) {
                dialog.set_secondary_text(secondary);
            }

            dialog.run();
        }

        void on_menu_file_popup_generic(Gtk::ScrolledWindow* window, san::SimpleTab* tab, san::SimpleTabModel* model) {
            const auto selection = tab->tree->get_selection();

            if (selection) {
                std::size_t size = selection->get_selected_rows().size();

                std::stringstream question;
                question << "Are you sure you want to permanently delete "
                         << size
                         << " row(s)?";

                Gtk::MessageDialog dialog(
                    *this,
                    question.str(), false /* use_markup */, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
                dialog.set_modal();

                std::stringstream text;

                text << "The following rows will be deleted: ";

                auto rows = selection->get_selected_rows();

                unsigned i = 0;

                for (const auto& row_path : rows) {
                    Gtk::TreeModel::iterator row_iter = tab->tree->get_model()->get_iter(row_path);

                    if (row_iter) {
                        Gtk::TreeModel::Row row = *row_iter;

                        if (i > 0)
                            text << ", ";

                        text << row.get_value(model->cols["#"]);

                        i++;
                    }
                }

                dialog.set_secondary_text(text.str());

                const int result = dialog.run();

            if (result == Gtk::RESPONSE_OK) {
                auto pks = model->get_primary_key();

                auto rows = selection->get_selected_rows();

                std::vector<std::vector<std::pair<Glib::ustring, Glib::ustring>>> rows_to_delete;

                for (const auto& row_path : rows) {
                    Gtk::TreeModel::iterator row_iter = tab->tree->get_model()->get_iter(row_path);

                    if (row_iter) {
                        Gtk::TreeModel::Row row = *row_iter;

                        std::vector<std::pair<Glib::ustring, Glib::ustring>> pk_values;

                        for (auto pk : pks) {
                            const Glib::ustring pk_value = row.get_value(model->cols[pk.column_name]);

                            pk_values.push_back(std::make_pair<Glib::ustring, Glib::ustring>(Glib::ustring(pk.column_name), Glib::ustring(pk_value)));
                        }

                        rows_to_delete.push_back(pk_values);
                    }
                }

                auto result = model->delete_rows(rows_to_delete);

                if (result->success) {
                    load_list_results(window);
                } else {
                    san::insert_log_message(tab->log_buffer, result->error_message);

                    show_warning("Deleting rows failed", result->error_message);
                }
            }
            }
        }

        bool on_list_press(GdkEventButton* button_event, san::SimpleTab* tab, san::SimpleTabModel* model) {
            if ((button_event->type == GDK_BUTTON_RELEASE) && (button_event->button == 3)) {
                // gtkmm 3.22 only:
                // tab->popup.popup_at_pointer((GdkEvent*) button_event);
                tab->popup.popup(button_event->button, button_event->time);
            }

            return true;
        };

        void on_browser_refresh_clicked() {
			auto pc = handle_connect();

			if (! pc)
				return;

			refresh_browser(pc);
        }

        std::shared_ptr<san::ConnectionDetails>& find_current_connection() {
            const Glib::ustring connection_name = combo_connections.get_active_text();

            return san::Connections::instance()->find_connection(connection_name);
        }

        std::shared_ptr<san::PostgresConnection> handle_connect() {
            auto current_connection = find_current_connection();

            if (current_connection == san::Connections::instance()->end())
                return nullptr;

            std::shared_ptr<san::PostgresConnection> pc = nullptr;

            try {
                pc = connect(current_connection);
            } catch (const san::NoConnection& e) {
                Glib::ustring error_message;

                if (current_connection->password.empty()) {
                    error_message = "Password for connection \"" + current_connection->name + "\" is empty. Please edit the connection and provide password.";
                } else {
                    error_message = e.what();
                }

                show_warning("Connection failed", error_message);

                reset_browser();

                return nullptr;
            }

            return pc;
        }

        bool on_browser_button_released(GdkEventButton* button_event) {
            if (button_event->button == 3) {
                Gtk::TreeModel::Path path;

                bool found = browser.get_path_at_pos(button_event->x, button_event->y, path);

                if (! found)
                    return false;

                Gtk::TreeModel::iterator iter = browser_store->get_iter(path);

                if (! iter)
                    return false;

                Gtk::TreeModel::Row current_row = *iter;

                if (current_row[browser_model.type] == san::BrowserItemType::Header) {
                    // gtkmm 3.22 only:
                    // popup_browser_header.popup_at_pointer((GdkEvent*) button_event);
                    popup_browser_header.popup(button_event->button, button_event->time);

                    return true;
                }
            }

            return false;
        }

        void on_results_column_clicked(Gtk::ScrolledWindow*, Gtk::TreeViewColumn*);
        void on_tab_close_button_clicked(Gtk::ScrolledWindow*);
        void on_reload_table_clicked(Gtk::ScrolledWindow*);
        void on_insert_row_clicked(Gtk::ScrolledWindow*);
        void on_prev_results_page_clicked(Gtk::ScrolledWindow*);
        void on_next_results_page_clicked(Gtk::ScrolledWindow*);
        void on_browser_row_activated(const Gtk::TreeModel::Path& path,
                                      Gtk::TreeViewColumn* column);
        void on_open_sql_editor_clicked();
        void on_submit_query_clicked(Gtk::ScrolledWindow*, Glib::RefPtr<Gsv::Buffer>&);

        void on_btn_accept_changes_clicked(san::SimpleTab* tab, san::SimpleTabModel* model);

        void load_list_results(Gtk::ScrolledWindow*);
        void load_query_results(Gtk::ScrolledWindow*);

        void handle_results_sort(const san::SimpleTabModel* model,
                                 Gtk::TreeViewColumn* sorted_col);

        void on_action_file_new();
        void on_action_file_quit();
        void on_action_file_about();

        void refresh_connections_list();
        void refresh_browser(const std::shared_ptr<san::PostgresConnection>&);
        void refresh_tree_connections();
        void reset_browser() {
            // Reset current_connection first before triggering events
            san::Connections::instance()->current_connection = nullptr;

            browser_store->clear();
            combo_connections.set_active(0);
            combo_schemas.remove_all();
            combo_schemas.set_active(0);
        };

        Glib::RefPtr<Gtk::Builder> res_builder;
        Glib::RefPtr<Gio::SimpleActionGroup> menu_group;

        Gtk::Box main_box;
        Gtk::TreeView browser;
        Gtk::Menu popup_browser_header;
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

        std::unordered_map
        <Gtk::ScrolledWindow*, std::shared_ptr<AbstractTabModel> > tab_models;

        std::unordered_map
        <Gtk::ScrolledWindow*, std::shared_ptr<san::AbstractTab> > tabs;
    };
}

#endif
