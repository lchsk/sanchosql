#include <iostream>

#include "win_new_connection.hpp"

#include "string.hpp"

namespace san {
NewConnectionWindow::NewConnectionWindow(
    BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
    : Gtk::Window(cobject), builder(builder)
{
    builder->get_widget("box_left", box_left);
    builder->get_widget("paned_new_connections", paned_new_connections);
    builder->get_widget("btn_save", btn_save);
    builder->get_widget("btn_close", btn_close);
    builder->get_widget("btn_test_connection", btn_test_connection);
    builder->get_widget("btn_add_connection", btn_add_connection);
    builder->get_widget("btn_del_connection", btn_del_connection);

    builder->get_widget("checkbox_save_password", checkbox_save_password);

    btn_save->set_sensitive(false);
    btn_del_connection->set_sensitive(false);

    builder->get_widget("label_connection_status", label_connection_status);

    builder->get_widget("text_connection_name", text_connection_name);
    builder->get_widget("text_host", text_host);
    builder->get_widget("text_port", text_port);
    builder->get_widget("text_db", text_db);
    builder->get_widget("text_username", text_user);
    builder->get_widget("text_password", text_password);

    // After widgets were loaded
    set_adding_mode();
    NewConnectionWindow::reset_connection_status();

    btn_save->signal_clicked().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::on_btn_save_clicked));

    btn_close->signal_clicked().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::on_btn_close_clicked));

    btn_test_connection->signal_clicked().connect(sigc::mem_fun(
        *this, &NewConnectionWindow::on_btn_test_connection_clicked));

    btn_add_connection->signal_clicked().connect(sigc::mem_fun(
        *this, &NewConnectionWindow::on_btn_add_connection_clicked));

    btn_del_connection->signal_clicked().connect(sigc::mem_fun(
        *this, &NewConnectionWindow::on_btn_del_connection_clicked));

    checkbox_save_password->signal_toggled().connect(sigc::mem_fun(
        *this, &NewConnectionWindow::on_checkbox_save_password_toggled));

    signal_show().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::on_win_show));
    signal_hide().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::on_win_hide));

    text_connection_name->signal_changed().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::update_save_btn));

    text_host->signal_changed().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::update_save_btn));

    text_port->signal_changed().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::update_save_btn));

    text_db->signal_changed().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::update_save_btn));

    text_user->signal_changed().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::update_save_btn));

    text_password->signal_changed().connect(
        sigc::mem_fun(*this, &NewConnectionWindow::update_save_btn));

    tree_connections.signal_cursor_changed().connect(sigc::mem_fun(
        *this, &NewConnectionWindow::on_selected_connection_changed));

    scrolled_window.add(tree_connections);
    scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    box_left->pack_start(scrolled_window);

    connections_model = Gtk::TreeStore::create(connection_columns);
    tree_connections.set_model(connections_model);

    tree_connections.append_column("Postgres connections",
                                   connection_columns.col_name);

    show_all_children();

    int w, h;
    get_size(w, h);
    paned_new_connections->set_position(0.3 * w);
}

void NewConnectionWindow::on_btn_save_clicked()
{
    if (mode == Mode::Adding) {
        san::Connections::instance()->add(
            text_connection_name->get_text(), text_host->get_text(),
            text_user->get_text(), text_password->get_text(),
            text_db->get_text(), text_port->get_text(),
            checkbox_save_password->get_active());
    } else if (mode == Mode::Editing) {
        san::Connections::instance()->update_conn(
            edited_conn_name, text_connection_name->get_text(),
            text_host->get_text(), text_user->get_text(),
            text_password->get_text(), text_db->get_text(),
            text_port->get_text(), checkbox_save_password->get_active());
    }

    on_win_show();
    set_adding_mode();
}

void NewConnectionWindow::on_btn_close_clicked() { hide(); }

void NewConnectionWindow::on_btn_test_connection_clicked()
{
    std::shared_ptr<san::ConnectionDetails> conn =
        std::make_shared<san::ConnectionDetails>();

    conn->set_host(text_host->get_text());
    conn->user = text_user->get_text();

    // Always use actual password value regardless of the checkbox
    conn->password = text_password->get_text();
    conn->dbname = text_db->get_text();
    conn->port = text_port->get_text();
    ;

    san::PostgresConnection pg_conn(conn);

    try {
        pg_conn.init_connection();
        g_debug("Test connection OK: %s", conn->postgres_string_safe().c_str());
    } catch (const san::NoConnection& e) {
        g_debug("Test connection failed: %s",
                conn->postgres_string_safe().c_str());
    }

    update_connection_status(pg_conn);
}

