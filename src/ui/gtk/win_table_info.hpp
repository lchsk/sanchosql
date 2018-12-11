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

    void init(sancho::db::PostgresConnection &conn,
              const std::string &schema_name, const std::string &table_name);

  private:
    void on_win_show();
    void on_win_hide();
    void on_btn_close_clicked();
    void load_columns_data(sancho::db::PostgresConnection &conn,
                           const std::string &schema_name,
                           const std::string &table_name);
    void load_constraints_data(sancho::db::PostgresConnection &conn,
                               const std::string &schema_name,
                               const std::string &table_name);
    void load_indexes_data(sancho::db::PostgresConnection &conn,
                           const std::string &schema_name,
                           const std::string &table_name);
    void load_stats_data(sancho::db::PostgresConnection &conn,
                         const std::string &schema_name,
                         const std::string &table_name);

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::Notebook *notebook_tabs;
    Gtk::Box *box_columns;
    Gtk::Box *box_constraints;
    Gtk::Box *box_indexes;
    Gtk::Box *box_stats;
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

    // Stats tab
    class StatsColumns : public Gtk::TreeModel::ColumnRecord {
      public:
        StatsColumns() {
            add(col_name);
            add(col_description);
            add(col_value);
        }

        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<Glib::ustring> col_description;
        Gtk::TreeModelColumn<Glib::ustring> col_value;
    };

    StatsColumns stats_columns;
    Gtk::ScrolledWindow scrolled_stats;
    Gtk::TreeView tree_stats;
    Glib::RefPtr<Gtk::TreeStore> stats_model;

    const std::unordered_map<std::string, std::string> descriptions = {
        {"relid", "OID of a table"},
        {"schemaname", "Name of the schema that this table is in"},
        {"relname", "Name of this table"},
        {"seq_scan", "Number of sequential scans initiated on this table"},
        {"seq_tup_read", "Number of live rows fetched by sequential scans"},
        {"idx_scan", "Number of index scans initiated on this table"},
        {"idx_tup_fetch", "Number of live rows fetched by index scans"},
        {"n_tup_ins", "Number of rows inserted"},
        {"n_tup_upd", "Number of rows updated (includes HOT updated rows)"},
        {"n_tup_del", "Number of rows deleted"},
        {"n_tup_hot_upd", "Number of rows HOT updated (i.e., with no separate "
                          "index update required)"},
        {"n_live_tup", "Estimated number of live rows"},
        {"n_dead_tup", "Estimated number of dead rows"},
        {"n_mod_since_analyze", "Estimated number of rows modified since this "
                                "table was last analyzed"},
        {"last_vacuum", "Last time at which this table was manually vacuumed "
                        "(not counting VACUUM FULL)"},
        {"last_autovacuum",
         "Last time at which this table was vacuumed by the autovacuum daemon"},
        {"last_analyze", "Last time at which this table was manually analyzed"},
        {"last_autoanalyze",
         "Last time at which this table was analyzed by the autovacuum daemon"},
        {"vacuum_count", "Number of times this table has been manually "
                         "vacuumed (not counting VACUUM FULL)"},
        {"autovacuum_count", "Number of times this table has been vacuumed by "
                             "the autovacuum daemon"},
        {"analyze_count",
         "Number of times this table has been manually analyzed"},
        {"autoanalyze_count", "Number of times this table has been analyzed by "
                              "the autovacuum daemon"},
    };
};
} // namespace sancho
}
}

#endif
