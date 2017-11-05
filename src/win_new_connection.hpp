#ifndef NEW_CONNECTION_WINDOW_HPP
#define NEW_CONNECTION_WINDOW_HPP

#include <gtkmm.h>

namespace san
{
    class NewConnectionWindow : public Gtk::Window
    {
    public:
        NewConnectionWindow
        (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
        virtual ~NewConnectionWindow() {};

    private:
        enum class Mode { Adding, Editing };

        Mode mode;
        // Name of the connection currently being edited
        Glib::ustring edited_conn_name;

        void set_adding_mode();
        void set_editing_mode();

        bool can_add_new_connection() const;
        bool can_save_edited_connection() const;
        void update_save_btn();
        void prepare_adding();
        void reset_widgets();

        void on_win_show();

        void on_btn_save_clicked();
        void on_btn_close_clicked();
        void on_btn_test_connection_clicked();
        void on_btn_add_connection_clicked();
        void on_btn_del_connection_clicked();

        void on_selected_connection_changed();

        Glib::RefPtr<Gtk::Builder> builder;

        class ConnectionColumns : public Gtk::TreeModel::ColumnRecord
        {
        public:
            ConnectionColumns()
            {
                add(col_name);
            }

            Gtk::TreeModelColumn<Glib::ustring> col_name;
        };

        ConnectionColumns connection_columns;
        Gtk::ScrolledWindow scrolled_window;
        Gtk::TreeView tree_connections;
        Glib::RefPtr<Gtk::TreeStore> connections_model;

        Gtk::Box* box_left;
        Gtk::Paned* paned_new_connections;
        Gtk::Button* btn_close;
        Gtk::Button* btn_save;
        Gtk::Button* btn_test_connection;
        Gtk::Button* btn_add_connection;
        Gtk::Button* btn_del_connection;

        Gtk::Label* label_connection_status;

        Gtk::Entry* text_connection_name;
        Gtk::Entry* text_host;
        Gtk::Entry* text_port;
        Gtk::Entry* text_db;
        Gtk::Entry* text_user;
        Gtk::Entry* text_password;
    };
}

#endif
