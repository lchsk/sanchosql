#ifndef NEW_CONNECTION_WINDOW_HPP
#define NEW_CONNECTION_WINDOW_HPP

#include <gtkmm.h>

class NewConnectionWindow : public Gtk::Window
{
public:
    NewConnectionWindow
    (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~NewConnectionWindow() {};

private:
    void on_btn_close_clicked();
    void on_btn_test_connection_clicked();
    void on_win_show();

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
    Gtk::Button* btn_test_connection;

    Gtk::Label* label_connection_status;

    Gtk::Entry* text_host;
    Gtk::Entry* text_port;
    Gtk::Entry* text_db;
    Gtk::Entry* text_user;
    Gtk::Entry* text_password;
};

#endif
