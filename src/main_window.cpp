#include <iostream>

#include "main_window.hpp"
#include "util.hpp"
#include "model/tab_model.hpp"

#include <glib.h>

namespace san
{
    MainWindow::MainWindow()
        : win_connections(nullptr),
          main_box(Gtk::ORIENTATION_VERTICAL),
          box_browser(Gtk::ORIENTATION_VERTICAL)
    {
        Glib::init();

        g_debug("Debugging is on");

        set_title("Postgres Client");

        const Glib::ustring config_home = Glib::get_user_config_dir();

        gchar* tmp = g_build_filename(config_home.c_str(), "sancho", NULL);

        std::string path = tmp;

        gint r = g_mkdir_with_parents(path.c_str(), 0755);

        if (r == 0) {
            std::cout << "Created\n";
        }

        g_free(tmp);

        tmp = g_build_filename(path.c_str(), "connections", NULL);

        san::Connections::instance()->CONN_PATH = tmp;
        san::Connections::instance()->init_connections();

        g_free(tmp);

        set_border_width(0);
        set_default_size(1200, 800);

        add(main_box);

        refresh_connections_list();

        combo_connections.signal_changed().connect(sigc::mem_fun(*this,
            &MainWindow::on_connection_changed));

        Gsv::init();

        browser_scrolled_window.add(browser);
        browser_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        combo_connections.set_title("Active connection");

        // expand, fill, padding
        box_browser.pack_start(combo_connections, false, false, 4);
        box_browser.pack_start(browser_scrolled_window);

        paned.pack1(box_browser);

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

        res_builder->get_widget_derived("win_new_connection", win_connections);

        if (win_connections) {
            win_connections->signal_hide().connect(
                sigc::mem_fun(*this, &MainWindow::on_win_connections_hide));
            win_connections->set_transient_for(*this);
            win_connections->set_modal();
        }

        show_all_children();

        int w, h;
        get_size(w, h);
        paned.set_position(0.21 * w);
    }

    void MainWindow::on_action_file_quit()
    {
        hide();
    }

    void MainWindow::on_action_file_new()
    {
        if (win_connections) {
            win_connections->show();
        }
    }

    void MainWindow::on_win_connections_hide()
    {
        refresh_connections_list();
    }

    san::SimpleTabModel& MainWindow::get_simple_tab_model(Gtk::ScrolledWindow* win)
    {
        return static_cast<san::SimpleTabModel&>(*(tab_models[win]));
    }

    san::QueryTabModel& MainWindow::get_query_tab_model(Gtk::ScrolledWindow* win)
    {
        return static_cast<san::QueryTabModel&>(*(tab_models[win]));
    }

    san::SimpleTab& MainWindow::get_simple_tab(Gtk::ScrolledWindow* win)
    {
        return static_cast<san::SimpleTab&>(*(tabs[win]));
    }

    san::QueryTab& MainWindow::get_query_tab(Gtk::ScrolledWindow* win)
    {
        return static_cast<san::QueryTab&>(*(tabs[win]));
    }

    void MainWindow::on_results_column_clicked(
        Gtk::ScrolledWindow* window,
        Gtk::TreeViewColumn* col
    )
    {
        san::SimpleTab& tab = get_simple_tab(window);
        san::SimpleTabModel& model = get_simple_tab_model(window);

        model.set_sort(tab.col_names[col]);

        load_list_results(window);
    }

