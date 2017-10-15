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
};

#endif
