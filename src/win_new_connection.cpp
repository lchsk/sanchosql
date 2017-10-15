#include "win_new_connection.hpp"
#include "pg_conn.hpp"

#include <iostream>

NewConnectionWindow::NewConnectionWindow
(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
	: Gtk::Window(cobject), builder(builder)
{
	builder->get_widget("box_left", box_left);
	builder->get_widget("paned_new_connections", paned_new_connections);
	builder->get_widget("btn_close", btn_close);
	builder->get_widget("btn_test_connection", btn_test_connection);

	builder->get_widget("label_connection_status", label_connection_status);

	builder->get_widget("text_host", text_host);
	builder->get_widget("text_port", text_port);
	builder->get_widget("text_db", text_db);
	builder->get_widget("text_username", text_user);
	builder->get_widget("text_password", text_password);

	btn_close->signal_clicked().connect
        (sigc::mem_fun(*this, &NewConnectionWindow::on_btn_close_clicked));

	btn_test_connection->signal_clicked().connect
        (sigc::mem_fun
		 (*this, &NewConnectionWindow::on_btn_test_connection_clicked));

	signal_show().connect(sigc::mem_fun(*this, &NewConnectionWindow::on_win_show));

	tree_connections.set_headers_visible(false);

	scrolled_window.add(tree_connections);
	scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	box_left->pack_start(scrolled_window);

	connections_model = Gtk::TreeStore::create(connection_columns);
	tree_connections.set_model(connections_model);

	Gtk::TreeModel::Row row = *(connections_model->append());
	row[connection_columns.col_name] = "Postgres connections";

	Gtk::TreeModel::Row childrow = *(connections_model->append(row.children()));
	childrow[connection_columns.col_name] = "Connection 1";

	tree_connections.append_column("Name", connection_columns.col_name);

	show_all_children();

	int w, h;
    get_size(w, h);
    paned_new_connections->set_position(0.35 * w);
}

void NewConnectionWindow::on_btn_close_clicked()
{
	hide();
}

void NewConnectionWindow::on_btn_test_connection_clicked()
{
	std::shared_ptr<ConnectionDetails> conn = std::make_shared<ConnectionDetails>();

	conn->set_host(text_host->get_text());
	conn->user = text_user->get_text();
	conn->password = text_password->get_text();
	conn->dbname = text_db->get_text();
	conn->port = text_port->get_text();;

	PostgresConnection pg_conn(conn);
	pg_conn.init_connection();

	if (pg_conn.is_open()) {
		label_connection_status->set_text("Success");
		label_connection_status->override_color
			(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
	} else {
		label_connection_status->set_text("Failed: " + pg_conn.error_message());
		label_connection_status->override_color
			(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
	}
}

void NewConnectionWindow::on_win_show()
{
	label_connection_status->set_text("");
}
