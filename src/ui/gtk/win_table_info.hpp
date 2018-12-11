#ifndef TABLE_INFO_WINDOW_HPP
#define TABLE_INFO_WINDOW_HPP

#include <gtkmm.h>

#include "../../db/pg/pg_conn.hpp"

namespace sancho {
namespace ui {
namespace gtk {
class TableInfoWindow : public Gtk::Window {
  public:
    TableInfoWindow(BaseObjectType *cobject,
                    const Glib::RefPtr<Gtk::Builder> &builder);
    virtual ~TableInfoWindow(){};

  void init(sancho::db::PostgresConnection& conn, const std::string& schema_name, const std::string& table_name);

  private:
    void on_win_show();
    void on_win_hide();
    void on_btn_close_clicked();
    void load_columns_data(sancho::db::PostgresConnection& conn,
                           const std::string& schema_name, const std::string& table_name);
    void load_constraints_data(sancho::db::PostgresConnection& conn,
                               const std::string& schema_name, const std::string& table_name);
    void load_indexes_data(sancho::db::PostgresConnection& conn,
                               const std::string& schema_name, const std::string& table_name);

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::Notebook *notebook_tabs;
    Gtk::Box *box_columns;
    Gtk::Box *box_constraints;
    Gtk::Box *box_indexes;
    Gtk::Button *btn_close;

  // Schema tab
    class SchemaColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        SchemaColumns() {
          add(col_pos);
          add(col_name);
          add(col_type);
          add(col_is_nullable);
          add(col_default);
        }

        Gtk::TreeModelColumn<Glib::ustring> col_pos;
        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<Glib::ustring> col_type;
        Gtk::TreeModelColumn<Glib::ustring> col_is_nullable;
        Gtk::TreeModelColumn<Glib::ustring> col_default;
    };

    SchemaColumns schema_columns;
    Gtk::ScrolledWindow scrolled_schema;
    Gtk::TreeView tree_schema;
    Glib::RefPtr<Gtk::TreeStore> schema_model;

  // Constraints tab
    class ConstraintsColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        ConstraintsColumns() {
          add(col_name);
          add(col_check);
        }

        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<Glib::ustring> col_check;
    };

    ConstraintsColumns constraints_columns;
    Gtk::ScrolledWindow scrolled_constraints;
    Gtk::TreeView tree_constraints;
    Glib::RefPtr<Gtk::TreeStore> constraints_model;

  // Indexes tab
    class IndexesColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        IndexesColumns() {
          add(col_name);
          add(col_definition);
        }

        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<Glib::ustring> col_definition;
    };

    IndexesColumns indexes_columns;
    Gtk::ScrolledWindow scrolled_indexes;
    Gtk::TreeView tree_indexes;
    Glib::RefPtr<Gtk::TreeStore> indexes_model;

};
} // namespace sancho
}
}

#endif
