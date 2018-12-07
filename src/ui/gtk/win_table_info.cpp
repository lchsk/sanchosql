#include "win_table_info.hpp"

namespace sancho {
namespace ui {
namespace gtk {
TableInfoWindow::TableInfoWindow(
    BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder)
    : Gtk::Window(cobject), builder(builder) {

    builder->get_widget("box_columns", box_columns);
    builder->get_widget("btn_close", btn_close);

    signal_show().connect(
        sigc::mem_fun(*this, &TableInfoWindow::on_win_show));
    signal_hide().connect(
        sigc::mem_fun(*this, &TableInfoWindow::on_win_hide));
    btn_close->signal_clicked().connect(
        sigc::mem_fun(*this, &TableInfoWindow::on_btn_close_clicked));

    scrolled_schema.add(tree_schema);
    scrolled_schema.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    box_columns->pack_start(scrolled_schema);

    schema_model = Gtk::TreeStore::create(schema_columns);
    tree_schema.set_model(schema_model);

    tree_schema.append_column("Column name", schema_columns.col_name);
    tree_schema.append_column("Type", schema_columns.col_type);
    tree_schema.append_column("Nullable", schema_columns.col_is_nullable);
    tree_schema.append_column("Default value", schema_columns.col_default);
    tree_schema.append_column("Primary Key", schema_columns.col_is_primary);

    show_all_children();
}

void TableInfoWindow::on_win_show() {
}

void TableInfoWindow::on_win_hide() {
}

void TableInfoWindow::on_btn_close_clicked() {
  hide();
}

} // namespace sancho
}
}