void NewConnectionWindow::reset_connection_status()
{
    label_connection_status->set_text("");
}

void NewConnectionWindow::update_connection_status(
    const san::PostgresConnection& pg_conn)
{
    if (pg_conn.is_open()) {
        label_connection_status->set_text("Success");
        label_connection_status->override_color(Gdk::RGBA("green"),
                                                Gtk::STATE_FLAG_NORMAL);
    } else {
        label_connection_status->set_text("Failed: " + pg_conn.error_message());
        label_connection_status->override_color(Gdk::RGBA("red"),
                                                Gtk::STATE_FLAG_NORMAL);
    }
}

void NewConnectionWindow::on_btn_add_connection_clicked() { set_adding_mode(); }

void NewConnectionWindow::on_btn_del_connection_clicked()
{
    Glib::RefPtr<Gtk::TreeSelection> selection =
        tree_connections.get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();

    if (!iter)
        return;

    Gtk::TreeModel::Row row = *iter;

    const std::string conn_name = row.get_value(connection_columns.col_name);
    const std::string question =
        "Do you want to delete connection '" + conn_name + "'?";

    Gtk::MessageDialog dialog(*this, question, false /* use_markup */,
                              Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
    dialog.set_secondary_text("This connection will be permanently lost.");

    const int result = dialog.run();

    if (result == Gtk::RESPONSE_OK) {
        san::Connections::instance()->remove(conn_name);

        on_win_show();
    }
}

void NewConnectionWindow::on_win_show()
{
    connections_model->clear();

    for (const auto& conn : san::Connections::instance()->get_connections()) {
        Gtk::TreeModel::Row row = *(connections_model->append());
        row[connection_columns.col_name] = conn.first;
    }

    prepare_adding();
}

void NewConnectionWindow::on_win_hide()
{
    san::Connections::instance()->save_connections();
}

void NewConnectionWindow::on_checkbox_save_password_toggled()
{
    update_save_btn();
}

void NewConnectionWindow::on_selected_connection_changed()
{
    Glib::RefPtr<Gtk::TreeSelection> selection =
        tree_connections.get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();

    if (!iter) {
        btn_del_connection->set_sensitive(false);

        return;
    };

    set_editing_mode();
    reset_connection_status();

    Gtk::TreeModel::Row row = *iter;

    const std::string conn_name = row.get_value(connection_columns.col_name);
    edited_conn_name = conn_name;

    auto& connection_details = san::Connections::instance()->get(conn_name);

    text_connection_name->set_text(connection_details->name);
    text_host->set_text(connection_details->host);
    text_user->set_text(connection_details->user);
    text_password->set_text(connection_details->password);
    text_db->set_text(connection_details->dbname);
    text_port->set_text(connection_details->port);

    checkbox_save_password->set_active(connection_details->save_password);

    btn_del_connection->set_sensitive(true);

    update_save_btn();
}

void NewConnectionWindow::set_adding_mode()
{
    mode = Mode::Adding;
    edited_conn_name = "";
    Glib::RefPtr<Gtk::TreeSelection> selection =
        tree_connections.get_selection();
    selection->unselect_all();
    btn_del_connection->set_sensitive(false);

    prepare_adding();
}

void NewConnectionWindow::set_editing_mode() { mode = Mode::Editing; }

void NewConnectionWindow::update_save_btn()
{
    if (mode == Mode::Adding)
        btn_save->set_sensitive(can_add_new_connection());
    else if (mode == Mode::Editing)
        btn_save->set_sensitive(can_save_edited_connection());
}

bool NewConnectionWindow::can_save_edited_connection() const
{
    return san::Connections::instance()->can_update_conn_details(
        edited_conn_name, text_connection_name->get_text(),
        text_host->get_text(), text_user->get_text(), text_password->get_text(),
        // get_password(),
        text_db->get_text(), text_port->get_text(),
        checkbox_save_password->get_active());
}

bool NewConnectionWindow::can_add_new_connection() const
{
    if (san::Connections::instance()->any_fields_empty(
            text_host->get_text(), text_port->get_text(), text_db->get_text(),
            text_user->get_text(), text_connection_name->get_text()))
        return false;

    if (san::Connections::instance()->exists(text_connection_name->get_text()))
        return false;

    return true;
}

void NewConnectionWindow::reset_widgets()
{
    text_connection_name->set_text(
        "Connection " +
        std::to_string(san::Connections::instance()->size() + 1));
    text_host->set_text("127.0.0.1");
    text_port->set_text("5432");
    text_db->set_text("");
    text_user->set_text("");
    text_password->set_text("");

    reset_connection_status();
}

void NewConnectionWindow::prepare_adding() { reset_widgets(); }
} // namespace san
