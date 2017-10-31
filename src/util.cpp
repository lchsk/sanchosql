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
    }
}
