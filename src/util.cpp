#include <string>

#include "util.hpp"

namespace san
{
    namespace util
    {
        std::string replace_all(std::string str,
                                const std::string& from,
                                const std::string& to)
        {
            size_t start_pos = 0;

            while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }

            return str;
        }

        bool contains_only_numbers(const Glib::ustring& text)
        {
            for (unsigned i = 0; i < text.length(); i++) {
                if (! Glib::Unicode::isdigit(text[i]))
                    return false;
            }

            return true;
        }

        std::string trim(const std::string& input) {
            std::string s(input);

            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));

            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), s.end());

            return s;
        }
    }
}
