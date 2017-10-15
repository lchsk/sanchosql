#include "win_new_connection.hpp"

NewConnectionWindow::NewConnectionWindow
(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
	: Gtk::Window(cobject), builder(builder)
{
	builder->get_widget("box_left", box_left);
	builder->get_widget("paned_new_connections", paned_new_connections);
	builder->get_widget("btn_close", btn_close);

	btn_close->signal_clicked().connect
        (sigc::mem_fun(*this, &NewConnectionWindow::on_btn_close_clicked));

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
