#include <iostream>

#include "main_window.hpp"
#include "win_new_connection.hpp"
#include "util.hpp"


MainWindow::MainWindow()
    : main_box(Gtk::ORIENTATION_VERTICAL)
{
    set_title("Postgres Client");

    set_border_width(0);
    set_default_size(1200, 800);

    add(main_box);

    Gsv::init();

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
        res_builder->add_from_resource("/res/window_new_connection.glade");
    } catch(const Glib::Error& e) {
        std::cerr << "Building menus and toolbar failed: " <<  e.what();
    }

    Gtk::ToolButton* toolbutton_sql;
    res_builder->get_widget("toolbutton_sql", toolbutton_sql);

    toolbutton_sql->signal_clicked().connect
        (sigc::mem_fun(*this, &MainWindow::on_open_sql_editor_clicked));

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

    int w, h;
    get_size(w, h);
    paned.set_position(0.21 * w);

    insert_tables();
}

void MainWindow::on_action_file_quit()
{
    hide();
}

void MainWindow::on_action_file_new()
{
    NewConnectionWindow* win = nullptr;
    res_builder->get_widget_derived("win_new_connection", win);

    if (win) {
        win->set_transient_for(*this);
        win->set_modal();
        win->show();
    }
}

void MainWindow::insert_tables()
{
    std::shared_ptr<PostgresConnection> pc
        = std::make_shared<PostgresConnection>(san::Connections::instance()->connection());
    pc->init_connection();

    const std::vector<std::string>& tables = pc->get_db_tables();

    Gtk::TreeModel::Row row = *(browser_store->append());
    row[browser_model.table] = "Tables";

    for (const std::string& table_name : tables) {
        Gtk::TreeModel::Row table_row = *(browser_store->append(row.children()));
        table_row[browser_model.table] = table_name;
    }
}

TabModel& MainWindow::tab_model(Gtk::ScrolledWindow* win)
{
    return *(tab_models[win]);
}

san::Tab& MainWindow::get_tab(Gtk::ScrolledWindow* win)
{
    return *(tabs[win]);
}

void MainWindow::on_tab_close_button_clicked(Gtk::ScrolledWindow* tree)
{
    if (tab_models.find(tree) == tab_models.end()) {
        std::cerr << "Could not find connection when closing a tab model" << std::endl;
    } else {
        tab_models.erase(tab_models.find(tree));
    }

    if (tabs.find(tree) == tabs.end()) {
        std::cerr << "Could not find connection when closing a tab" << std::endl;
    } else {
        tabs.erase(tabs.find(tree));
    }

    notebook.remove_page(*tree);

    const unsigned models = tab_models.size();
    const unsigned tabs_n = tabs.size();
    const unsigned notebook_tabs = notebook.get_n_pages();

    if (tabs_n != models || tabs_n != notebook_tabs) {
        std::cerr << "Tabs != Models != Notebook tabs: "
                  << tabs_n << " "
                  << models << " "
                  << notebook_tabs << std::endl;
    }
}

void MainWindow::on_open_sql_editor_clicked()
{
    auto tab = std::make_shared<san::Tab>();

    Gtk::ScrolledWindow* window = tab->tree_scrolled_window;

    tab->b->signal_clicked().connect
        (sigc::bind<Gtk::ScrolledWindow*>
         (sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
          window));

    tab->btn1->signal_clicked().connect
        (sigc::bind<Gtk::ScrolledWindow*, Glib::RefPtr<Gsv::Buffer> >
         (sigc::mem_fun(*this, &MainWindow::on_submit_query_clicked),
          window, tab->buffer));

    tabs[window] = (tab);

    tab_models[window]
        = std::make_unique<TabModel>(san::Connections::instance()->connection());

    notebook.append_page(*window, *(tab->hb));

    show_all_children();

    notebook.next_page();
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

        Gtk::ScrolledWindow* tree_scrolled_window
            = Gtk::manage(new Gtk::ScrolledWindow);

        tab_models[tree_scrolled_window]
            = std::make_unique<TabModel>(san::Connections::instance()->connection());

        const TabModel& tab = tab_model(tree_scrolled_window);
        auto& pc = tab.conn();

        auto columns = pc.get_table_columns(table_name);
        auto data = pc.get_table_data(table_name, columns);

        Gtk::Toolbar* toolbar = Gtk::manage(new Gtk::Toolbar);
        Gtk::ToolButton* btn1 = Gtk::manage(new Gtk::ToolButton);
        btn1->set_icon_name("document-save");

        toolbar->append(*btn1);

        Gtk::TreeView* tree = Gtk::manage(new Gtk::TreeView);

        Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
        box->pack_start(*toolbar, Gtk::PACK_SHRINK);
        box->pack_start(*tree);

        tree_scrolled_window->add(*box);
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

        const std::vector<Gtk::TreeViewColumn*> tree_columns = tree->get_columns();

        for (const auto column : tree_columns) {
            column->set_resizable();
        }

        notebook.append_page(*tree_scrolled_window, *hb);

        b->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
              tree_scrolled_window));

        hb->show_all_children();

        show_all_children();

        notebook.next_page();
    }
}

void MainWindow::on_submit_query_clicked
(Gtk::ScrolledWindow* tree_scrolled_window, Glib::RefPtr<Gsv::Buffer>& buffer)
{
    const std::string query = buffer->get_text();

    std::cout << query << std::endl;

    const TabModel& tab = tab_model(tree_scrolled_window);
    auto& pc = tab.conn();

    std::shared_ptr<QueryResult> result = pc.run_query(query);

    san::Tab& tab2 = get_tab(tree_scrolled_window);

    std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;

    for (const auto& column : result->columns) {
        Gtk::TreeModelColumn<Glib::ustring> col;

        cols[column.column_name] = col;

        tab2.cr.add(cols[column.column_name]);
        tab2.tree->append_column(replace_all(column.column_name, "_", "__") + "\n" + column.data_type, cols[column.column_name]);
    }

    tab2.list_store = Gtk::ListStore::create(tab2.cr);
    tab2.tree->set_model(tab2.list_store);

    for (const auto& row : result->data) {
        Gtk::TreeModel::Row r = *(tab2.list_store->append());

        int i = 0;

        for (const auto& c : result->columns) {
            r[cols[c.column_name]] = row[i];

            i++;
        }
    }
}
