#ifndef TAB_HPP
#define TAB_HPP

#include <iostream>
#include <unordered_map>

#include <gtkmm.h>
#include <gtksourceviewmm.h>

#include "../../db/model/list_tab_model.hpp"
#include "number_entry.hpp"

namespace sancho {
namespace ui {
namespace gtk {
void insert_log_message(Glib::RefPtr<Gsv::Buffer> &log_buffer,
                        const Glib::ustring &message);

enum class TabType { List, Query, Invalid };

  struct SQLFileStatus {
    std::string path;
    bool modified;
    bool file_loaded;

    SQLFileStatus(const std::string& path, bool modified, bool file_loaded)
      : path(path), modified(modified), file_loaded(file_loaded) {
    }
  };

class AbstractTab {
  public:
    AbstractTab(const Glib::ustring &tab_name, TabType type);

    void show() const;

    Gtk::HBox *hb;
    Gtk::Button *b;
    Gtk::Label *l;
    Gtk::EventBox event_box;

    Gtk::Image *i;
    Gtk::TextView *tv;

    Gtk::Toolbar *toolbar;

    Glib::RefPtr<Gtk::ListStore> list_store;
    std::unique_ptr<Gtk::TreeModel::ColumnRecord> cr;

    Gtk::ScrolledWindow *tree_scrolled_window;

    // Temp
    std::unordered_map<Gtk::TreeViewColumn *, std::string> col_names;

    Gtk::Box *box;

    const Glib::ustring tab_name;

    const TabType type;
};

class QueryTab : public AbstractTab {
  public:
  QueryTab(const Glib::ustring &tab_name, Gtk::Window* window);

    void on_buffer_changed();
    void on_btn_open_file_clicked(QueryTab* tab);
    void on_btn_save_file_clicked(QueryTab* tab);
    void on_btn_save_file_as_clicked(QueryTab* tab);
    void on_btn_execute_all_editor_queries_clicked(QueryTab* tab);

    Gtk::VPaned paned_source;
    Gtk::VPaned paned_results;

    Glib::RefPtr<Gsv::Buffer> buffer;
    Gsv::View *source_view;
    Gtk::Box* box_source;
    Gtk::ScrolledWindow *source_scrolled_window;

    Glib::RefPtr<Gsv::Buffer> log_buffer;
    Gsv::View *log;
    Gtk::ScrolledWindow *log_scrolled_window;
    Gtk::Box* box_editor;
    Gtk::Label* label_cursor_position;
    Gtk::Label* label_filename;

    Gtk::TreeView *tree;
    Gtk::ScrolledWindow *data_scrolled_window;
    Gtk::ToolButton *btn_execute_editor_query;
    Gtk::ToolButton *btn_execute_all_editor_queries;
    Gtk::ToolButton *btn_open_file;
    Gtk::ToolButton *btn_save_file;
    Gtk::ToolButton *btn_save_file_as;
private:
  const std::string read_file(const std::string& path);
  void save_file(const std::string& path);
  void update_file_buttons();
    Gtk::Window* parent_window;
    SQLFileStatus file_status;
};

class SimpleTab : public AbstractTab {
  public:
    SimpleTab(const Glib::ustring &tab_name,
              std::shared_ptr<sancho::db::SimpleTabModel> &model);
    // Browse box
    Gtk::Box *browse_box;
    // Accept DB changes
    Gtk::ToolButton *btn_accept;
    Gtk::ToolButton *btn_refresh;
    Gtk::ToolButton *btn_primary_key_warning;
    Gtk::ToolButton *btn_insert;
    Gtk::ToolButton *btn_prev;
    Gtk::ToolButton *btn_next;
    Gtk::ToolButton *btn_reset_filtering;
    Gtk::ToolButton *btn_table_info;
    Gtk::Entry *entry_column_mask;
    Gtk::Entry *entry_filter;

    Gtk::Label *label_offset;
    Gtk::Label *label_limit;
    sancho::ui::gtk::NumberEntry *number_offset;
    sancho::ui::gtk::NumberEntry *number_limit;
    Gtk::TreeView *tree;
    Gtk::ScrolledWindow *data_scrolled_window;
    Gtk::Menu popup;

    Gtk::MenuItem *popup_item_delete_rows;

    Glib::RefPtr<Gsv::Buffer> log_buffer;
    Gsv::View *log;
    Gtk::ScrolledWindow *log_scrolled_window;

    Gtk::VPaned paned_main;

    std::shared_ptr<sancho::db::SimpleTabModel> model;
};
} // namespace sancho
}
}

#endif
