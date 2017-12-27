#ifndef TAB_HPP
#define TAB_HPP

#include <iostream>
#include <unordered_map>

#include <gtkmm.h>
#include <gtksourceviewmm.h>

#include "widgets/number_entry.hpp"
#include "model/list_tab_model.hpp"

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

        Gtk::VPaned paned_source;
        Gtk::VPaned paned_results;

        Glib::RefPtr<Gsv::Buffer> buffer;
        Gsv::View* source_view;
        Gtk::ScrolledWindow* source_scrolled_window;

        Glib::RefPtr<Gsv::Buffer> log_buffer;
        Gsv::View* log;
        Gtk::ScrolledWindow* log_scrolled_window;

        Gtk::TreeView* tree;
        Gtk::ScrolledWindow* data_scrolled_window;
        Gtk::ToolButton* btn_execute_editor_query;
    };

    class SimpleTab : public AbstractTab
    {
    public:
        SimpleTab(std::shared_ptr<san::SimpleTabModel>& model);

        // Browse box
        Gtk::Box* browse_box;
        Gtk::Button* btn_insert;
        Gtk::Button* btn_reload;
        // Accept DB changes
        Gtk::ToolButton* btn_accept;
        Gtk::ToolButton* btn_primary_key_warning;
        Gtk::Button* btn_prev;
        Gtk::Button* btn_next;
        Gtk::Label* label_offset;
        Gtk::Label* label_limit;
        san::NumberEntry* number_offset;
        san::NumberEntry* number_limit;
        Gtk::TreeView* tree;

        Gtk::Menu popup;

        Gtk::MenuItem* popup_item_delete_rows;

        std::shared_ptr<san::SimpleTabModel> model;
    };
}

#endif
