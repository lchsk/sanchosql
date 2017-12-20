#ifndef ABSTRACT_TAB_MODEL_HPP
#define ABSTRACT_TAB_MODEL_HPP

#include "../pg_conn.hpp"

namespace san
{
	class AbstractTabModel
	{
	public:
		AbstractTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details)
			: conn_details(conn_details),
			  connection(std::make_unique<san::PostgresConnection>(conn_details))
		{
			connection->init_connection();
		}

		virtual ~AbstractTabModel() {}

		san::PostgresConnection& conn() const {
			return *connection;
		}

		virtual const std::string get_query() const = 0;

		virtual const std::string& get_sort_column() const {
			return EMPTY_SORT_COLUMN;
		};

		const Gdk::RGBA col_white = Gdk::RGBA("rgba(255, 255, 255, 1.0)");
		const Gdk::RGBA col_highlighted = Gdk::RGBA("rgba(100, 20, 210, 1.0)");
		const Gdk::RGBA col_inserted = Gdk::RGBA("rgba(40, 200, 10, 1.0)");
		std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;
		std::unique_ptr<Gtk::TreeModelColumn<Gdk::RGBA>> col_color;

	private:
		std::shared_ptr<san::ConnectionDetails> conn_details;
		std::unique_ptr<san::PostgresConnection> connection;

		const std::string EMPTY_SORT_COLUMN = "";
	};
}

#endif
