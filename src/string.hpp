#ifndef STRING_HPP
#define STRING_HPP

#include <vector>

#include <glibmm.h>

#define IN_MAP(map, item) (map.find(item) != map.end())
#define COUT(var) std::cout << var << std::endl;

namespace sancho {
namespace string {
const std::string EMPTY_DB_STRING = "\"\"";

std::string replace_all(std::string str, const std::string &from,
                        const std::string &to);

std::vector<std::string> split(std::string str, const std::string& delimiter);
std::string escape_sql(std::string str);
std::string escape_db_data(std::string str);
std::string prepare_sql_value(std::string str, bool handle_strings = false);

bool contains_only_numbers(const Glib::ustring &text);

Glib::ustring trim(const Glib::ustring &input);

bool is_empty(const Glib::ustring &input);
  Glib::ustring remove_comments(const Glib::ustring& text);
Glib::ustring get_query(const Glib::ustring &text,
                        const Glib::ustring::size_type point);
} // namespace string

namespace date {
Glib::ustring get_current_datetime();
}

namespace user {
std::string get_user_name();
}
} // namespace sancho

#endif
