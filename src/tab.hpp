#ifndef TAB_HPP
#define TAB_HPP

#include <iostream>

#include <gtkmm.h>
#include <gtksourceviewmm.h>

class Tab
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

#endif
