#include "win_table_info.hpp"

namespace sancho {
namespace ui {
namespace gtk {
TableInfoWindow::TableInfoWindow(
    BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder)
    : Gtk::Window(cobject), builder(builder) {

    builder->get_widget("box_columns", box_columns);
    builder->get_widget("box_constraints", box_constraints);
    builder->get_widget("btn_close", btn_close);

    signal_show().connect(
        sigc::mem_fun(*this, &TableInfoWindow::on_win_show));
    signal_hide().connect(
        sigc::mem_fun(*this, &TableInfoWindow::on_win_hide));
    btn_close->signal_clicked().connect(
        sigc::mem_fun(*this, &TableInfoWindow::on_btn_close_clicked));

    // Set up schema tab
    scrolled_schema.add(tree_schema);
    scrolled_schema.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    box_columns->pack_start(scrolled_schema);

    schema_model = Gtk::TreeStore::create(schema_columns);
    tree_schema.set_model(schema_model);
    tree_schema.set_grid_lines(Gtk::TreeViewGridLines::TREE_VIEW_GRID_LINES_BOTH);

    tree_schema.append_column("Position", schema_columns.col_pos);
    tree_schema.append_column("Column name", schema_columns.col_name);
    tree_schema.append_column("Type", schema_columns.col_type);
    tree_schema.append_column("Nullable", schema_columns.col_is_nullable);
    tree_schema.append_column("Default value", schema_columns.col_default);

    // Set up constraints tab
    scrolled_constraints.add(tree_constraints);
    scrolled_constraints.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    box_constraints->pack_start(scrolled_constraints);

    constraints_model = Gtk::TreeStore::create(constraints_columns);
    tree_constraints.set_model(constraints_model);
    tree_constraints.set_grid_lines(Gtk::TreeViewGridLines::TREE_VIEW_GRID_LINES_BOTH);

    tree_constraints.append_column("Name", constraints_columns.col_name);
    tree_constraints.append_column("Check", constraints_columns.col_check);

    show_all_children();
}

void TableInfoWindow::init(sancho::db::PostgresConnection& conn,
                           const std::string& schema_name, const std::string& table_name)
{
  set_title("Table Information for " + schema_name + "." + table_name + " - SanchoSQL");

  load_columns_data(conn, schema_name, table_name);
  load_constraints_data(conn, schema_name, table_name);

}
  void TableInfoWindow::load_columns_data(sancho::db::PostgresConnection& conn,
                           const std::string& schema_name, const std::string& table_name)
  {
    schema_model->clear();

    std::shared_ptr<sancho::QueryResult> result =
        conn.run_query(sancho::QueryType::NonTransaction,
                       conn.get_columns_query(schema_name, table_name),
                       conn.get_columns_query(schema_name, table_name));

    if (!result->success) {
      return;
    }

    const auto& data = result->as_map();

    for (const auto& row_data : data) {
      Gtk::TreeModel::Row row = *(schema_model->append());

      std::string data_type = "";
      std::string char_len = "";

      for (const auto& pair : row_data) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;

        if (key == "ordinal_position") {
          row[schema_columns.col_pos] = value;
        } else if (key == "column_name") {
          row[schema_columns.col_name] = value;
        } else if (key == "column_default") {
          row[schema_columns.col_default] = value;
        } else if (key == "is_nullable") {
          row[schema_columns.col_is_nullable] = value == "YES" ? "yes" : "no";
        } else if (key == "data_type") {
          data_type = value;
        } else if (key == "character_maximum_length") {
          char_len = value;
        }
      }

      if (!char_len.empty()) {
        data_type = data_type + " (" + char_len + ")";
      }

      row[schema_columns.col_type] = data_type;
    }
}

  void TableInfoWindow::load_constraints_data(sancho::db::PostgresConnection& conn,
                                              const std::string& schema_name, const std::string& table_name)
  {
    constraints_model->clear();

    std::shared_ptr<sancho::QueryResult> result =
        conn.run_query(sancho::QueryType::NonTransaction,
                       conn.get_check_constraints_query(schema_name, table_name),
                       conn.get_columns_query(schema_name, table_name));

    if (!result->success) {
      return;
    }

    const auto& data = result->as_map();

    for (const auto& row_data : data) {
      Gtk::TreeModel::Row row = *(constraints_model->append());

      for (const auto& pair : row_data) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;

        if (key == "check_name") {
          row[constraints_columns.col_name] = value;
        } else if (key == "check_constraint") {
          row[constraints_columns.col_check] = value;
        }
      }
    }
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
