#ifndef NUMBER_ENTRY_HPP
#define NUMBER_ENTRY_HPP

#include <gtkmm.h>

#include "../util.hpp"

namespace san
{
    class NumberEntry : public Gtk::Entry
    {
        void on_insert_text(const Glib::ustring& text, int* position)
        {
            if (san::util::contains_only_numbers(text))
                Gtk::Entry::on_insert_text(text, position);
        }
    };
}

#endif
