#include "main_window.hpp"


MainWindow::MainWindow()
{
    set_title("Postgres Client");

    set_border_width(0);
    set_default_size(1200, 800);

    add(box);

    box.set_border_width(2);

    browser_scrolled_window.add(browser);
    browser_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    box.pack_start(browser_scrolled_window);

    notebook.set_border_width(10);
    box.pack_start(notebook);

    browser_store = Gtk::TreeStore::create(browser_model);
    browser.set_model(browser_store);

    browser.append_column("Table", browser_model.table);
    browser.append_column("Table Catalog", browser_model.table_catalog);
    browser.append_column("Table Schema", browser_model.table_schema);

    browser.signal_row_activated().connect
        (sigc::mem_fun(*this, &MainWindow::on_browser_row_activated));

    show_all_children();
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

void MainWindow::on_tab_close_button_clicked(const Glib::ustring& btn)
{
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

        b->signal_clicked().connect
            (sigc::bind<Glib::ustring>
             (sigc::mem_fun(
                            *this,
                            &MainWindow::on_tab_close_button_clicked),
              "table-name"));

        Gtk::Image* i = Gtk::manage
            (new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));
        b->add(*i);
        hb->pack_start(*l, Gtk::PACK_SHRINK);
        hb->pack_start(*b, Gtk::PACK_SHRINK);

        Gtk::TextView* tv = Gtk::manage(new Gtk::TextView);

        Gtk::TreeModel::ColumnRecord cr;

        Gtk::TreeModelColumn<Glib::ustring> col;

        cr.add(col);

        Glib::RefPtr<Gtk::ListStore> list_store = Gtk::ListStore::create(cr);

        Gtk::TreeView* tree = Gtk::manage(new Gtk::TreeView);
        tree->set_model(list_store);

        Gtk::TreeModel::Row row = *(list_store->append());

        row[col] = "Cell";
        tree->append_column("Column", col);

        notebook.append_page(*tree, *hb);

        hb->show_all_children();

        show_all_children();
    }
}
