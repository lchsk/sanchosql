#ifndef TAB_MODEL_HPP
#define TAB_MODEL_HPP

#include <set>

#include <gtkmm.h>

#include "../pg_conn.hpp"

namespace san
{
	enum class TabType { Simple, Query };

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
		std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;
		std::unique_ptr<Gtk::TreeModelColumn<Gdk::RGBA>> col_color;

	private:
		std::shared_ptr<san::ConnectionDetails> conn_details;
		std::unique_ptr<san::PostgresConnection> connection;

		const std::string EMPTY_SORT_COLUMN = "";
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

		const bool has_primary_key() const {
			return primary_key.size();
		}

		const std::vector<san::PrimaryKey> get_primary_key() const {
			return primary_key;
		}

		void accept_changes() {
			std::stringstream query;

			query << "update "
				  << table_name
				  << " ";

			std::stringstream where;
			std::stringstream values;

			for	(const auto& el : pk) {
				where << " where ";
				bool first_where = true;

				for (const auto& key : el.first) {
					if (! first_where) {
						where << " and ";
					}

					where << key.first
						  << " = '"
						  << key.second
						  << "'";

					first_where = false;
				}

				values << " set ";
				bool first_value = true;

				for (const auto& value : el.second) {
					if (! first_value) {
						values << ", ";
					}

					values << value.first
						   << " = '"
						   << value.second
						   << "'";

					first_value = false;
				}
			}

			query << values.str() << where.str() << "; commit;";

			g_debug("Accept query: %s", query.str().c_str());

			conn().run_query(query.str());

			pk.clear();
		}

		const std::string get_query() const;

		std::vector<std::map<std::string, std::string>> original_pk;
		std::vector<std::map<std::string, std::string>> to_update;
		std::vector<Glib::ustring> columns_to_update;

		std::map<std::set<std::pair<Glib::ustring, Glib::ustring>>, std::unordered_map<std::string, Glib::ustring>> pk;

	private:
		const std::string get_order_by_query() const;

		const Glib::ustring table_name;
		const std::vector<san::PrimaryKey> primary_key;

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

		std::string query;

		const std::string get_query() const {
			return query;
		}
	};
}

#endif
