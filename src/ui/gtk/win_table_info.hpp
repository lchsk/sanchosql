#ifndef TABLE_INFO_WINDOW_HPP
#define TABLE_INFO_WINDOW_HPP

#include <gtkmm.h>

namespace sancho {
namespace ui {
namespace gtk {
class TableInfoWindow : public Gtk::Window {
  public:
    TableInfoWindow(BaseObjectType *cobject,
                    const Glib::RefPtr<Gtk::Builder> &builder);
    virtual ~TableInfoWindow(){};

  private:
    void on_win_show();
    void on_win_hide();
    void on_btn_close_clicked();

    Glib::RefPtr<Gtk::Builder> builder;

    class SchemaColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        SchemaColumns() {
          add(col_name);
          add(col_type);
          add(col_is_nullable);
          add(col_default);
          add(col_is_primary);
        }

        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<Glib::ustring> col_type;
        Gtk::TreeModelColumn<Glib::ustring> col_is_nullable;
        Gtk::TreeModelColumn<Glib::ustring> col_default;
        Gtk::TreeModelColumn<Glib::ustring> col_is_primary;
    };

    SchemaColumns schema_columns;
    Gtk::ScrolledWindow scrolled_schema;
    Gtk::TreeView tree_schema;
    Glib::RefPtr<Gtk::TreeStore> schema_model;

    Gtk::Box *box_columns;
    Gtk::Button *btn_close;

};
} // namespace sancho
}
}

#endif
