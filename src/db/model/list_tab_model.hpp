#ifndef LIST_TAB_MODEL_HPP
#define LIST_TAB_MODEL_HPP

#include <set>

#include <gtkmm.h>

#include "../pg/pg_conn.hpp"
#include "abstract_tab_model.hpp"

namespace sancho {
namespace db {
class SimpleTabModel : public AbstractTabModel {
  public:
    enum class ColumnSortType { None, Asc, Desc };

    SimpleTabModel(
        const std::shared_ptr<sancho::db::ConnectionDetails>& conn_details,
        const Glib::ustring& p_table_name, const Glib::ustring& p_schema_name);

    void set_limit(const std::string& p_limit);
    void set_offset(const std::string& p_offset);
    void set_sort(const std::string& p_sort_column);
    void set_filter(const std::string& p_filter);

    Gtk::SortType get_sort_type() const;

    bool is_sorted() const { return sort_type != ColumnSortType::None; }

    void next_page() { offset += limit; }

    void prev_page() { offset -= std::min(offset, limit); }

    unsigned get_limit() const { return limit; }
    unsigned get_offset() const { return offset; }

    const std::string& get_sort_column() const { return sort_column; }

    const bool has_primary_key() const { return primary_key.size(); }

    const std::vector<sancho::db::PrimaryKey> get_primary_key() const {
        return primary_key;
    }

    const bool is_part_of_pk(const Glib::ustring& column_name);

    // Run UPDATE query to save pending changes in the DB
    // Doesn't include changes to Primary Key
    std::shared_ptr<sancho::QueryResult> accept_changes();
    std::shared_ptr<sancho::QueryResult> accept_pk_change();
    std::shared_ptr<sancho::QueryResult> delete_rows(
        const std::vector<std::vector<std::pair<Glib::ustring, Glib::ustring>>>&
            rows_to_delete);
    std::shared_ptr<sancho::QueryResult>
    insert_row(const Gtk::TreeModel::Row& row);

    unsigned db_rows_cnt;

    const std::string get_query() const;
    const Glib::ustring& get_table_name() const { return table_name; }
    const Glib::ustring& get_schema_name() const { return schema_name; }

    // PK currently being edited
    // Column name -> Current Value (before commiting to DB)
    std::map<Glib::ustring, Glib::ustring> pk_hist;

    // Pending changes:
    // - first: primary key (column -> value)
    // - second: value changes (column -> new value)
    std::map<std::map<Glib::ustring, Glib::ustring>,
             std::map<Glib::ustring, Glib::ustring>>
        map_test;

    // Storing old value of the cell that's being edited:
    // - first: primary key (column -> value)
    // - second: value (column -> old value)
    std::map<std::map<Glib::ustring, Glib::ustring>,
             std::map<Glib::ustring, Glib::ustring>>
        map_old_values;

    // column_name: (old_value, new_value)
    std::map<Glib::ustring, std::pair<Glib::ustring, Glib::ustring>> pk_changes;

  private:
    const std::string get_order_by_query() const;

    const Glib::ustring table_name;
    const Glib::ustring schema_name;
    const std::vector<sancho::db::PrimaryKey> primary_key;
    std::string query_filter;

    unsigned limit;
    unsigned offset;

    std::string sort_column;
    ColumnSortType sort_type;

    static const unsigned DEFAULT_LIMIT = 100;
    static const unsigned DEFAULT_OFFSET = 0;
};
} // namespace sancho
}

#endif
