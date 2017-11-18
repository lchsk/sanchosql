#include "tab_model.hpp"

namespace san
{
	SimpleTabModel::SimpleTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details,
								   const Glib::ustring& p_table_name) :
		AbstractTabModel(conn_details),
		table_name(p_table_name),
		primary_key(conn().get_primary_key(p_table_name)),
		limit(DEFAULT_LIMIT),
		offset(DEFAULT_OFFSET),
		sort_column(""),
		sort_type(ColumnSortType::None)
	{
	}

	QueryTabModel::QueryTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details)
		: AbstractTabModel(conn_details)
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

		query << "select * from "
			  << table_name
			  << get_order_by_query()
			  << " offset "
			  << std::to_string(offset)
			  << " limit "
			  << std::to_string(limit);

		return query.str();
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
