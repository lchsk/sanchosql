#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/notebook.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

#include <gtkmm.h>

#include "pg_conn.hpp"

class MainWindow : public Gtk::Window
{
public:
    MainWindow(std::shared_ptr<PostgresConnection>&);
    virtual ~MainWindow() {};

    void insert_tables(const std::vector<std::string>& tables);

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

    std::shared_ptr<PostgresConnection> pc;

    void on_tab_close_button_clicked(Gtk::ScrolledWindow*);
    void on_browser_row_activated(const Gtk::TreeModel::Path& path,
                                  Gtk::TreeViewColumn* column);
    void on_open_sql_editor_clicked();

    void on_action_file_new();
    void on_action_file_quit();

    Glib::RefPtr<Gtk::Builder> res_builder;
    Glib::RefPtr<Gio::SimpleActionGroup> menu_group;

    Gtk::Box main_box;
    Gtk::TreeView browser;
    Glib::RefPtr<Gtk::TreeStore> browser_store;
    Gtk::ScrolledWindow browser_scrolled_window;
    Gtk::ScrolledWindow notebook_scrolled_window;
    Gtk::HPaned paned;
    Gtk::Notebook notebook;
};

#endif