	void MainWindow::load_list_results(Gtk::ScrolledWindow* window)
	{
        auto& pc = tab_models[window]->conn();

        san::SimpleTab& tab = get_simple_tab(window);
        san::SimpleTabModel& model = get_simple_tab_model(window);

        std::shared_ptr<san::QueryResult> result
            = pc.run_query(model.get_query());

        tab.col_names.clear();
        tab.tree->remove_all_columns();

        tab.cr = std::make_unique<Gtk::TreeModel::ColumnRecord>();
        model.col_color = std::make_unique<Gtk::TreeModelColumn<Gdk::RGBA>>();
        tab.cr->add(*model.col_color);

        Gtk::TreeViewColumn* sorted_col = nullptr;

        Gtk::TreeModelColumn<Glib::ustring> col;
        model.cols["#"] = col;

        tab.cr->add(model.cols["#"]);

        Gtk::TreeViewColumn* tree_view_column
            = Gtk::manage(new Gtk::TreeViewColumn("#", model.cols["#"]));
        tab.tree->append_column(*tree_view_column);
        tab.col_names[tree_view_column] = "#";

        tree_view_column->add_attribute(*tree_view_column->get_first_cell(),
                                        "background-rgba",
                                        *model.col_color);

        // Column records must be added before setting the model
        for (const auto& column : result->columns) {
            Gtk::TreeModelColumn<Glib::ustring> col;

            model.cols[column.column_name] = col;

            tab.cr->add(model.cols[column.column_name]);
        }

        tab.list_store = Gtk::ListStore::create(*(tab.cr));
        tab.tree->set_model(tab.list_store);

        for (const auto& column : result->columns) {
            const std::string escaped_column_name
                = san::util::replace_all(column.column_name, "_", "__");
            const std::string data_type = column.data_type;
            const std::string column_name = escaped_column_name + "\n" + data_type;

            int c = tab.tree->append_column_editable(column_name, model.cols[column.column_name]);
            Gtk::TreeViewColumn* tree_view_column = tab.tree->get_columns()[c - 1];

            tree_view_column->set_resizable();

            auto cren = tree_view_column->get_first_cell();

            tree_view_column->add_attribute(*cren, "background-rgba", *model.col_color);

            Gtk::CellRendererText* crtext = dynamic_cast<Gtk::CellRendererText*>(cren);

            if (crtext) {
                auto signal_edited_slot = sigc::bind<san::SimpleTab*, san::SimpleTabModel*, const std::string>(
sigc::mem_fun(*this, &MainWindow::cellrenderer_validated_on_edited), &tab, &model, column.column_name);

                crtext->signal_edited().connect(signal_edited_slot);

                auto editing_started_slot = sigc::bind<san::SimpleTab*, san::SimpleTabModel*, const std::string>(
sigc::mem_fun(*this, &MainWindow::cellrenderer_validated_on_editing_started), &tab, &model, column.column_name);

                crtext->signal_editing_started().connect(editing_started_slot);
            }

            tree_view_column->signal_clicked().connect
                (sigc::bind<Gtk::ScrolledWindow*, Gtk::TreeViewColumn*>
                 (sigc::mem_fun(*this, &MainWindow::on_results_column_clicked),
                  window, tree_view_column));

            if (column.column_name == model.get_sort_column()) {
                sorted_col = tree_view_column;
            }

            tab.col_names[tree_view_column] = column.column_name;
        }

        tab.tree->set_headers_clickable();

        handle_results_sort(static_cast<const san::SimpleTabModel*>(&model),
                            sorted_col);

        unsigned row_i = 1;

        for (const auto& row : result->data) {
            Gtk::TreeModel::Row r = *(tab.list_store->append());

            int i = 0;

            r[model.cols["#"]] = std::to_string(row_i++);
            r[*model.col_color] = model.col_white;

            for (const auto& c : result->columns) {
                r[model.cols[c.column_name]] = row[i];

                i++;
            }
        }
    }

	void MainWindow::load_query_results(Gtk::ScrolledWindow* window)
	{
        auto& pc = tab_models[window]->conn();

        san::QueryTab& tab = get_query_tab(window);
        san::QueryTabModel& model = get_query_tab_model(window);

        std::shared_ptr<san::QueryResult> result
            = pc.run_query(model.get_query());

        tab.col_names.clear();
        tab.tree->remove_all_columns();

        tab.cr = std::make_unique<Gtk::TreeModel::ColumnRecord>();

        Gtk::TreeModelColumn<Glib::ustring> col;
        model.cols["#"] = col;

        tab.cr->add(model.cols["#"]);

        Gtk::TreeViewColumn* tree_view_column
            = Gtk::manage(new Gtk::TreeViewColumn("#", model.cols["#"]));
        tab.tree->append_column(*tree_view_column);
        tab.col_names[tree_view_column] = "#";

        // Column records must be added before setting the model
        for (const auto& column : result->columns) {
            Gtk::TreeModelColumn<Glib::ustring> col;

            model.cols[column.column_name] = col;

            tab.cr->add(model.cols[column.column_name]);
        }

        tab.list_store = Gtk::ListStore::create(*(tab.cr));
        tab.tree->set_model(tab.list_store);

        for (const auto& column : result->columns) {
            const std::string escaped_column_name
                = san::util::replace_all(column.column_name, "_", "__");
            const std::string data_type = column.data_type;
            const std::string column_name = escaped_column_name + "\n" + data_type;

            int c = tab.tree->append_column(column_name, model.cols[column.column_name]);
            Gtk::TreeViewColumn* tree_view_column = tab.tree->get_columns()[c - 1];

            tree_view_column->set_resizable();

            tab.col_names[tree_view_column] = column.column_name;
        }

        unsigned row_i = 1;

        for (const auto& row : result->data) {
            Gtk::TreeModel::Row r = *(tab.list_store->append());

            int i = 0;

            r[model.cols["#"]] = std::to_string(row_i++);

            for (const auto& c : result->columns) {
                r[model.cols[c.column_name]] = row[i];

                i++;
            }
        }
	}

