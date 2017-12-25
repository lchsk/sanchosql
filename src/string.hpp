#ifndef STRING_HPP
#define STRING_HPP

#include <glibmm.h>

#define IN_MAP(map, item) (map.find(item) != map.end())
#define COUT(var) std::cout << var << std::endl;

namespace san
{
    namespace string
    {
        std::string replace_all(std::string str,
                                const std::string& from,
                                const std::string& to);
        bool contains_only_numbers(const Glib::ustring& text);

        Glib::ustring trim(const Glib::ustring& input);

        bool is_empty(const Glib::ustring& input);
    }
}

#endif
