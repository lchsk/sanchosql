#ifndef TAB_HPP
#define TAB_HPP

#include <iostream>
#include <unordered_map>

#include <gtkmm.h>
#include <gtksourceviewmm.h>

namespace san
{
    class NumberEntry : public Gtk::Entry
    {
        bool contains_only_numbers(const Glib::ustring& text)
        {
            for (int i = 0; i < text.length(); i++) {
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

    class AbstractTab {};

    class Tab : public AbstractTab
    {
    public:
        Tab();

        Glib::RefPtr<Gsv::Buffer> buffer;

        Gtk::HBox* hb;
        Gtk::Button* b;
        Gtk::Label* l;

        Gtk::Image* i;

        Gtk::TextView* tv;

        Gtk::TreeModel::ColumnRecord cr;

        Gtk::Toolbar* toolbar;
        Gtk::ToolButton* btn1;

        Gtk::TreeView* tree;
        Glib::RefPtr<Gtk::ListStore> list_store;

        Gtk::ScrolledWindow* tree_scrolled_window;

        Gsv::View* source_view;

        Gtk::Box* box;
    };

    class EasyTab : public AbstractTab
    {
    public:
        EasyTab();

        Gtk::HBox* hb;
        Gtk::Button* b;
        Gtk::Label* l;

        Gtk::Image* i;

        Gtk::TextView* tv;

        std::shared_ptr<Gtk::TreeModel::ColumnRecord> cr;

        std::unordered_map<Gtk::TreeViewColumn*, std::string> col_names;

        Gtk::Toolbar* toolbar;
        Gtk::ToolButton* btn1;

        Gtk::TreeView* tree;
        Glib::RefPtr<Gtk::ListStore> list_store;

        Gtk::ScrolledWindow* tree_scrolled_window;

        Gtk::Box* box;

        // Browse box
        Gtk::Box* browse_box;
        Gtk::Button* btn_reload;
        Gtk::Button* btn_prev;
        Gtk::Button* btn_next;
        Gtk::Label* label_offset;
        Gtk::Label* label_limit;
        san::NumberEntry* number_offset;
        san::NumberEntry* number_limit;
    };
}

#endif