    void MainWindow::on_prev_results_page_clicked(Gtk::ScrolledWindow* window)
    {
        san::SimpleTab& tab = get_simple_tab(window);
        san::SimpleTabModel& tab_model = get_simple_tab_model(window);

        tab_model.set_offset(tab.number_offset->get_text());
        tab_model.set_limit(tab.number_limit->get_text());

        tab_model.prev_page();

        tab.number_offset->set_text(tab_model.get_offset());
        tab.number_limit->set_text(tab_model.get_limit());

        load_list_results(window);
    }

    void MainWindow::on_next_results_page_clicked(Gtk::ScrolledWindow* window)
    {
        san::SimpleTab& tab = get_simple_tab(window);
        san::SimpleTabModel& tab_model = get_simple_tab_model(window);

        tab_model.set_offset(tab.number_offset->get_text());
        tab_model.set_limit(tab.number_limit->get_text());

        tab_model.next_page();

        tab.number_offset->set_text(tab_model.get_offset());
        tab.number_limit->set_text(tab_model.get_limit());

        load_list_results(window);
    }

    void MainWindow::on_reload_table_clicked(Gtk::ScrolledWindow* window)
    {
        san::SimpleTab& tab = get_simple_tab(window);
        san::SimpleTabModel& tab_model = get_simple_tab_model(window);

        tab_model.set_offset(tab.number_offset->get_text());
        tab_model.set_limit(tab.number_limit->get_text());

        tab.number_offset->set_text(tab_model.get_offset());
        tab.number_limit->set_text(tab_model.get_limit());

        load_list_results(window);
    }

