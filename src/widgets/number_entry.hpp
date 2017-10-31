#ifndef NUMBER_ENTRY_HPP
#define NUMBER_ENTRY_HPP

#include <gtkmm.h>

namespace san
{
    class NumberEntry : public Gtk::Entry
    {
        bool contains_only_numbers(const Glib::ustring& text)
        {
            for (unsigned i = 0; i < text.length(); i++) {
                if (! Glib::Unicode::isdigit(text[i]))
                    return false;
            }

            return true;
        }

        void on_insert_text(const Glib::ustring& text, int* position)
        {
            if (contains_only_numbers(text))
                Gtk::Entry::on_insert_text(text, position);
        }
    };
}

#endif
