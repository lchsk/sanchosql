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

        void on_connection_changed();
        void on_win_connections_hide();

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

        std::unordered_map
        <Gtk::ScrolledWindow*, std::shared_ptr<AbstractTabModel> > tab_models;

        std::unordered_map
        <Gtk::ScrolledWindow*, std::shared_ptr<san::AbstractTab> > tabs;
    };
}

#endif