    void MainWindow::on_insert_row_clicked(Gtk::ScrolledWindow* window)
    {
        san::SimpleTab& tab = get_simple_tab(window);
        san::SimpleTabModel& model = get_simple_tab_model(window);

        Gtk::TreeModel::Children children = tab.list_store->children();

        Gtk::TreeModel::Row r = *(tab.list_store->append());
        r[model.cols["#"]] = std::to_string(children.size());
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
        const auto current_connection = san::Connections::instance()->current_connection;

        if (! current_connection) return;

        auto tab = std::make_shared<san::QueryTab>();

        Gtk::ScrolledWindow* window = tab->tree_scrolled_window;

        tab->b->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
              window));

        tab->btn1->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*, Glib::RefPtr<Gsv::Buffer> >
             (sigc::mem_fun(*this, &MainWindow::on_submit_query_clicked),
              window, tab->buffer));

        tabs[window] = tab;

        tab_models[window] = std::make_shared<san::QueryTabModel>(current_connection);

        notebook.append_page(*window, *(tab->hb));

        show_all_children();

        notebook.next_page();
    }

    void MainWindow::on_browser_row_activated(const Gtk::TreeModel::Path& path,
                                              Gtk::TreeViewColumn*)
    {
        Gtk::TreeModel::iterator iter = browser_store->get_iter(path);

        if (! iter)
            return;

        Gtk::TreeModel::Row current_row = *iter;

        Glib::ustring table_name = current_row[browser_model.table];

        auto tab = std::make_shared<san::SimpleTab>();

        Gtk::ScrolledWindow* window = tab->tree_scrolled_window;

        tabs[window] = (tab);

        const auto current_connection = san::Connections::instance()->current_connection;

        if (! current_connection) return;

        auto shared_tab_model
            = std::make_shared<san::SimpleTabModel>(
            current_connection, table_name);

        tab_models[window] = shared_tab_model;

        san::SimpleTab* simple_tab = tab.get();
        san::SimpleTabModel* simple_tab_model = shared_tab_model.get();

        tab->btn_accept->signal_clicked().connect
            (sigc::bind<san::SimpleTab*, san::SimpleTabModel*>
             (sigc::mem_fun(*this, &MainWindow::on_btn_accept_changes_clicked),
              simple_tab, simple_tab_model));

        tab->b->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
              window));

        tab->btn_reload->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_reload_table_clicked),
              window));

        tab->btn_insert->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_insert_row_clicked),
              window));

        tab->btn_prev->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_prev_results_page_clicked),
              window));

        tab->btn_next->signal_clicked().connect
            (sigc::bind<Gtk::ScrolledWindow*>
             (sigc::mem_fun(*this, &MainWindow::on_next_results_page_clicked),
              window));

        load_list_results(window);
        notebook.append_page(*window, *(tab->hb));

        show_all_children();
        notebook.next_page();
    }

    void MainWindow::on_submit_query_clicked
    (Gtk::ScrolledWindow* tree_scrolled_window, Glib::RefPtr<Gsv::Buffer>& buffer)
    {
        const std::string query = buffer->get_text();

        std::cout << query << std::endl;

        auto& model = get_query_tab_model(tree_scrolled_window);
        model.query = query;

        load_query_results(tree_scrolled_window);
    }

    void MainWindow::handle_results_sort(const san::SimpleTabModel* model,
                                         Gtk::TreeViewColumn* sorted_col)
    {
        if (sorted_col && model->is_sorted()) {
            sorted_col->set_sort_indicator(true);
            sorted_col->set_sort_order(model->get_sort_type());
        }
    }

    void MainWindow::refresh_connections_list()
    {
        combo_connections.remove_all();

        for (const auto& details : san::Connections::instance()->get_connections()) {
            combo_connections.append(details.second->name);
        }
    }

    void MainWindow::on_connection_changed()
    {
        browser_store->clear();

        const Glib::ustring connection_name = combo_connections.get_active_text();

        if (! san::Connections::instance()->exists(connection_name))
            return;

        const auto current_connection
            = san::Connections::instance()->get_connection(connection_name);

        std::shared_ptr<san::PostgresConnection> pc
            = std::make_shared<san::PostgresConnection>(current_connection);
        pc->init_connection();

        san::Connections::instance()->current_connection = current_connection;

        const std::vector<std::string>& tables = pc->get_db_tables();

        Gtk::TreeModel::Row row = *(browser_store->append());
        row[browser_model.table] = "Tables";

        for (const std::string& table_name : tables) {
            Gtk::TreeModel::Row table_row = *(browser_store->append(row.children()));
            table_row[browser_model.table] = table_name;
        }

        browser.expand_all();
    }

    void MainWindow::cellrenderer_validated_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path, san::SimpleTab* tab, san::SimpleTabModel* model, const std::string& column_name)
    {
        Gtk::TreeModel::iterator iter = tab->list_store->get_iter(path);

        if (! iter) return;

        Gtk::TreeModel::Row row = *iter;

        for (const auto& key : model->get_primary_key()) {
            model->pk_hist[key.column_name] = row[model->cols[key.column_name]];;
        }

        if (model->is_part_of_pk(column_name)) {
            model->pk_changes[column_name].first = row[model->cols[column_name]];
        }
    }

    void MainWindow::cellrenderer_validated_on_edited(const Glib::ustring& path, const Glib::ustring& new_text, san::SimpleTab* tab, san::SimpleTabModel* model, const std::string& column_name)
    {
        Gtk::TreeModel::iterator iter = tab->list_store->get_iter(path);

        if (! iter) return;

        Gtk::TreeModel::Row row = *iter;

        // TODO: model->pk_hist

        if (model->pk_changes.size() && model->is_part_of_pk(column_name) && new_text != model->pk_changes[column_name].first) {
            Gtk::MessageDialog dialog(*this, "Are you sure you want to change the primary key?", false /* use_markup */, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
            std::stringstream q;

            q << "Primary key column '"
              << column_name
              << "' will permanently change from '"
              << model->pk_changes[column_name].first
              << "' to '"
              << new_text << "'";

            dialog.set_secondary_text(q.str());

            int result = dialog.run();

            if (result == Gtk::RESPONSE_OK) {
                model->pk_changes[column_name].second = new_text;
                model->accept_pk_change();
            } else {
                row[model->cols[column_name]] = model->pk_changes[column_name].first;
            }

            return;
        }

        if (! model->is_part_of_pk(column_name)) {
            model->map_test[model->pk_hist][column_name] = new_text;
            model->pk_hist.clear();
        }

        row[*model->col_color] = model->col_highlighted;
    }

    void MainWindow::on_btn_accept_changes_clicked(san::SimpleTab* tab, san::SimpleTabModel* model)
    {
        model->accept_changes();

        // Reset the background color of highlighted rows

        Gtk::TreeModel::Children children = tab->list_store->children();

        for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
            Gtk::TreeModel::Row row = *iter;

            row[*model->col_color] = model->col_white;
        }
    }
}
