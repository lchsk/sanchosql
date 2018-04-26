#include <ctime>
#include <iostream>
#include <string>

#include "string.hpp"

namespace sancho {
namespace string {
std::string replace_all(std::string str, const std::string& from,
                        const std::string& to)
{
    size_t start_pos = 0;

    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return str;
}

std::string escape_sql(std::string str)
{
    return sancho::string::replace_all(str, "'", "''");
}

const std::vector<std::pair<std::string, std::string>> special_chars{
    {"\b", "\\b"}, {"\f", "\\f"}, {"\n", "\\n"}, {"\r", "\\r"}, {"\t", "\\t"},
};

std::string escape_db_data(std::string str)
{
    for (const auto& special_char : special_chars) {
        str = replace_all(str, special_char.first, special_char.second);
    }

    return str;
}

std::string _prepare_sql_value(std::string str)
{
    return "'" + sancho::string::escape_sql(Glib::strcompress(str)) + "'";
}

std::string prepare_sql_value(std::string str, bool handle_strings)
{
    if (!handle_strings) {
        return _prepare_sql_value(str);
    }

    // Here, the value might actually be either an empty string or NULL
    if (str.empty()) {
        // NULL
        return "NULL";
    } else if (str == sancho::string::EMPTY_DB_STRING) {
        // Empty string
        return "''";
    }

    // Ordinary string
    return _prepare_sql_value(str);
}

bool contains_only_numbers(const Glib::ustring& text)
{
    for (unsigned i = 0; i < text.length(); i++) {
        if (!Glib::Unicode::isdigit(text[i]))
            return false;
    }

    return true;
}

Glib::ustring trim(const Glib::ustring& input)
{
    Glib::ustring s(input);

    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](int ch) { return !std::isspace(ch); }));

    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());

    return s;
}

bool is_empty(const Glib::ustring& input) { return trim(input) == ""; }

Glib::ustring get_query(const Glib::ustring& text,
                        const Glib::ustring::size_type point)
{
    if (point == Glib::ustring::npos)
        return trim(text);

    unsigned end = point;
    int start = point;

    while (end != text.length()) {
        if (text[end++] == ';') {
            break;
        }
    }

    while (true) {
        if (text[start] == ';') {
            start++;
            break;
        }

        if (start == 0)
            break;

        start--;
    }

    Glib::ustring q = trim(text.substr(start, end - start));

    if (q == "") {
        if (!point) {
            // In the worst-case scenario, just return trimmed input
            return trim(text);
        }

        return get_query(text, point - 1);
    }

    if (q[q.length() - 1] != ';') {
        q += ';';
    }

    return q;
}
} // namespace string

namespace date {
Glib::ustring get_current_datetime()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    return Glib::ustring(buffer);
}
} // namespace date

namespace user {
std::string get_user_name() { return Glib::get_user_name(); }
} // namespace user
} // namespace sancho
