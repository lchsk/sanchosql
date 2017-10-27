#ifndef TAB_MODEL_HPP
#define TAB_MODEL_HPP

#include "../pg_conn.hpp"

class TabModel
{
public:
	TabModel(const std::shared_ptr<san::ConnectionDetails>& conn_details);

	san::PostgresConnection& conn() const {
		return *connection;
	}

	std::string get_query() const {
		return "select * from " + table_name + " offset " + std::to_string(offset) + " limit " + std::to_string(limit);
	}

	void set_limit(const std::string& p_limit) {
		try {
			limit = std::stoul(p_limit);
		} catch (const std::invalid_argument&) {
			limit = DEFAULT_LIMIT;
		}
	}

	void set_offset(const std::string& p_offset) {
		try {
			offset = std::stoul(p_offset);
		} catch (const std::invalid_argument&) {
			offset = DEFAULT_OFFSET;
		}
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

	std::string table_name;

	unsigned limit;
	unsigned offset;

private:
	std::shared_ptr<san::ConnectionDetails> conn_details;
	std::unique_ptr<san::PostgresConnection> connection;

	static const unsigned DEFAULT_LIMIT = 30;
	static const unsigned DEFAULT_OFFSET = 0;
};

#endif
