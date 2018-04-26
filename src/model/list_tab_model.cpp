#include "list_tab_model.hpp"

namespace sancho {
SimpleTabModel::SimpleTabModel(
    const std::shared_ptr<sancho::ConnectionDetails> &conn_details,
    const Glib::ustring &p_table_name, const Glib::ustring &p_schema_name)
    : AbstractTabModel(conn_details), table_name(p_table_name),
      schema_name(p_schema_name),
      primary_key(conn().get_primary_key(p_table_name, p_schema_name)),
      limit(DEFAULT_LIMIT), offset(DEFAULT_OFFSET), sort_column(""),
      sort_type(ColumnSortType::None) {}

void SimpleTabModel::set_sort(const std::string &p_sort_column) {
    switch (sort_type) {
    case ColumnSortType::None:
        sort_type = ColumnSortType::Asc;
        sort_column = p_sort_column;
        break;
    case ColumnSortType::Asc:
        sort_type = ColumnSortType::Desc;
        sort_column = p_sort_column;
        break;
    case ColumnSortType::Desc:
        sort_type = ColumnSortType::None;
        sort_column = "";
        break;
    }
}

void SimpleTabModel::set_limit(const std::string &p_limit) {
    try {
        limit = std::stoul(p_limit);
    } catch (const std::invalid_argument &) {
        limit = DEFAULT_LIMIT;
    }
}

void SimpleTabModel::set_offset(const std::string &p_offset) {
    try {
        offset = std::stoul(p_offset);
    } catch (const std::invalid_argument &) {
        offset = DEFAULT_OFFSET;
    }
}

Gtk::SortType SimpleTabModel::get_sort_type() const {
    if (sort_type == ColumnSortType::Asc)
        return Gtk::SortType::SORT_ASCENDING;
    else if (sort_type == ColumnSortType::Desc)
        return Gtk::SortType::SORT_DESCENDING;

    return Gtk::SortType::SORT_ASCENDING;
}

const std::string SimpleTabModel::get_query() const {
    std::stringstream query;

    query << "select * from ";

    if (schema_name != "") {
        query << schema_name << ".";
    }

    query << table_name << get_order_by_query() << " offset "
          << std::to_string(offset) << " limit " << std::to_string(limit);

    return query.str();
}

const std::string SimpleTabModel::get_columns_query() const {
    std::stringstream query;

    query << "select * from information_schema.columns where table_name = "
          << "'" << table_name << "'";

    if (schema_name != "") {
        query << " and table_schema = "
              << "'" << schema_name << "'";
    }

    return query.str();
}

std::shared_ptr<sancho::QueryResult> SimpleTabModel::accept_changes() {
    if (!map_test.size()) {
        return sancho::QueryResult::get(true);
    }

    std::stringstream query;

    for (auto pk : map_test) {
        query << "update " << schema_name << "." << table_name << " set ";

        unsigned i = 0;

        for (auto pk_val : pk.second) {
            if (i > 0) {
                query << ", ";
            }

            query << pk_val.first << " = "
                  << sancho::string::prepare_sql_value(pk_val.second, true);

            i++;
        }

        query << " where ";

        i = 0;

        for (auto pk_col : pk.first) {
            if (i > 0) {
                query << " and ";
            }

            query << pk_col.first << " = "
                  << sancho::string::prepare_sql_value(pk_col.second);

            i++;
        }

        query << "; ";
    }

    g_debug("Executing UPDATE value query: %s", query.str().c_str());

    auto result = conn().run_query(sancho::QueryType::Transaction, query.str());

    // We can't use affected rows information here because we're sending
    // several UPDATE queries in one.
    if (result->success) {
        result->commit();
        map_test.clear();
    } else {
        result->rollback();
    }

    return result;
}

std::shared_ptr<sancho::QueryResult> SimpleTabModel::accept_pk_change() {
    if (pk_changes.empty()) {
        return sancho::QueryResult::get(true);
    }

    if (pk_changes.size() > 1) {
        pk_changes.clear();
        pk_hist.clear();

        return sancho::QueryResult::get(true);
    };

    std::stringstream query;

    const auto data = *(pk_changes.begin());

    query << "update " << schema_name << "." << table_name << " set "
          << data.first << " = "
          << sancho::string::prepare_sql_value(data.second.second) << " where ";

    unsigned i = 0;

    for (auto t : pk_hist) {
        if (i > 0) {
            query << " and ";
        }

        query << t.first << " = "
              << sancho::string::prepare_sql_value(t.second);

        i++;
    }

    g_debug("Executing PRIMARY KEY change query: %s", query.str().c_str());

    auto result = conn().run_query(sancho::QueryType::Transaction, query.str());

    if (result->affected_rows > 1) {
        // That should never happen unless primary key / unique constraint
        // was removed from the column in the meantime.
        g_warning("More than 1 row was affected by the primary key change, "
                  "attempting rollback");
        result->set_status(false, "Looks like primary key constraint was "
                                  "recently changed, rolling back, please "
                                  "reload the tab");

        result->rollback();

        return result;
    }

    if (result->success) {
        result->commit();
    } else {
        result->rollback();
    }

    return result;
}

std::shared_ptr<sancho::QueryResult> SimpleTabModel::delete_rows(
    const std::vector<std::vector<std::pair<Glib::ustring, Glib::ustring>>>
        &rows_to_delete) {
    if (rows_to_delete.empty()) {
        return sancho::QueryResult::get(true);
    }

    g_debug("Removing %d rows", rows_to_delete.size());

    std::stringstream query;

    for (const auto &row : rows_to_delete) {
        std::stringstream row_query;

        row_query << "delete from " << schema_name << "." << table_name
                  << " where ";

        unsigned i = 0;

        for (auto t : row) {
            if (i > 0) {
                row_query << " and ";
            }

            row_query << t.first << " = "
                      << sancho::string::prepare_sql_value(t.second);

            i++;
        }

        row_query << "; ";
        query << row_query.str();
    }

    g_debug("Executing DELETE query: %s", query.str().c_str());

    auto result = conn().run_query(sancho::QueryType::Transaction, query.str());

    // Number of affected rows doesn't help here because we're sending all
    // DELETE queries in one batch.

    if (result->success) {
        result->commit();
    } else {
        result->rollback();
    }

    return result;
}

std::shared_ptr<sancho::QueryResult>
SimpleTabModel::insert_row(const Gtk::TreeModel::Row &row) {
    std::stringstream query;

    query << "insert into " << schema_name << "." << table_name << " (";

    unsigned i = 0;

    for (const auto &col : cols) {
        if (col.first == "#")
            continue;

        Glib::ustring value = row.get_value(col.second);
        ;

        if (!value.empty()) {
            if (i > 0) {
                query << ", ";
            }

            query << col.first;

            i++;
        }
    }

    if (!i) {
        auto result = sancho::QueryResult::get(true);
        result->inserted_empty_row = true;

        return result;
    }

    i = 0;

    query << ") values (";

    for (const auto &col : cols) {
        if (col.first == "#")
            continue;

        Glib::ustring value = row.get_value(col.second);
        ;

        if (!value.empty()) {
            if (i > 0) {
                query << ", ";
            }
            query << sancho::string::prepare_sql_value(value);

            i++;
        }
    }

    query << "); ";

    g_debug("Executing INSERT query: %s", query.str().c_str());

    auto result = conn().run_query(sancho::QueryType::Transaction, query.str());

    // We're executing INSERT queries one by one so we can check if there's
    // exactly one affected row.
    if (result->affected_rows != 1) {
        g_warning("There should be exactly 1 affected row, instead: %d",
                  result->affected_rows);
        result->set_status(false,
                           "There should be exactly 1 affected row, instead: " +
                               result->affected_rows);

        result->rollback();

        return result;
    }

    if (result->success) {
        result->commit();
    } else {
        result->rollback();
    }

    return result;
}

const std::string SimpleTabModel::get_order_by_query() const {
    std::stringstream order_by;

    if (is_sorted()) {
        order_by << " order by " << sort_column;

        if (sort_type == ColumnSortType::Asc)
            order_by << " asc";
        else if (sort_type == ColumnSortType::Desc)
            order_by << " desc";
    }

    return order_by.str();
}

const bool SimpleTabModel::is_part_of_pk(const Glib::ustring &column_name) {
    for (const auto &pk_column : get_primary_key()) {
        if (pk_column.column_name == column_name)
            return true;
    }

    return false;
}

} // namespace sancho
