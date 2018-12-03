#include <iostream>

#include <algorithm>
#include <glib.h>

#include "../../string.hpp"
#include "../../system/files.hpp"
#include "main_window.hpp"

namespace sancho {
namespace ui {
namespace gtk {
MainWindow::MainWindow()
    : win_connections(nullptr), main_box(Gtk::ORIENTATION_VERTICAL),
      box_main_pane(Gtk::ORIENTATION_VERTICAL),
      box_browser(Gtk::ORIENTATION_VERTICAL) {
    Glib::init();

    g_debug("SanchoSQL %s\tGlib %d.%d.%d",
            sancho::config::current_version.c_str(), GLIB_MAJOR_VERSION,
            GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);

    sancho::db::Connections::instance()->CONN_PATH =
        sancho::files::get_connections_file_path();
    sancho::db::Connections::instance()->init_connections();

    set_title("SanchoSQL");
    set_border_width(0);
    set_default_size(1200, 800);

    add(main_box);

    refresh_connections_list();

    combo_connections.signal_changed().connect(
        sigc::mem_fun(*this, &MainWindow::on_connection_changed));
    combo_schemas.signal_changed().connect(
        sigc::mem_fun(*this, &MainWindow::on_schema_changed));

    Gsv::init();

    browser_scrolled_window.add(browser);
    browser_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC,
                                       Gtk::POLICY_AUTOMATIC);

    popup_item_refresh_browser =
        Gtk::manage(new Gtk::MenuItem("_Refresh", true));
    auto slot_browser_refresh =
        sigc::mem_fun(*this, &MainWindow::on_browser_refresh_clicked);
    popup_item_refresh_browser->signal_activate().connect(slot_browser_refresh);

    popup_browser_header.append(*popup_item_refresh_browser);
    popup_browser_header.accelerate(*this);
    popup_browser_header.show_all();

    auto slot_button_released =
        sigc::mem_fun(*this, &MainWindow::on_browser_button_released);

    browser.signal_button_release_event().connect(slot_button_released);

    combo_connections.set_title("Active connection");
    combo_schemas.set_title("Schema");

    // expand, fill, padding
    label_connections.set_text("Connection:");
    label_connections.set_halign(Gtk::Align::ALIGN_START);
    label_schemas.set_text("Schema:");
    label_schemas.set_halign(Gtk::Align::ALIGN_START);

    box_browser.pack_start(label_connections, false, false, 0);
    box_browser.pack_start(combo_connections, false, false, 4);
    box_browser.pack_start(label_schemas, false, false, 0);
    box_browser.pack_start(combo_schemas, false, false, 4);
    box_browser.pack_start(browser_scrolled_window);

    paned.pack1(box_browser);

    notebook_scrolled_window.add(notebook);
    box_main_pane.pack_start(notebook_scrolled_window);
    paned.pack2(box_main_pane);
    notebook.set_scrollable(true);
    notebook.popup_enable();

    browser_store = Gtk::TreeStore::create(browser_model);
    browser.set_model(browser_store);
    browser.set_headers_visible(false);

    browser.append_column("Table", browser_model.table);

    browser.signal_row_activated().connect(
        sigc::mem_fun(*this, &MainWindow::on_browser_row_activated));

    menu_group = Gio::SimpleActionGroup::create();

    menu_group->add_action(
        "new_conn", sigc::mem_fun(*this, &MainWindow::on_action_file_new));

    menu_group->add_action(
        "quit", sigc::mem_fun(*this, &MainWindow::on_action_file_quit));

    insert_action_group("file", menu_group);

    res_builder = Gtk::Builder::create();

    try {
        res_builder->add_from_resource("/res/toolbar.glade");
        res_builder->add_from_resource("/res/window_new_connection.glade");
        res_builder->add_from_resource("/res/dashboard.glade");
    } catch (const Glib::Error &e) {
        g_critical("Building menus and toolbar failed: %s", e.what().c_str());
        return;
    }

    Gtk::ToolButton *toolbutton_sql;
    res_builder->get_widget("toolbutton_sql", toolbutton_sql);

    toolbutton_sql->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_open_sql_editor_clicked));

    auto theme = Gtk::IconTheme::get_default();
    // theme->add_resource_path("/icons/64x64/res/icons");
    theme->add_resource_path("/icons/512x512/res/icons");

    Gtk::Image *icon = Gtk::manage(new Gtk::Image);
    icon->set_from_resource("/icons/512x512/res/icons/sanchosql.png");

    set_icon(icon->get_pixbuf());
    set_default_icon(icon->get_pixbuf());

    add_accel_group(main_menu.group);
    group = main_menu.group;

    main_box.pack_start(*main_menu.menu, Gtk::PACK_SHRINK);

