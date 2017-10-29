#ifndef TAB_MODEL_HPP
#define TAB_MODEL_HPP

#include <gtkmm.h>

#include "../pg_conn.hpp"

class TabModel
{
public:
	enum class ColumnSortType {
		None,
		Asc,
		Desc,
	};

	TabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details,
			 const Glib::ustring& p_table_name);
	TabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details);

	san::PostgresConnection& conn() const {
		return *connection;
	}

	const std::string get_query() const;

	void set_limit(const std::string& p_limit);
	void set_offset(const std::string& p_offset);
	void set_sort(const std::string& p_sort_column);

	Gtk::SortType get_sort_type() const;

	bool is_sorted() const {
		return sort_type != ColumnSortType::None;
	}

	void next_page() {
		offset += limit;
	}

	void prev_page() {
		offset -= std::min(offset, limit);;
	}

	std::string get_limit() const {
		return std::to_string(limit);
	}

	std::string get_offset() const {
		return std::to_string(offset);
	}

	const std::string& get_sort_column() const {
		return sort_column;
	}

private:
	const std::string get_order_by_query() const;

	// EasyTab
	const Glib::ustring table_name;

	unsigned limit;
	unsigned offset;

	std::shared_ptr<san::ConnectionDetails> conn_details;
	std::unique_ptr<san::PostgresConnection> connection;

	// EasyTab
	std::string sort_column;
	ColumnSortType sort_type;

	static const unsigned DEFAULT_LIMIT = 30;
	static const unsigned DEFAULT_OFFSET = 0;
};

#endif
