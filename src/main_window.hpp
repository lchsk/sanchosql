#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <unordered_map>

#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/notebook.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

#include <gtkmm.h>

#include <gtksourceviewmm.h>

#include "pg_conn.hpp"
#include "model/tab_model.hpp"

class Tab
{
public:
    Tab()
    {
        hb = Gtk::manage(new Gtk::HBox);
        b = Gtk::manage(new Gtk::Button);
        l = Gtk::manage(new Gtk::Label("SQL Editor"));

        i = Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));

        b->add(*i);
        hb->pack_start(*l, Gtk::PACK_SHRINK);
        hb->pack_start(*b, Gtk::PACK_SHRINK);

        tv = Gtk::manage(new Gtk::TextView);

        toolbar = Gtk::manage(new Gtk::Toolbar);
        btn1 = Gtk::manage(new Gtk::ToolButton);
        btn1->set_icon_name("document-save");

        toolbar->append(*btn1);

        tree = Gtk::manage(new Gtk::TreeView);

        list_store = Gtk::ListStore::create(cr);

        tree->set_model(list_store);

        tree_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

        // tab_models[tree_scrolled_window]
            // = std::make_unique<TabModel>(Connections::instance()->connection());

        source_view = Gtk::manage(new Gsv::View);

        buffer = source_view->get_source_buffer();

        if (! buffer) {
            std::cerr << "Gsv::View::get_source_buffer () failed" << std::endl;
        }

        Glib::RefPtr<Gsv::LanguageManager> lm = Gsv::LanguageManager::get_default();
        Glib::RefPtr<Gsv::Language> lang = lm->get_language("sql");

        Glib::RefPtr<Gsv::StyleSchemeManager> sm = Gsv::StyleSchemeManager::get_default();
        Glib::RefPtr<Gsv::StyleScheme> style = sm->get_scheme("cobalt");

        buffer->set_language(lang);
        buffer->set_style_scheme(style);
        buffer->set_text("select * from country") ;

        const std::string s = buffer->get_text();
        std::cout << s << std::endl;

        box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
        box->pack_start(*toolbar, Gtk::PACK_SHRINK);
        box->pack_start(*source_view);
        box->pack_start(*tree);

        tree_scrolled_window->add(*box);
        tree_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        hb->show_all_children();
    }

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

class MainWindow : public Gtk::Window
{
public:
    MainWindow();
    virtual ~MainWindow() {};

    void insert_tables();

protected:
  class BrowserModel : public Gtk::TreeModel::ColumnRecord
  {
  public:
      BrowserModel()
      {
          add(table);
          add(table_catalog);
          add(table_schema);
      }

      Gtk::TreeModelColumn<Glib::ustring> table;
      Gtk::TreeModelColumn<Glib::ustring> table_catalog;
      Gtk::TreeModelColumn<Glib::ustring> table_schema;
  };

private:
    BrowserModel browser_model;

    TabModel& tab_model(Gtk::ScrolledWindow*);
    Tab& get_tab(Gtk::ScrolledWindow*);

    void on_tab_close_button_clicked(Gtk::ScrolledWindow*);
    void on_browser_row_activated(const Gtk::TreeModel::Path& path,
                                  Gtk::TreeViewColumn* column);
    void on_open_sql_editor_clicked();
    void on_submit_query_clicked(Gtk::ScrolledWindow*, Glib::RefPtr<Gsv::Buffer>&);

    void on_action_file_new();
    void on_action_file_quit();

    Glib::RefPtr<Gtk::Builder> res_builder;
    Glib::RefPtr<Gio::SimpleActionGroup> menu_group;

    Gtk::Box main_box;
    Gtk::TreeView browser;
    Glib::RefPtr<Gtk::TreeStore> browser_store;
    Gtk::ScrolledWindow browser_scrolled_window;
    Gtk::ScrolledWindow notebook_scrolled_window;
    Gtk::HPaned paned;
    Gtk::Notebook notebook;

    std::unordered_map
    <Gtk::ScrolledWindow*, std::unique_ptr<TabModel> > tab_models;

    std::unordered_map
    <Gtk::ScrolledWindow*, std::shared_ptr<Tab> > tabs;
};

#endif
