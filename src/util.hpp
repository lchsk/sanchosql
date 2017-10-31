#ifndef UTIL_HPP
#define UTIL_HPP

#include <glibmm.h>

namespace san
{
    namespace util
    {
        std::string replace_all(std::string str,
                                const std::string& from,
                                const std::string& to);
        bool contains_only_numbers(const Glib::ustring& text);
    }
}

#endif
