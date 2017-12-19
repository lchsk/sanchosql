#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <unordered_map>

#include <gtkmm.h>

#include "pg_conn.hpp"
#include "model/tab_model.hpp"
#include "tab.hpp"
#include "win_new_connection.hpp"

namespace san
{
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
                add(table_catalog);
                add(table_schema);
            }

            Gtk::TreeModelColumn<Glib::ustring> table;
            Gtk::TreeModelColumn<Glib::ustring> table_catalog;
            Gtk::TreeModelColumn<Glib::ustring> table_schema;
        };

    private:
        BrowserModel browser_model;
        san::NewConnectionWindow* win_connections;

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

                model->delete_rows(rows_to_delete);
                load_list_results(window);
            }
            }
        }

        bool on_list_press(GdkEventButton* button_event, san::SimpleTab* tab, san::SimpleTabModel* model) {
            if ((button_event->type == GDK_BUTTON_RELEASE) && (button_event->button == 3)) {
                tab->popup.popup_at_pointer((GdkEvent*) button_event);
            }

            return true;
        };

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

        void refresh_connections_list();
        void refresh_browser(const std::shared_ptr<san::PostgresConnection>&);

        Glib::RefPtr<Gtk::Builder> res_builder;
        Glib::RefPtr<Gio::SimpleActionGroup> menu_group;

        Gtk::Box main_box;
        Gtk::TreeView browser;
        Glib::RefPtr<Gtk::TreeStore> browser_store;
        Gtk::ScrolledWindow browser_scrolled_window;
        Gtk::ScrolledWindow notebook_scrolled_window;
        Gtk::HPaned paned;
        Gtk::Notebook notebook;

        Gtk::Box box_browser;
        // List of connections
        Gtk::ComboBoxText combo_connections;
        Gtk::ComboBoxText combo_schemas;

        std::unordered_map
        <Gtk::ScrolledWindow*, std::shared_ptr<AbstractTabModel> > tab_models;

        std::unordered_map
        <Gtk::ScrolledWindow*, std::shared_ptr<san::AbstractTab> > tabs;
    };
}

#endif
