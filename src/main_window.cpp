#include <iostream>

#include "main_window.hpp"
#include "util.hpp"


MainWindow::MainWindow(std::shared_ptr<PostgresConnection>& pc)
    : main_box(Gtk::ORIENTATION_VERTICAL),
      pc(pc)
{
    set_title("Postgres Client");

    set_border_width(0);
    set_default_size(1200, 800);

    add(main_box);

    browser_scrolled_window.add(browser);
    browser_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    paned.pack1(browser_scrolled_window);

    notebook_scrolled_window.add(notebook);
    paned.pack2(notebook_scrolled_window);

    browser_store = Gtk::TreeStore::create(browser_model);
    browser.set_model(browser_store);

    browser.append_column("Table", browser_model.table);

    browser.signal_row_activated().connect
        (sigc::mem_fun(*this, &MainWindow::on_browser_row_activated));

    menu_group = Gio::SimpleActionGroup::create();

    menu_group->add_action
        ("new_conn",
         sigc::mem_fun(*this, &MainWindow::on_action_file_new));

    menu_group->add_action
        ("quit",
         sigc::mem_fun(*this, &MainWindow::on_action_file_quit) );

    insert_action_group("file", menu_group);

    res_builder = Gtk::Builder::create();

    try {
        res_builder->add_from_resource("/res/toolbar.glade");
        res_builder->add_from_resource("/res/main_menu.glade");
    } catch(const Glib::Error& e) {
        std::cerr << "Building menus and toolbar failed: " <<  e.what();
    }

    auto object = res_builder->get_object("menubar");
    auto menu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);

    if (menu) {
        auto pMenuBar = Gtk::manage(new Gtk::MenuBar(menu));

        main_box.pack_start(*pMenuBar, Gtk::PACK_SHRINK);
    } else {
        g_warning("Menu not found");
    }

    Gtk::Toolbar* toolbar = nullptr;
    res_builder->get_widget("toolbar", toolbar);

    if (toolbar)
        main_box.pack_start(*toolbar, Gtk::PACK_SHRINK);
    else
        g_warning("Toolbar not found");

    main_box.pack_start(paned);

    show_all_children();
}

void MainWindow::on_action_file_quit()
{
    hide();
}

void MainWindow::on_action_file_new()
{
}

void MainWindow::insert_tables(const std::vector<std::string>& tables)
{
    Gtk::TreeModel::Row row = *(browser_store->append());
    row[browser_model.table] = "Tables";


    for (const std::string& table_name : tables) {
        Gtk::TreeModel::Row table_row = *(browser_store->append(row.children()));
        table_row[browser_model.table] = table_name;
    }
}

void MainWindow::on_tab_close_button_clicked(Gtk::TreeView* tree)
{
    notebook.remove_page(*tree);
}

void MainWindow::on_browser_row_activated(const Gtk::TreeModel::Path& path,
                                          Gtk::TreeViewColumn*)
{

    Gtk::TreeModel::iterator iter = browser_store->get_iter(path);

    if (iter) {
        Gtk::TreeModel::Row current_row = *iter;

        Glib::ustring table_name = current_row[browser_model.table];

        Gtk::HBox* hb = Gtk::manage(new Gtk::HBox);
        Gtk::Button* b = Gtk::manage(new Gtk::Button);
        Gtk::Label* l = Gtk::manage(new Gtk::Label(table_name));

        Gtk::Image* i = Gtk::manage
            (new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));
        b->add(*i);
        hb->pack_start(*l, Gtk::PACK_SHRINK);
        hb->pack_start(*b, Gtk::PACK_SHRINK);

        Gtk::TextView* tv = Gtk::manage(new Gtk::TextView);

        Gtk::TreeModel::ColumnRecord cr;

        auto columns = pc->get_table_columns(table_name);
        auto data = pc->get_table_data(table_name, columns);

        Gtk::TreeView* tree = Gtk::manage(new Gtk::TreeView);

        Gtk::ScrolledWindow* tree_scrolled_window
            = Gtk::manage(new Gtk::ScrolledWindow);
        tree_scrolled_window->add(*tree);
        tree_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;

        for (const auto& column : columns) {
            Gtk::TreeModelColumn<Glib::ustring> col;

            cols[column.first] = col;

            cr.add(cols[column.first]);

            tree->append_column(replace_all(column.first, "_", "__") + "\n" + column.second, cols[column.first]);
        }

        Glib::RefPtr<Gtk::ListStore> list_store = Gtk::ListStore::create(cr);

        tree->set_model(list_store);

        for (auto& row : data) {
            Gtk::TreeModel::Row r = *(list_store->append());

            for (const auto& c : cols) {
                r[c.second] = row[c.first];
            }
        }

        notebook.append_page(*tree_scrolled_window, *hb);

        b->signal_clicked().connect
            (sigc::bind<Gtk::TreeView*>
             (sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
              tree));


        hb->show_all_children();

        show_all_children();

        notebook.next_page();
    }
}
