#include <iostream>

#include "main_window.hpp"
#include "win_new_connection.hpp"
#include "util.hpp"
#include "model/tab_model.hpp"

namespace san
{
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
        san::NewConnectionWindow* win = nullptr;
        res_builder->get_widget_derived("win_new_connection", win);

        if (win) {
            win->set_transient_for(*this);
            win->set_modal();
            win->show();
        }
    }

    void MainWindow::insert_tables()
    {
        std::shared_ptr<san::PostgresConnection> pc
            = std::make_shared<san::PostgresConnection>(san::Connections::instance()->connection());
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

    san::AbstractTab& MainWindow::get_tab(Gtk::ScrolledWindow* win)
    {
        return *(tabs[win]);
    }

    void MainWindow::on_results_column_clicked(
        Gtk::ScrolledWindow* window,
        Gtk::TreeViewColumn* col
    )
    {
        san::AbstractTab& at = get_tab(window);
        san::EasyTab& tab = static_cast<san::EasyTab&>(at);
        auto& model = tab_model(window);

        model.set_sort(tab.col_names[col]);

        load_results(window);
    }

    void MainWindow::load_results(Gtk::ScrolledWindow* window)
    {
        auto& pc = tab_models[window]->conn();

        san::AbstractTab& at = get_tab(window);
        san::EasyTab& tab = static_cast<san::EasyTab&>(at);

        std::shared_ptr<san::QueryResult> result
            = pc.run_query(tab_models[window]->get_query());

        std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;

        tab.col_names.clear();
        tab.tree->remove_all_columns();

        tab.cr = std::make_shared<Gtk::TreeModel::ColumnRecord>();

        Gtk::TreeViewColumn* sorted_col = nullptr;

        for (const auto& column : result->columns) {
            Gtk::TreeModelColumn<Glib::ustring> col;

            cols[column.column_name] = col;

            tab.cr->add(cols[column.column_name]);

            const std::string escaped_column_name
                = san::util::replace_all(column.column_name, "_", "__");
            const std::string data_type = column.data_type;
            const std::string column_name = escaped_column_name + "\n" + data_type;

            Gtk::TreeViewColumn* tree_view_column
                = Gtk::manage(new Gtk::TreeViewColumn(column_name,
                                                      cols[column.column_name]));

            tab.tree->append_column(*tree_view_column);

            tree_view_column->signal_clicked().connect
                (sigc::bind<Gtk::ScrolledWindow*, Gtk::TreeViewColumn*>
                 (sigc::mem_fun(*this, &MainWindow::on_results_column_clicked),
                  window, tree_view_column));

            if (column.column_name == tab_models[window]->get_sort_column()) {
                sorted_col = tree_view_column;
            }

            tab.col_names[tree_view_column] = column.column_name;
        }

        tab.tree->set_headers_clickable();

        tab.list_store = Gtk::ListStore::create(*(tab.cr));
        tab.tree->set_model(tab.list_store);

        if (tab_models[window]->is_sorted() && sorted_col) {
            sorted_col->set_sort_indicator(true);
            sorted_col->set_sort_order(tab_models[window]->get_sort_type());
        }

        for (const auto& row : result->data) {
            Gtk::TreeModel::Row r = *(tab.list_store->append());

            int i = 0;

            for (const auto& c : result->columns) {
                r[cols[c.column_name]] = row[i];

                i++;
            }
        }
    }

    void MainWindow::on_prev_results_page_clicked(Gtk::ScrolledWindow* window)
    {
        san::AbstractTab& at = get_tab(window);
        san::EasyTab& tab = static_cast<san::EasyTab&>(at);

        tab_models[window]->set_offset(tab.number_offset->get_text());
        tab_models[window]->set_limit(tab.number_limit->get_text());

        tab_models[window]->prev_page();

        tab.number_offset->set_text(tab_models[window]->get_offset());
        tab.number_limit->set_text(tab_models[window]->get_limit());

        load_results(window);
    }

    void MainWindow::on_next_results_page_clicked(Gtk::ScrolledWindow* window)
    {
        san::AbstractTab& at = get_tab(window);
        san::EasyTab& tab = static_cast<san::EasyTab&>(at);

        tab_models[window]->set_offset(tab.number_offset->get_text());
        tab_models[window]->set_limit(tab.number_limit->get_text());

        tab_models[window]->next_page();

        tab.number_offset->set_text(tab_models[window]->get_offset());
        tab.number_limit->set_text(tab_models[window]->get_limit());

        load_results(window);
    }

    void MainWindow::on_reload_table_clicked(Gtk::ScrolledWindow* window)
    {
        san::AbstractTab& at = get_tab(window);
        san::EasyTab& tab = static_cast<san::EasyTab&>(at);

        tab_models[window]->set_offset(tab.number_offset->get_text());
        tab_models[window]->set_limit(tab.number_limit->get_text());

        tab.number_offset->set_text(tab_models[window]->get_offset());
        tab.number_limit->set_text(tab_models[window]->get_limit());

        load_results(window);
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

            auto tab = std::make_shared<san::EasyTab>();

            Gtk::ScrolledWindow* window = tab->tree_scrolled_window;

            tabs[window] = (tab);

            tab_models[window]
                = std::make_unique<TabModel>(
                    san::Connections::instance()->connection(),
                    table_name);

            tab->number_offset->set_text(tab_models[window]->get_offset());
            tab->number_limit->set_text(tab_models[window]->get_limit());

            auto& pc = tab_models[window]->conn();

            tab->b->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
              window));

            tab->btn_reload->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_reload_table_clicked),
              window));

            tab->btn_prev->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_prev_results_page_clicked),
              window));

            tab->btn_next->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_next_results_page_clicked),
              window));

            std::shared_ptr<san::QueryResult> result = pc.run_query(tab_models[window]->get_query());

            std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;

            for (const auto& column : result->columns) {
                Gtk::TreeModelColumn<Glib::ustring> col;

                cols[column.column_name] = col;

                tab->cr->add(cols[column.column_name]);
                tab->tree->append_column(san::util::replace_all(column.column_name, "_", "__") + "\n" + column.data_type, cols[column.column_name]);
            }

            tab->list_store = Gtk::ListStore::create(*tab->cr);
            tab->tree->set_model(tab->list_store);

            for (const auto& row : result->data) {
                Gtk::TreeModel::Row r = *(tab->list_store->append());

                int i = 0;

                for (const auto& c : result->columns) {
                    r[cols[c.column_name]] = row[i];

                    i++;
                }
            }

            notebook.append_page(*window, *(tab->hb));

            show_all_children();

            notebook.next_page();

            const std::vector<Gtk::TreeViewColumn*> tree_columns = tab->tree->get_columns();

            for (const auto column : tree_columns) {
                column->set_resizable();
            }
        }
    }

    void MainWindow::on_submit_query_clicked
    (Gtk::ScrolledWindow* tree_scrolled_window, Glib::RefPtr<Gsv::Buffer>& buffer)
    {
        const std::string query = buffer->get_text();

        std::cout << query << std::endl;

        const TabModel& tab = tab_model(tree_scrolled_window);
        auto& pc = tab.conn();

        std::shared_ptr<san::QueryResult> result = pc.run_query(query);

        san::AbstractTab& at = get_tab(tree_scrolled_window);
        san::Tab& tab2 = static_cast<san::Tab&>(at);

        std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;

        for (const auto& column : result->columns) {
            Gtk::TreeModelColumn<Glib::ustring> col;

            cols[column.column_name] = col;

            tab2.cr.add(cols[column.column_name]);
            tab2.tree->append_column(san::util::replace_all(column.column_name, "_", "__") + "\n" + column.data_type, cols[column.column_name]);
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
}
