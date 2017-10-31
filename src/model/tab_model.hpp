#ifndef TAB_MODEL_HPP
#define TAB_MODEL_HPP

#include <gtkmm.h>

#include "../pg_conn.hpp"

class AbstractTabModel
{
public:
	AbstractTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details)
		: conn_details(conn_details),
		  connection(std::make_unique<san::PostgresConnection>(conn_details))
	{
		connection->init_connection();
	}

	san::PostgresConnection& conn() const {
		return *connection;
	}

private:
	std::shared_ptr<san::ConnectionDetails> conn_details;
	std::unique_ptr<san::PostgresConnection> connection;
};

class SimpleTabModel : public AbstractTabModel
{
public:
	enum class ColumnSortType { None, Asc, Desc };

	SimpleTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details,
				   const Glib::ustring& p_table_name);

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

	const std::string get_query() const;

private:
	const std::string get_order_by_query() const;

	const Glib::ustring table_name;

	unsigned limit;
	unsigned offset;

	std::string sort_column;
	ColumnSortType sort_type;

	static const unsigned DEFAULT_LIMIT = 30;
	static const unsigned DEFAULT_OFFSET = 0;
};

class QueryTabModel : public AbstractTabModel
{
public:
	QueryTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details);
};

#endif
