#include "list_tab_model.hpp"

namespace san
{
	SimpleTabModel::SimpleTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details,
								   const Glib::ustring& p_table_name,
								   const Glib::ustring& p_schema_name) :
		AbstractTabModel(conn_details),
		table_name(p_table_name),
		schema_name(p_schema_name),
		primary_key(conn().get_primary_key(p_table_name, p_schema_name)),
		limit(DEFAULT_LIMIT),
		offset(DEFAULT_OFFSET),
		sort_column(""),
		sort_type(ColumnSortType::None)
	{
	}

	void SimpleTabModel::set_sort(const std::string& p_sort_column)
	{
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

	void SimpleTabModel::set_limit(const std::string& p_limit) {
		try {
			limit = std::stoul(p_limit);
		} catch (const std::invalid_argument&) {
			limit = DEFAULT_LIMIT;
		}
	}

	void SimpleTabModel::set_offset(const std::string& p_offset) {
		try {
			offset = std::stoul(p_offset);
		} catch (const std::invalid_argument&) {
			offset = DEFAULT_OFFSET;
		}
	}

	Gtk::SortType SimpleTabModel::get_sort_type() const
	{
		if (sort_type == ColumnSortType::Asc)
			return Gtk::SortType::SORT_ASCENDING;
		else if (sort_type == ColumnSortType::Desc)
			return Gtk::SortType::SORT_DESCENDING;

		return Gtk::SortType::SORT_ASCENDING;
	}

	const std::string SimpleTabModel::get_query() const
	{
		std::stringstream query;

		query << "select * from ";

		if (schema_name != "") {
			query << schema_name << ".";
		}

		query << table_name
			  << get_order_by_query()
			  << " offset "
			  << std::to_string(offset)
			  << " limit "
			  << std::to_string(limit);

		return query.str();
	}

	const std::string SimpleTabModel::get_columns_query() const
	{
		std::stringstream query;

		query << "select * from information_schema.columns where table_name = "
			  << "'"
			  << table_name
			  << "'";

		if (schema_name != "") {
			query << " and table_schema = "
				  << "'"
				  << schema_name
				  << "'";
		}

		return query.str();
	}

	void SimpleTabModel::accept_changes()
	{
		if (! map_test.size()) return;

		std::stringstream query;

		for (auto pk : map_test) {
			query << "update "
				  << schema_name
				  << "."
				  << table_name
				  << " set ";

			unsigned i = 0;

			for (auto pk_val : pk.second) {
				if (i > 0) {
					query << ", ";
				}

				query << pk_val.first << " = " << san::string::prepare_sql_value(pk_val.second);

				i++;
			}

			query << " where ";

			i = 0;

			for (auto pk_col : pk.first) {
				if (i > 0) {
					query << " and ";
				}

				query << pk_col.first << " = " << san::string::prepare_sql_value(pk_col.second);

				i++;
			}

			query << "; ";
		}

		query << "commit;";

		g_debug("Accept query: %s", query.str().c_str());

		conn().run_query(query.str());

		map_test.clear();
	}

	void SimpleTabModel::accept_pk_change() {
		if (pk_changes.empty())
			return;

		if (pk_changes.size() > 1) {
			pk_changes.clear();
			pk_hist.clear();

			return;
		};

		std::stringstream query;

		const auto data = *(pk_changes.begin());

		query << "update "
			  << schema_name
			  << "."
			  << table_name
			  << " set "
			  << data.first
			  << " = "
			  << san::string::prepare_sql_value(data.second.second)
			  << " where ";

		unsigned i = 0;

		for (auto t : pk_hist) {
			if (i > 0) {
				query << " and ";
			}

			query << t.first << " = " << san::string::prepare_sql_value(t.second);

			i++;
		}

		query << "; commit;";

		g_debug("Accept PK change query: %s", query.str().c_str());

		conn().run_query(query.str());
	};

	void SimpleTabModel::delete_rows(const std::vector<std::vector<std::pair<Glib::ustring, Glib::ustring>>>& rows_to_delete) {
		if (rows_to_delete.empty())
			return;

		std::cout << "Removing " << rows_to_delete.size() << " rows" << std::endl;

		std::stringstream query;

		for (const auto& row : rows_to_delete) {
			std::stringstream row_query;

			row_query << "delete from "
					  << schema_name
					  << "."
					  << table_name
					  << " where ";

			unsigned i = 0;

			for (auto t : row) {
				if (i > 0) {
					row_query << " and ";
				}

				row_query << t.first << " = " << san::string::prepare_sql_value(t.second);

				i++;
			}

			row_query << "; ";
			query << row_query.str();
		}

		query << "commit;";

		g_debug("Executing delete query: %s", query.str().c_str());

		conn().run_query(query.str());
	}

	bool SimpleTabModel::insert_row(const Gtk::TreeModel::Row& row) {
		std::stringstream query;

		query << "insert into "
			  << schema_name
			  << "."
			  << table_name
			  << " (";

		unsigned i = 0;

		for (const auto& col : cols) {
			if (col.first == "#") continue;

			Glib::ustring value = row.get_value(col.second);;

			if (! value.empty()) {
				if (i > 0) {
					query << ", ";
				}

				query << col.first;

				i++;
			}
		}

		if (! i) {
			return false;
		}

		i = 0;

		query << ") values (";

		for (const auto& col : cols) {
			if (col.first == "#") continue;

			Glib::ustring value = row.get_value(col.second);;

			if (! value.empty()) {
				if (i > 0) {
					query << ", ";
				}
				query << san::string::prepare_sql_value(value);

				i++;
			}
		}

		query << "); commit;";

		g_debug("Insert row query: %s", query.str().c_str());

		conn().run_query(query.str());

		return true;
	}

	const std::string SimpleTabModel::get_order_by_query() const
	{
		std::stringstream order_by;

		if (is_sorted()) {
			order_by << " order by "
					 << sort_column;

			if (sort_type == ColumnSortType::Asc)
				order_by << " asc";
			else if (sort_type == ColumnSortType::Desc)
				order_by << " desc";
		}

		return order_by.str();
	}
}
