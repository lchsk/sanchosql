#ifndef LIST_TAB_MODEL_HPP
#define LIST_TAB_MODEL_HPP

#include <set>

#include <gtkmm.h>

#include "../pg_conn.hpp"
#include "abstract_tab_model.hpp"

namespace san
{
	class SimpleTabModel : public AbstractTabModel
	{
	public:
		enum class ColumnSortType { None, Asc, Desc };

		SimpleTabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details,
					   const Glib::ustring& p_table_name,
					   const Glib::ustring& p_schema_name);

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

        const bool is_part_of_pk(const Glib::ustring& column_name) {
            for (const auto& pk_column : get_primary_key()) {
                if (pk_column.column_name == column_name)
                    return true;
            }

            return false;
        }

		// Run UPDATE query to save pending changes in the DB
		// Doesn't include changes to Primary Key
		void accept_changes();

		void accept_pk_change() {
			if (pk_changes.empty())
				return;

			std::cout << "Accepting pk change " << std::endl;

			std::stringstream query;

			const auto data = *(pk_changes.begin());

			query << "update "
				  << schema_name
				  << "."
				  << table_name
				  << " set "
				  << data.first
				  << " = '"
				  << data.second.second
				  << "' where ";

			unsigned i = 0;

			for (auto t : pk_hist) {
				if (i > 0) {
					query << " and ";
				}

				query << t.first << " = '" << t.second << "'";

				i++;
			}

			query << "; commit;";

			g_debug("Accept PK change query: %s", query.str().c_str());

			conn().run_query(query.str());
		};

		void delete_rows(const std::vector<std::vector<std::pair<Glib::ustring, Glib::ustring>>>& rows_to_delete) {
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

					row_query << t.first << " = '" << t.second << "'";

					i++;
				}

				row_query << "; ";
				query << row_query.str();
			}

			query << "commit;";

			g_debug("Executing delete query: %s", query.str().c_str());

			conn().run_query(query.str());
		}

		bool insert_row(const Gtk::TreeModel::Row& row) {
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
					query << "'" << value << "'";

					i++;
				}
			}

			query << "); commit;";

			g_debug("Insert row query: %s", query.str().c_str());

			conn().run_query(query.str());

			return true;
		}

		unsigned db_rows_cnt;

		const std::string get_query() const;

		// PK currently being edited
        // Column name -> Current Value
		std::map<Glib::ustring, Glib::ustring> pk_hist;

        std::map<std::map<Glib::ustring, Glib::ustring>,
                 std::map<Glib::ustring, Glib::ustring>> map_test;

		// column_name: (old_value, new_value)
		std::map<Glib::ustring, std::pair<Glib::ustring, Glib::ustring>> pk_changes;

	private:
		const std::string get_order_by_query() const;

		const Glib::ustring table_name;
		const Glib::ustring schema_name;
		const std::vector<san::PrimaryKey> primary_key;

		unsigned limit;
		unsigned offset;

		std::string sort_column;
		ColumnSortType sort_type;

		static const unsigned DEFAULT_LIMIT = 30;
		static const unsigned DEFAULT_OFFSET = 0;
	};
}

#endif