    main_menu.menu_item_connections->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::on_action_file_new));

    main_menu.menu_item_sql_editor->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::on_open_sql_editor_clicked));

    main_menu.menu_item_quit->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::on_action_file_quit));

    main_menu.menu_item_about->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::on_action_file_about));

    Gtk::Toolbar *toolbar = nullptr;
    res_builder->get_widget("toolbar", toolbar);

    if (toolbar)
        main_box.pack_start(*toolbar, Gtk::PACK_SHRINK);
    else
        g_warning("Toolbar not found");

    main_box.pack_start(paned);

    box_dashboard = nullptr;
    res_builder->get_widget("box_dashboard", box_dashboard);

    if (box_dashboard) {
        box_main_pane.add(*box_dashboard);
    }

    tree_connections = nullptr;
    res_builder->get_widget("tree_connections", tree_connections);
    res_builder->get_widget("btn_dashboard_connections",
                            btn_dashboard_connections);

    if (btn_dashboard_connections) {
        btn_dashboard_connections->signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_action_file_new));
    }

    if (tree_connections) {
        store_connections = Gtk::ListStore::create(connections_model);
        tree_connections->set_model(store_connections);
        tree_connections->append_column("Name", connections_model.name);
        tree_connections->append_column("Host", connections_model.host);
        tree_connections->append_column("Database", connections_model.dbname);
        tree_connections->append_column("User", connections_model.user);

        refresh_tree_connections();
    }

    res_builder->get_widget_derived("win_new_connection", win_connections);

    if (win_connections) {
        win_connections->signal_hide().connect(
            sigc::mem_fun(*this, &MainWindow::on_win_connections_hide));
        win_connections->set_transient_for(*this);
        win_connections->set_modal();
    }

    add_events(Gdk::KEY_PRESS_MASK);

    show_all_children();

    notebook_scrolled_window.hide();

    int w, h;
    get_size(w, h);
    paned.set_position(0.21 * w);
}

void MainWindow::on_action_file_quit() { hide(); }

void MainWindow::on_action_file_new() {
    if (win_connections) {
        win_connections->show();
    }
}

