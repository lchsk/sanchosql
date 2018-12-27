#ifndef NUMBER_ENTRY_HPP
#define NUMBER_ENTRY_HPP

#include <gtkmm.h>

#include "../../string.hpp"

namespace sancho {
namespace ui {
namespace gtk {
class NumberEntry : public Gtk::Entry {
    void on_insert_text(const Glib::ustring& text, int* position) {
        if (sancho::string::contains_only_numbers(text))
            Gtk::Entry::on_insert_text(text, position);
    }
};
} // namespace sancho
}
}
#endif
