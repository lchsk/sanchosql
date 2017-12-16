#ifndef TAB_HPP
#define TAB_HPP

#include <iostream>
#include <unordered_map>

#include <gtkmm.h>
#include <gtksourceviewmm.h>

#include "widgets/number_entry.hpp"

namespace san
{
    class AbstractTab
    {
    public:
        Gtk::HBox* hb;
        Gtk::Button* b;
        Gtk::Label* l;

        Gtk::Image* i;
        Gtk::TextView* tv;

        Gtk::Toolbar* toolbar;
        Gtk::ToolButton* btn1;

        Glib::RefPtr<Gtk::ListStore> list_store;
        std::unique_ptr<Gtk::TreeModel::ColumnRecord> cr;

        Gtk::ScrolledWindow* tree_scrolled_window;

        // Temp
        std::unordered_map<Gtk::TreeViewColumn*, std::string> col_names;

        Gtk::Box* box;
    };

    class QueryTab : public AbstractTab
    {
    public:
        QueryTab();

        Glib::RefPtr<Gsv::Buffer> buffer;
        Gsv::View* source_view;
        Gtk::TreeView* tree;
    };

    class SimpleTab : public AbstractTab
    {
    public:
        SimpleTab();

        // Browse box
        Gtk::Box* browse_box;
        Gtk::Button* btn_insert;
        Gtk::Button* btn_reload;
        // Accept DB changes
        Gtk::ToolButton* btn_accept;
        Gtk::Button* btn_prev;
        Gtk::Button* btn_next;
        Gtk::Label* label_offset;
        Gtk::Label* label_limit;
        san::NumberEntry* number_offset;
        san::NumberEntry* number_limit;
        Gtk::TreeView* tree;

        Gtk::Menu popup;
    };
}

#endif