void MainWindow::on_action_file_about() {
    std::stringstream license;

    license
        << "SanchoSQL is provided as-is. You are using it at your own risk. "
        << "There is no warranty. ";

    Gtk::MessageDialog dialog(*this, "SanchoSQL", false /* use_markup */,
                              Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    std::stringstream q;

    q << "Database client"
      << "\n\n"
      << "Version: " << sancho::config::current_version << "\n\n"
      << "http://sanchosql.com"
      << "\n\n"
      << license.str();

    dialog.set_secondary_text(q.str());

    dialog.run();
}

void MainWindow::on_win_connections_hide() {
    refresh_connections_list();
    refresh_tree_connections();
}

sancho::db::SimpleTabModel &
MainWindow::get_simple_tab_model(Gtk::ScrolledWindow *win) {
    return static_cast<sancho::db::SimpleTabModel &>(*(tab_models[win]));
}

sancho::db::QueryTabModel &
MainWindow::get_query_tab_model(Gtk::ScrolledWindow *win) {
    return static_cast<sancho::db::QueryTabModel &>(*(tab_models[win]));
}

sancho::ui::gtk::SimpleTab &
MainWindow::get_simple_tab(Gtk::ScrolledWindow *win) {
    return static_cast<sancho::ui::gtk::SimpleTab &>(*(tabs[win]));
}

sancho::ui::gtk::QueryTab &MainWindow::get_query_tab(Gtk::ScrolledWindow *win) {
    return static_cast<sancho::ui::gtk::QueryTab &>(*(tabs[win]));
}

void MainWindow::on_results_column_clicked(Gtk::ScrolledWindow *window,
                                           Gtk::TreeViewColumn *col) {
    sancho::ui::gtk::SimpleTab &tab = get_simple_tab(window);
    sancho::db::SimpleTabModel &model = get_simple_tab_model(window);

    model.set_sort(tab.col_names[col]);

    load_list_results(window);
}

void MainWindow::load_list_results(Gtk::ScrolledWindow *window) {
    auto &pc = tab_models[window]->conn();

    sancho::ui::gtk::SimpleTab &tab = get_simple_tab(window);
    sancho::db::SimpleTabModel &model = get_simple_tab_model(window);

    model.set_filter(tab.entry_filter->get_text());

    std::shared_ptr<sancho::QueryResult> result =
        pc.run_query(sancho::QueryType::NonTransaction, model.get_query(),
                     model.get_columns_query());

    sancho::ui::gtk::insert_log_message(tab.log_buffer, result->get_message());

    if (result->success) {
        tab.data_scrolled_window->show();
    } else {
        tab.data_scrolled_window->hide();
        return;
    }

    tab.col_names.clear();
    tab.tree->remove_all_columns();
    tab.number_offset->set_text(std::to_string(model.get_offset()));
    tab.number_limit->set_text(std::to_string(model.get_limit()));

    tab.btn_prev->set_sensitive(model.get_offset() > 0);
    tab.btn_next->set_sensitive(result->data.size() >= model.get_limit());

    tab.tree->set_grid_lines(
        Gtk::TreeViewGridLines::TREE_VIEW_GRID_LINES_VERTICAL);

    tab.cr = std::make_unique<Gtk::TreeModel::ColumnRecord>();
    model.col_color = std::make_unique<Gtk::TreeModelColumn<Gdk::RGBA>>();
    tab.cr->add(*model.col_color);

    Gtk::TreeViewColumn *sorted_col = nullptr;

    Gtk::TreeModelColumn<Glib::ustring> col;
    model.cols["#"] = col;

    tab.cr->add(model.cols["#"]);

    Gtk::TreeViewColumn *tree_view_column =
        Gtk::manage(new Gtk::TreeViewColumn("", model.cols["#"]));
    tab.tree->append_column(*tree_view_column);
    tab.col_names[tree_view_column] = "";

    tree_view_column->add_attribute(*tree_view_column->get_first_cell(),
                                    "background-rgba", *model.col_color);

    std::vector<std::string> column_mask_list = sancho::string::split(
        sancho::string::replace_all(tab.entry_column_mask->get_text(), " ", ""),
        ",");

    std::set<std::string> column_mask;

    std::copy_if(column_mask_list.cbegin(), column_mask_list.cend(),
                 std::inserter(column_mask, column_mask.end()),
                 [](const auto &s) { return !s.empty(); });

    if (column_mask.empty()) {
        for (const auto &column : result->columns) {
            column_mask.insert(column.get_column_name());
        }
    }

    // Add Primary Keys to the list of columns (mask)
    for (const auto &column : result->columns) {
        const std::string col = column.get_column_name();

        if (model.is_part_of_pk(col)) {
            column_mask.insert(col);
        }
    }

    // Column records must be added before setting the model
    for (const auto &column : result->columns) {
        const std::string column_name = column.get_column_name();

        if (column_mask.find(column_name) == column_mask.end()) {
            continue;
        }

        Gtk::TreeModelColumn<Glib::ustring> col;

        model.cols[column_name] = col;

        tab.cr->add(model.cols[column_name]);
    }

    tab.list_store = Gtk::ListStore::create(*(tab.cr));
    tab.tree->set_model(tab.list_store);

    for (const auto &column : result->columns) {
        const std::string column_name2 = column.get_column_name();

        if (column_mask.find(column_name2) == column_mask.end()) {
            continue;
        }

        const std::string escaped_column_name =
            sancho::string::replace_all(column.get_column_name(), "_", "__");
        const std::string data_type = column.get_data_type();

        std::stringstream column_name;

        if (model.is_part_of_pk(column.get_column_name())) {
            column_name << "(PK) ";
        }

        column_name << escaped_column_name << "\n" << data_type;

        if (!column.get_char_length().empty()) {
            column_name << " (" << column.get_char_length() << ")";
        }

        if (column.is_nullable()) {
            column_name << " [N]";
        }

        const int c = tab.tree->append_column_editable(
            column_name.str(), model.cols[column.get_column_name()]);

        Gtk::TreeViewColumn *tree_view_column = tab.tree->get_columns()[c - 1];

        tree_view_column->set_resizable();
        tree_view_column->set_expand(true);

        if (data_type == "text") {
            tree_view_column->set_fixed_width(200);
        } else if (data_type == "json") {
            tree_view_column->set_fixed_width(150);
        }

        auto cren = tree_view_column->get_first_cell();

        tree_view_column->add_attribute(*cren, "background-rgba",
                                        *model.col_color);

        Gtk::CellRendererText *crtext =
            dynamic_cast<Gtk::CellRendererText *>(cren);

        if (crtext) {
            auto signal_edited_slot =
                sigc::bind<sancho::ui::gtk::SimpleTab *,
                           sancho::db::SimpleTabModel *, const std::string>(
                    sigc::mem_fun(
                        *this, &MainWindow::cellrenderer_validated_on_edited),
                    &tab, &model, column.get_column_name());

            crtext->signal_edited().connect(signal_edited_slot);

            auto editing_started_slot =
                sigc::bind<sancho::ui::gtk::SimpleTab *,
                           sancho::db::SimpleTabModel *, const std::string>(
                    sigc::mem_fun(
                        *this,
                        &MainWindow::cellrenderer_validated_on_editing_started),
                    &tab, &model, column.get_column_name());

            crtext->signal_editing_started().connect(editing_started_slot);
        }

        tree_view_column->signal_clicked().connect(
            sigc::bind<Gtk::ScrolledWindow *, Gtk::TreeViewColumn *>(
                sigc::mem_fun(*this, &MainWindow::on_results_column_clicked),
                window, tree_view_column));

        if (column.get_column_name() == model.get_sort_column()) {
            sorted_col = tree_view_column;
        }

        tab.col_names[tree_view_column] = column.get_column_name();
    }

    tab.tree->set_headers_clickable();

    handle_results_sort(static_cast<const sancho::db::SimpleTabModel *>(&model),
                        sorted_col);

    unsigned row_i = 1;

    model.db_rows_cnt = result->data.size();

    for (const auto &row : result->data) {
        Gtk::TreeModel::Row r = *(tab.list_store->append());

        int i = 0;

        r[model.cols["#"]] = std::to_string(row_i++);
        r[*model.col_color] = model.col_white;

        for (const auto &c : result->columns) {
            const std::string column_name = c.get_column_name();

            if (column_mask.find(column_name) == column_mask.end()) {
                i++;
                continue;
            }

            r[model.cols[c.get_column_name()]] =
                sancho::string::escape_db_data(row[i]);

            i++;
        }
    }
}

void MainWindow::load_query_results(Gtk::ScrolledWindow *window) {
    auto &pc = tab_models[window]->conn();

    sancho::ui::gtk::QueryTab &tab = get_query_tab(window);
    sancho::db::QueryTabModel &model = get_query_tab_model(window);

    std::shared_ptr<sancho::QueryResult> result =
        pc.run_query(sancho::QueryType::NonTransaction, model.get_query());

    tab.data_scrolled_window->set_visible(result->show_results);

    sancho::ui::gtk::insert_log_message(tab.log_buffer, result->get_message());

    if (!result->success) {
        tab.data_scrolled_window->hide();

        return;
    }

    tab.col_names.clear();
    tab.tree->remove_all_columns();

    tab.cr = std::make_unique<Gtk::TreeModel::ColumnRecord>();

    Gtk::TreeModelColumn<Glib::ustring> col;
    model.cols["#"] = col;

    tab.cr->add(model.cols["#"]);

    Gtk::TreeViewColumn *tree_view_column =
        Gtk::manage(new Gtk::TreeViewColumn("", model.cols["#"]));
    tab.tree->append_column(*tree_view_column);
    tab.col_names[tree_view_column] = "";

    // Column records must be added before setting the model
    for (const auto &column : result->columns) {
        Gtk::TreeModelColumn<Glib::ustring> col;

        model.cols[column.get_column_name()] = col;

        tab.cr->add(model.cols[column.get_column_name()]);
    }

    tab.list_store = Gtk::ListStore::create(*(tab.cr));
    tab.tree->set_model(tab.list_store);

    for (const auto &column : result->columns) {
        const std::string escaped_column_name =
            sancho::string::replace_all(column.get_column_name(), "_", "__");
        const std::string data_type = column.get_data_type();
        const std::string column_name = escaped_column_name + "\n" + data_type;

        int c = tab.tree->append_column(column_name,
                                        model.cols[column.get_column_name()]);
        Gtk::TreeViewColumn *tree_view_column = tab.tree->get_columns()[c - 1];

        tree_view_column->set_resizable();

        tab.col_names[tree_view_column] = column.get_column_name();
    }

    unsigned row_i = 1;

    for (const auto &row : result->data) {
        Gtk::TreeModel::Row r = *(tab.list_store->append());

        int i = 0;

        r[model.cols["#"]] = std::to_string(row_i++);

        for (const auto &c : result->columns) {
            r[model.cols[c.get_column_name()]] =
                sancho::string::escape_db_data(row[i]);

            i++;
        }
    }

    int w, h;
    get_size(w, h);
    tab.paned_source.set_position(0.2 * h);
    tab.paned_results.set_position(0.55 * h);
}

void MainWindow::on_prev_results_page_clicked(Gtk::ScrolledWindow *window) {
    sancho::ui::gtk::SimpleTab &tab = get_simple_tab(window);
    sancho::db::SimpleTabModel &tab_model = get_simple_tab_model(window);

    tab_model.set_offset(tab.number_offset->get_text());
    tab_model.set_limit(tab.number_limit->get_text());

    tab_model.prev_page();

    tab.number_offset->set_text(std::to_string(tab_model.get_offset()));
    tab.number_limit->set_text(std::to_string(tab_model.get_limit()));

    load_list_results(window);
}

void MainWindow::on_next_results_page_clicked(Gtk::ScrolledWindow *window) {
    sancho::ui::gtk::SimpleTab &tab = get_simple_tab(window);
    sancho::db::SimpleTabModel &tab_model = get_simple_tab_model(window);

    tab_model.set_offset(tab.number_offset->get_text());
    tab_model.set_limit(tab.number_limit->get_text());

    tab_model.next_page();

    tab.number_offset->set_text(std::to_string(tab_model.get_offset()));
    tab.number_limit->set_text(std::to_string(tab_model.get_limit()));

    load_list_results(window);
}

void MainWindow::on_reload_table_clicked(Gtk::ScrolledWindow *window) {
    sancho::ui::gtk::SimpleTab &tab = get_simple_tab(window);
    sancho::db::SimpleTabModel &tab_model = get_simple_tab_model(window);

    tab_model.set_offset(tab.number_offset->get_text());
    tab_model.set_limit(tab.number_limit->get_text());

    tab.number_offset->set_text(std::to_string(tab_model.get_offset()));
    tab.number_limit->set_text(std::to_string(tab_model.get_limit()));

    load_list_results(window);
}

void MainWindow::on_insert_row_clicked(Gtk::ScrolledWindow *window) {
    sancho::ui::gtk::SimpleTab &tab = get_simple_tab(window);
    sancho::db::SimpleTabModel &model = get_simple_tab_model(window);

    Gtk::TreeModel::Children children = tab.list_store->children();

    Gtk::TreeModel::Row r = *(tab.list_store->append());
    r[model.cols["#"]] = std::to_string(children.size());

    r[*model.col_color] = model.col_inserted;
}

bool MainWindow::on_tab_button_released(GdkEventButton *button_event,
                                        Gtk::ScrolledWindow *window) {
    if ((button_event->type == GDK_BUTTON_RELEASE) &&
        (button_event->button == 2)) {
        on_tab_close_button_clicked(window);
    }

    return true;
}

void MainWindow::on_tab_close_button_clicked(Gtk::ScrolledWindow *tree) {
    if (tab_models.find(tree) == tab_models.end()) {
        std::cerr << "Could not find connection when closing a tab model"
                  << std::endl;
    } else {
        tab_models.erase(tab_models.find(tree));
    }

    if (tabs.find(tree) == tabs.end()) {
        std::cerr << "Could not find connection when closing a tab"
                  << std::endl;
    } else {
        tabs.erase(tabs.find(tree));
    }

    notebook.remove_page(*tree);

    const unsigned models = tab_models.size();
    const unsigned tabs_n = tabs.size();
    const unsigned notebook_tabs = notebook.get_n_pages();

    if (!notebook_tabs) {
        notebook_scrolled_window.hide();
        box_dashboard->show();
    }

    if (tabs_n != models || tabs_n != notebook_tabs) {
        g_warning("Tabs %d != Models %d != Notebook tabs %d", tabs_n, models,
                  notebook_tabs);
    } else {
        g_debug("Correctly removed tab");
    }
}

void MainWindow::on_open_sql_editor_clicked() {
    const auto current_connection =
        sancho::db::Connections::instance()->current_connection;

    if (!current_connection) {
        show_warning("Please select a connection first!");
        return;
    };

    const Glib::ustring tab_name = current_connection->name + " (editor)";

    auto tab = std::make_shared<sancho::ui::gtk::QueryTab>(tab_name);

    Gtk::ScrolledWindow *window = tab->tree_scrolled_window;

    tab->b->signal_clicked().connect(sigc::bind<Gtk::ScrolledWindow *>(
        sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
        window));

    tab->event_box.signal_button_release_event().connect(
        sigc::bind<Gtk::ScrolledWindow *>(
            sigc::mem_fun(*this, &MainWindow::on_tab_button_released), window));

    tab->btn_execute_editor_query->signal_clicked().connect(
        sigc::bind<Gtk::ScrolledWindow *, Glib::RefPtr<Gsv::Buffer>>(
            sigc::mem_fun(*this, &MainWindow::on_submit_query_clicked), window,
            tab->buffer));

    tabs[window] = tab;

    tab_models[window] =
        std::make_shared<sancho::db::QueryTabModel>(current_connection);

    tab->show();

    notebook.append_page(*window, *(tab->hb));
    notebook.set_menu_label_text(*window, tab_name);
    notebook.set_current_page(notebook.get_n_pages() - 1);

    box_dashboard->hide();
    notebook_scrolled_window.show();

    // Hide initially because we don't have any data
    tab->data_scrolled_window->hide();

    tab->source_view->grab_focus();
}

void MainWindow::on_browser_row_activated(const Gtk::TreeModel::Path &path,
                                          Gtk::TreeViewColumn *) {
    Gtk::TreeModel::iterator iter = browser_store->get_iter(path);

    if (!iter)
        return;

    Gtk::TreeModel::Row current_row = *iter;

    if (current_row[browser_model.type] == BrowserItemType::Header) {
        return;
    }

    Glib::ustring table_name = current_row[browser_model.table];

    const auto current_connection =
        sancho::db::Connections::instance()->current_connection;

    if (!current_connection)
        return;

    const std::string &schema_name = combo_schemas.get_active_text();

    auto shared_tab_model = std::make_shared<sancho::db::SimpleTabModel>(
        current_connection, table_name, schema_name);

    auto tab = std::make_shared<sancho::ui::gtk::SimpleTab>(table_name,
                                                            shared_tab_model);
    Gtk::ScrolledWindow *window = tab->tree_scrolled_window;

    tab->tree->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

    tab->popup_item_delete_rows =
        Gtk::manage(new Gtk::MenuItem("_Delete selected row(s)", true));

    tab->popup_item_delete_rows->set_sensitive(
        shared_tab_model->has_primary_key());

    auto slot_delete =
        sigc::bind<Gtk::ScrolledWindow *, sancho::ui::gtk::SimpleTab *,
                   sancho::db::SimpleTabModel *>(
            sigc::mem_fun(*this, &MainWindow::on_menu_file_popup_generic),
            window, tab.get(), shared_tab_model.get());
    tab->popup_item_delete_rows->signal_activate().connect(slot_delete);
    tab->popup.append(*tab->popup_item_delete_rows);

    tab->popup.accelerate(*this);
    tab->popup.show_all();

    auto slot_popup =
        sigc::bind<sancho::ui::gtk::SimpleTab *, sancho::db::SimpleTabModel *>(
            sigc::mem_fun(*this, &MainWindow::on_list_press), tab.get(),
            shared_tab_model.get());

    tab->tree->signal_button_release_event().connect(slot_popup);

    tabs[window] = tab;
    tab_models[window] = shared_tab_model;

    sancho::ui::gtk::SimpleTab *simple_tab = tab.get();
    sancho::db::SimpleTabModel *simple_tab_model = shared_tab_model.get();

    tab->btn_accept->signal_clicked().connect(
        sigc::bind<sancho::ui::gtk::SimpleTab *, sancho::db::SimpleTabModel *>(
            sigc::mem_fun(*this, &MainWindow::on_btn_accept_changes_clicked),
            simple_tab, simple_tab_model));

    tab->event_box.signal_button_release_event().connect(
        sigc::bind<Gtk::ScrolledWindow *>(
            sigc::mem_fun(*this, &MainWindow::on_tab_button_released), window));

    tab->b->signal_clicked().connect(sigc::bind<Gtk::ScrolledWindow *>(
        sigc::mem_fun(*this, &MainWindow::on_tab_close_button_clicked),
        window));

    tab->btn_refresh->signal_clicked().connect(
        sigc::bind<Gtk::ScrolledWindow *>(
            sigc::mem_fun(*this, &MainWindow::on_reload_table_clicked),
            window));

    tab->btn_insert->signal_clicked().connect(sigc::bind<Gtk::ScrolledWindow *>(
        sigc::mem_fun(*this, &MainWindow::on_insert_row_clicked), window));

    tab->btn_prev->signal_clicked().connect(sigc::bind<Gtk::ScrolledWindow *>(
        sigc::mem_fun(*this, &MainWindow::on_prev_results_page_clicked),
        window));

    tab->btn_next->signal_clicked().connect(sigc::bind<Gtk::ScrolledWindow *>(
        sigc::mem_fun(*this, &MainWindow::on_next_results_page_clicked),
        window));

    load_list_results(window);

    tab->show();

    notebook.append_page(*window, *(tab->hb));
    notebook.set_menu_label_text(*window, table_name);
    notebook.set_current_page(notebook.get_n_pages() - 1);
    box_dashboard->hide();
    notebook_scrolled_window.show();

    if (shared_tab_model->has_primary_key()) {
        tab->btn_primary_key_warning->hide();
    } else {
        tab->btn_primary_key_warning->show();

        tab->btn_primary_key_warning->signal_clicked().connect(
            sigc::bind<const Glib::ustring>(
                sigc::mem_fun(*this,
                              &MainWindow::on_primary_key_warning_clicked),
                table_name));
    }
}

void MainWindow::on_submit_query_clicked(
    Gtk::ScrolledWindow *tree_scrolled_window,
    Glib::RefPtr<Gsv::Buffer> &buffer) {
    Glib::ustring query;

    if (buffer->get_has_selection()) {
        Glib::RefPtr<Gtk::TextBuffer::Mark> insert = buffer->get_insert();
        Glib::RefPtr<Gtk::TextBuffer::Mark> end = buffer->get_selection_bound();

        query = buffer->get_text(insert->get_iter(), end->get_iter());
    } else {
        Glib::RefPtr<Gtk::TextBuffer::Mark> insert = buffer->get_insert();

        query = sancho::string::get_query(buffer->get_text(),
                                          insert->get_iter().get_offset());
    }

    auto &model = get_query_tab_model(tree_scrolled_window);
    model.query = query;

    load_query_results(tree_scrolled_window);
}

void MainWindow::handle_results_sort(const sancho::db::SimpleTabModel *model,
                                     Gtk::TreeViewColumn *sorted_col) {
    if (sorted_col && model->is_sorted()) {
        sorted_col->set_sort_indicator(true);
        sorted_col->set_sort_order(model->get_sort_type());
    }
}

void MainWindow::refresh_connections_list() {
    Glib::ustring name;

    if (sancho::db::Connections::instance()->current_connection) {
        name = sancho::db::Connections::instance()->current_connection->name;
    }

    combo_connections.remove_all();

    combo_connections.append("");

    unsigned i = 1, selected = 0;

    for (const auto &details :
         sancho::db::Connections::instance()->get_connections()) {
        combo_connections.append(details.second->name);

        if (!name.empty() && details.second->name == name) {
            selected = i;
        }

        i++;
    }

    combo_connections.set_active(selected);
}

void MainWindow::on_connection_changed() {
    const auto current_connection = find_current_connection();

    if (current_connection == sancho::db::Connections::instance()->end())
        return;

    auto pc = handle_connect();

    if (!pc)
        return;

    current_connection->schemas = pc->get_schemas();

    combo_schemas.remove_all();

    unsigned selected_id = 0;

    for (const auto &schema : *current_connection->schemas) {
        combo_schemas.append(schema);

        if (schema != "public")
            selected_id++;
    }

    combo_schemas.set_active(selected_id);

    sancho::db::Connections::instance()->current_connection =
        current_connection;

    refresh_browser(pc);
}

void MainWindow::on_schema_changed() {
    auto pc = handle_connect();

    if (!pc)
        return;

    refresh_browser(pc);
}

void MainWindow::refresh_tree_connections() {
    store_connections->clear();

    for (const auto &details :
         sancho::db::Connections::instance()->get_connections()) {
        Gtk::TreeModel::Row row = *(store_connections->append());

        row[connections_model.name] = details.second->name;
        row[connections_model.host] = details.second->host;
        row[connections_model.user] = details.second->user;
        row[connections_model.dbname] = details.second->dbname;
    }
}

void MainWindow::refresh_browser(
    const std::shared_ptr<sancho::db::PostgresConnection> &pc) {
    browser_store->clear();

    const Glib::ustring schema_name = combo_schemas.get_active_text();

    const std::vector<std::string> &tables = pc->get_db_tables(schema_name);

    Gtk::TreeModel::Row row = *(browser_store->append());
    row[browser_model.table] = "Tables";
    row[browser_model.type] = BrowserItemType::Header;

    for (const std::string &table_name : tables) {
        Gtk::TreeModel::Row table_row =
            *(browser_store->append(row.children()));
        table_row[browser_model.table] = table_name;
        table_row[browser_model.type] = BrowserItemType::Table;
    }

    browser.expand_all();
}

void MainWindow::cellrenderer_validated_on_editing_started(
    Gtk::CellEditable *cell_editable, const Glib::ustring &path,
    sancho::ui::gtk::SimpleTab *tab, sancho::db::SimpleTabModel *model,
    const std::string &column_name) {
    Gtk::TreeModel::iterator iter = tab->list_store->get_iter(path);

    if (!iter)
        return;

    Gtk::TreeModel::Row row = *iter;

    for (const auto &key : model->get_primary_key()) {
        model->pk_hist[key.column_name] = row[model->cols[key.column_name]];
        ;
    }

    model->map_old_values[model->pk_hist][column_name] =
        row[model->cols[column_name]];

    if (model->is_part_of_pk(column_name)) {
        model->pk_changes[column_name].first = row[model->cols[column_name]];
    }
}

void MainWindow::cellrenderer_validated_on_edited(
    const Glib::ustring &path, const Glib::ustring &new_text,
    sancho::ui::gtk::SimpleTab *tab, sancho::db::SimpleTabModel *model,
    const std::string &column_name) {
    Gtk::TreeModel::iterator iter = tab->list_store->get_iter(path);

    if (!iter)
        return;

    Gtk::TreeModel::Row row = *iter;

    const Glib::ustring val = row.get_value(model->cols["#"]);

    if (std::atol(val.c_str()) > model->db_rows_cnt) {
        return;
    }

    // TODO: model->pk_hist

    if (model->pk_changes.size() && model->is_part_of_pk(column_name) &&
        new_text != model->pk_changes[column_name].first) {
        Gtk::MessageDialog dialog(
            *this, "Are you sure you want to change the primary key?",
            false /* use_markup */, Gtk::MESSAGE_QUESTION,
            Gtk::BUTTONS_OK_CANCEL);
        std::stringstream q;

        q << "Primary key column '" << column_name
          << "' will permanently change from '"
          << model->pk_changes[column_name].first << "' to '" << new_text
          << "'";

        dialog.set_secondary_text(q.str());

        int result = dialog.run();

        if (result == Gtk::RESPONSE_OK) {
            model->pk_changes[column_name].second = new_text;

            auto result = model->accept_pk_change();

            if (!result->success) {
                sancho::ui::gtk::insert_log_message(tab->log_buffer,
                                                    result->error_message);

                show_warning("Changing primary key failed",
                             result->error_message);

                row[model->cols[column_name]] =
                    model->pk_changes[column_name].first;

                return;
            }
        } else {
            row[model->cols[column_name]] =
                model->pk_changes[column_name].first;
        }

        return;
    }

    if (!model->has_primary_key()) {
        // When user attempts to edit table without pk - bring back the old
        // value
        if (IN_MAP(model->map_old_values, model->pk_hist) &&
            IN_MAP(model->map_old_values[model->pk_hist], column_name)) {
            row[model->cols[column_name]] =
                model->map_old_values[model->pk_hist][column_name];
        } else {
            g_warning("Table does not have a primary key - unable to bring the "
                      "old value back!");
        }
    } else if (IN_MAP(model->map_old_values, model->pk_hist) &&
               IN_MAP(model->map_old_values[model->pk_hist], column_name) &&
               model->map_old_values[model->pk_hist][column_name] == new_text) {
        // Leave row alone - the value of edited cell didn't change
    } else if (!model->is_part_of_pk(column_name)) {
        // Remember new value - later send it to db
        model->map_test[model->pk_hist][column_name] = new_text;

        row[*model->col_color] = model->col_highlighted;
    }

    model->map_old_values.clear();
    model->pk_hist.clear();
}

void MainWindow::on_btn_accept_changes_clicked(
    sancho::ui::gtk::SimpleTab *tab, sancho::db::SimpleTabModel *model) {
    std::shared_ptr<sancho::QueryResult> result = model->accept_changes();

    if (!result->success) {
        sancho::ui::gtk::insert_log_message(tab->log_buffer,
                                            result->error_message);

        show_warning("Committing changes failed", result->error_message);

        return;
    }

    // Reset the background color of highlighted rows

    Gtk::TreeModel::Children children = tab->list_store->children();

    for (Gtk::TreeModel::Children::iterator iter = children.begin();
         iter != children.end(); ++iter) {
        Gtk::TreeModel::Row row = *iter;

        const Glib::ustring val = row.get_value(model->cols["#"]);

        // Handle inserted row

        if (std::atol(val.c_str()) > model->db_rows_cnt) {
            std::shared_ptr<sancho::QueryResult> result =
                model->insert_row(row);

            if (result->success) {
                if (!result->inserted_empty_row) {
                    row[*model->col_color] = model->col_white;
                    model->db_rows_cnt++;
                }
            } else {
                sancho::ui::gtk::insert_log_message(tab->log_buffer,
                                                    result->error_message);

                show_warning("Inserting new row failed", result->error_message);
            }
        } else {
            row[*model->col_color] = model->col_white;
        }
    }
}

Gtk::ScrolledWindow *MainWindow::get_current_swindow() {
    return static_cast<Gtk::ScrolledWindow *>(
        notebook.get_nth_page(notebook.get_current_page()));
}

sancho::ui::gtk::TabType MainWindow::get_tab_type(Gtk::ScrolledWindow *win) {
    if (!IN_MAP(tabs, win)) {
        return sancho::ui::gtk::TabType::Invalid;
    }

    return tabs[win]->type;
}

bool MainWindow::check_mod_binding(GdkEventKey *key_event, GdkKeyCode mod_key,
                                   GdkKeyCode key) {
    return ((key_event->keyval == key) &&
            (key_event->state &
             (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) == mod_key);
}

bool MainWindow::on_key_press_event(GdkEventKey *key_event) {
    if (check_mod_binding(key_event, GDK_CONTROL_MASK, GDK_KEY_n)) {
        on_action_file_new();

        return true;
    } else if (check_mod_binding(key_event, GDK_CONTROL_MASK, GDK_KEY_e)) {
        on_open_sql_editor_clicked();

        return true;
    } else if (check_mod_binding(key_event, GDK_CONTROL_MASK, GDK_KEY_q)) {
        on_action_file_quit();

        return true;
    } else if (check_mod_binding(key_event, GDK_CONTROL_MASK, GDK_KEY_w)) {
        Gtk::ScrolledWindow *window = get_current_swindow();

        if (!window) {
            g_warning("Cannot remove tab!");

            return true;
        }

        on_tab_close_button_clicked(window);
    } else if (key_event->keyval == GDK_KEY_F5) {
        Gtk::ScrolledWindow *window = get_current_swindow();
        const sancho::ui::gtk::TabType type = get_tab_type(window);

        if (type == sancho::ui::gtk::TabType::Query) {
            sancho::ui::gtk::QueryTab &tab = get_query_tab(window);

            on_submit_query_clicked(window, tab.buffer);
        }

        return true;
    } else if (key_event->keyval == GDK_KEY_Return) {
        Gtk::ScrolledWindow *window = get_current_swindow();
        const sancho::ui::gtk::TabType type = get_tab_type(window);

        if (type == sancho::ui::gtk::TabType::List) {
          sancho::ui::gtk::SimpleTab &tab = get_simple_tab(window);

          if (tab.entry_column_mask->is_focus() || tab.entry_filter->is_focus()) {
            on_reload_table_clicked(window);
            return true;
          }
        }
    }

    return Gtk::Window::on_key_press_event(key_event);
}

std::shared_ptr<sancho::db::PostgresConnection> MainWindow::connect(
    const std::shared_ptr<sancho::db::ConnectionDetails> &conn_details) {
    std::shared_ptr<sancho::db::PostgresConnection> pc =
        std::make_shared<sancho::db::PostgresConnection>(conn_details);
    pc->init_connection();

    return pc;
}

void MainWindow::on_primary_key_warning_clicked(
    const Glib::ustring table_name) {
    Gtk::MessageDialog dialog(
        *this, "Table \"" + table_name + "\" doesn't a have primary key",
        false /* use_markup */, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
    dialog.set_modal();
    dialog.set_secondary_text("Updating values in the editor will be disabled");

    dialog.run();
}

void MainWindow::show_warning(const Glib::ustring &primary,
                              const Glib::ustring &secondary) {
    Gtk::MessageDialog dialog(*this, primary, false /* use_markup */,
                              Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
    dialog.set_modal();

    if (!secondary.empty()) {
        dialog.set_secondary_text(secondary);
    }

    dialog.run();
}

void MainWindow::on_menu_file_popup_generic(Gtk::ScrolledWindow *window,
                                            sancho::ui::gtk::SimpleTab *tab,
                                            sancho::db::SimpleTabModel *model) {
    const auto selection = tab->tree->get_selection();

    if (selection) {
        std::size_t size = selection->get_selected_rows().size();

        std::stringstream question;
        question << "Are you sure you want to permanently delete " << size
                 << " row(s)?";

        Gtk::MessageDialog dialog(*this, question.str(), false /* use_markup */,
                                  Gtk::MESSAGE_QUESTION,
                                  Gtk::BUTTONS_OK_CANCEL);
        dialog.set_modal();

        std::stringstream text;

        text << "The following rows will be deleted: ";

        auto rows = selection->get_selected_rows();

        unsigned i = 0;

        for (const auto &row_path : rows) {
            Gtk::TreeModel::iterator row_iter =
                tab->tree->get_model()->get_iter(row_path);

            if (row_iter) {
                Gtk::TreeModel::Row row = *row_iter;

                if (i > 0)
                    text << ", ";

                text << row.get_value(model->cols["#"]);

                i++;
            }
        }

        dialog.set_secondary_text(text.str());

        const int result = dialog.run();

        if (result == Gtk::RESPONSE_OK) {
            auto pks = model->get_primary_key();

            auto rows = selection->get_selected_rows();

            std::vector<std::vector<std::pair<Glib::ustring, Glib::ustring>>>
                rows_to_delete;

            for (const auto &row_path : rows) {
                Gtk::TreeModel::iterator row_iter =
                    tab->tree->get_model()->get_iter(row_path);

                if (row_iter) {
                    Gtk::TreeModel::Row row = *row_iter;

                    std::vector<std::pair<Glib::ustring, Glib::ustring>>
                        pk_values;

                    for (auto pk : pks) {
                        const Glib::ustring pk_value =
                            row.get_value(model->cols[pk.column_name]);

                        pk_values.push_back(
                            std::make_pair<Glib::ustring, Glib::ustring>(
                                Glib::ustring(pk.column_name),
                                Glib::ustring(pk_value)));
                    }

                    rows_to_delete.push_back(pk_values);
                }
            }

            auto result = model->delete_rows(rows_to_delete);

            if (result->success) {
                load_list_results(window);
            } else {
                sancho::ui::gtk::insert_log_message(tab->log_buffer,
                                                    result->error_message);

                show_warning("Deleting rows failed", result->error_message);
            }
        }
    }
}

bool MainWindow::on_list_press(GdkEventButton *button_event,
                               sancho::ui::gtk::SimpleTab *tab,
                               sancho::db::SimpleTabModel *model) {
    if ((button_event->type == GDK_BUTTON_RELEASE) &&
        (button_event->button == 3)) {
        // gtkmm 3.22 only:
        // tab->popup.popup_at_pointer((GdkEvent*) button_event);
        tab->popup.popup(button_event->button, button_event->time);
    }

    return true;
}

void MainWindow::on_browser_refresh_clicked() {
    auto pc = handle_connect();

    if (!pc)
        return;

    refresh_browser(pc);
}

std::shared_ptr<sancho::db::ConnectionDetails> &
MainWindow::find_current_connection() {
    const Glib::ustring connection_name = combo_connections.get_active_text();

    return sancho::db::Connections::instance()->find_connection(
        connection_name);
}

std::shared_ptr<sancho::db::PostgresConnection> MainWindow::handle_connect() {
    auto current_connection = find_current_connection();

    if (current_connection == sancho::db::Connections::instance()->end())
        return nullptr;

    std::shared_ptr<sancho::db::PostgresConnection> pc = nullptr;

    try {
        pc = connect(current_connection);
    } catch (const sancho::db::NoConnection &e) {
        Glib::ustring error_message;

        if (current_connection->password.empty()) {
            error_message = "Password for connection \"" +
                            current_connection->name +
                            "\" is empty. Please edit the connection and "
                            "provide password.";
        } else {
            error_message = e.what();
        }

        show_warning("Connection failed", error_message);

        reset_browser();

        return nullptr;
    }

    return pc;
}

bool MainWindow::on_browser_button_released(GdkEventButton *button_event) {
    if (button_event->button == 3) {
        Gtk::TreeModel::Path path;

        bool found =
            browser.get_path_at_pos(button_event->x, button_event->y, path);

        if (!found)
            return false;

        Gtk::TreeModel::iterator iter = browser_store->get_iter(path);

        if (!iter)
            return false;

        Gtk::TreeModel::Row current_row = *iter;

        if (current_row[browser_model.type] == BrowserItemType::Header) {
            // gtkmm 3.22 only:
            // popup_browser_header.popup_at_pointer((GdkEvent*)
            // button_event);
            popup_browser_header.popup(button_event->button,
                                       button_event->time);

            return true;
        }
    }

    return false;
}

void MainWindow::reset_browser() {
    // Reset current_connection first before triggering events
    sancho::db::Connections::instance()->current_connection = nullptr;

    browser_store->clear();
    combo_connections.set_active(0);
    combo_schemas.remove_all();
    combo_schemas.set_active(0);
}

} // namespace sancho
}
}
