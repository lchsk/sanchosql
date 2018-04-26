#include "tab.hpp"
#include "string.hpp"

namespace sancho {
const int LOG_BUFFER_MAX_SIZE = 2000; // characters

void insert_log_message(Glib::RefPtr<Gsv::Buffer>& log_buffer,
                        const Glib::ustring& message)
{
    // Limit the size of the buffer
    if (log_buffer->get_char_count() > LOG_BUFFER_MAX_SIZE) {
        log_buffer->set_text(
            log_buffer->get_text().substr(0, LOG_BUFFER_MAX_SIZE));
    }

    const Glib::ustring dated_message =
        sancho::date::get_current_datetime() + " " + message;

    // Add the message at the beginning
    log_buffer->insert(log_buffer->begin(), dated_message);
}

AbstractTab::AbstractTab(const Glib::ustring& tab_name, TabType type)
    : tab_name(tab_name), type(type)
{
    hb = Gtk::manage(new Gtk::HBox);
    b = Gtk::manage(new Gtk::Button);
    l = Gtk::manage(new Gtk::Label(tab_name));

    event_box.add(*l);
    event_box.set_events(Gdk::BUTTON_RELEASE_MASK);
    event_box.set_tooltip_text("Middle button click to close");
    // l->set_size_request(110, 20);
    // l->set_ellipsize(Pango::ELLIPSIZE_END);

    b->set_relief(Gtk::ReliefStyle::RELIEF_NONE);

    i = Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));

    b->add(*i);
    hb->pack_start(event_box, Gtk::PACK_SHRINK);
    hb->pack_start(*b, Gtk::PACK_SHRINK);
}

void AbstractTab::show() const
{
    tree_scrolled_window->show();
    tree_scrolled_window->show_all_children();
    hb->show_all_children();
}

QueryTab::QueryTab(const Glib::ustring& tab_name)
    : AbstractTab(tab_name, TabType::Query)
{

    tv = Gtk::manage(new Gtk::TextView);

    toolbar = Gtk::manage(new Gtk::Toolbar);
    btn_execute_editor_query = Gtk::manage(new Gtk::ToolButton);
    btn_execute_editor_query->set_icon_name("media-playback-start");
    btn_execute_editor_query->set_tooltip_text("Execute code in editor");

    toolbar->append(*btn_execute_editor_query);

    tree = Gtk::manage(new Gtk::TreeView);

    tree_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    source_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    log_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    data_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

    // Set up code source view
    source_view = Gtk::manage(new Gsv::View);
    buffer = source_view->get_source_buffer();

    if (!buffer) {
        g_warning("Gsv::View::get_source_buffer() failed for source_view");
    }

    source_view->set_show_line_numbers();
    source_view->set_highlight_current_line();

    Glib::RefPtr<Gsv::LanguageManager> lm = Gsv::LanguageManager::get_default();
    Glib::RefPtr<Gsv::Language> lang = lm->get_language("sql");

    Glib::RefPtr<Gsv::StyleSchemeManager> sm =
        Gsv::StyleSchemeManager::get_default();
    Glib::RefPtr<Gsv::StyleScheme> style = sm->get_scheme("cobalt");

    buffer->set_language(lang);
    buffer->set_style_scheme(style);

    const Glib::ustring default_text = Glib::ustring::compose(
        "-- Opened %1 by %2\n-- Code executed in this editor is "
        "automatically committed\n\n",
        sancho::date::get_current_datetime(), sancho::user::get_user_name());

    buffer->set_text(default_text);
    source_view->set_monospace();

    // Set up log source view
    log = Gtk::manage(new Gsv::View);
    log_buffer = log->get_source_buffer();
    log->set_monospace();

    if (!log_buffer) {
        g_warning("Gsv::View::get_source_buffer() failed for log");
    }

    insert_log_message(log_buffer,
                       Glib::ustring::compose("Editor opened by %1",
                                              sancho::user::get_user_name()));

    log->property_editable() = false;
    log_buffer->set_style_scheme(style);

    source_scrolled_window->add(*source_view);
    source_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                       Gtk::POLICY_AUTOMATIC);

    log_scrolled_window->add(*log);
    log_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                    Gtk::POLICY_AUTOMATIC);

    data_scrolled_window->add(*tree);
    data_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                     Gtk::POLICY_AUTOMATIC);

    paned_source.pack1(*source_scrolled_window);
    paned_source.pack2(paned_results);

    source_scrolled_window->set_valign(Gtk::Align::ALIGN_FILL);
    paned_results.set_valign(Gtk::Align::ALIGN_BASELINE);

    data_scrolled_window->set_valign(Gtk::Align::ALIGN_FILL);
    log_scrolled_window->set_valign(Gtk::Align::ALIGN_BASELINE);

    paned_results.pack1(*data_scrolled_window);
    paned_results.pack2(*log_scrolled_window);

    box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box->pack_start(*toolbar, Gtk::PACK_SHRINK);

    box->pack_start(paned_source);

    tree_scrolled_window->add(*box);
    tree_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                     Gtk::POLICY_AUTOMATIC);
}

SimpleTab::SimpleTab(const Glib::ustring& tab_name,
                     std::shared_ptr<sancho::SimpleTabModel>& model)
    : AbstractTab(tab_name, TabType::List),
      tree(Gtk::manage(new Gtk::TreeView)), model(model)
{
    tv = Gtk::manage(new Gtk::TextView);

    toolbar = Gtk::manage(new Gtk::Toolbar);

    btn_accept = Gtk::manage(new Gtk::ToolButton);
    btn_accept->set_icon_name("go-down");
    btn_accept->set_tooltip_text("Commit");

    btn_refresh = Gtk::manage(new Gtk::ToolButton);
    btn_refresh->set_icon_name("view-refresh");
    btn_refresh->set_tooltip_text("Refresh");

    btn_primary_key_warning = Gtk::manage(new Gtk::ToolButton);
    btn_primary_key_warning->set_icon_name("dialog-warning");
    btn_primary_key_warning->set_tooltip_text(
        "The table doesn't have primary key");

    toolbar->append(*btn_refresh);
    toolbar->append(*btn_accept);
    toolbar->append(*btn_primary_key_warning);

    tree_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    log_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    data_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

    // Set up log source view

    Glib::RefPtr<Gsv::StyleSchemeManager> sm =
        Gsv::StyleSchemeManager::get_default();
    Glib::RefPtr<Gsv::StyleScheme> style = sm->get_scheme("cobalt");

    log = Gtk::manage(new Gsv::View);
    log_buffer = log->get_source_buffer();
    log->set_monospace();

    if (!log_buffer) {
        g_warning("Gsv::View::get_source_buffer() failed for listview log");
    }

    log->property_editable() = false;
    log_buffer->set_style_scheme(style);

    log_scrolled_window->add(*log);
    log_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                    Gtk::POLICY_AUTOMATIC);

    data_scrolled_window->add(*tree);
    data_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                     Gtk::POLICY_AUTOMATIC);

    box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

    btn_insert = Gtk::manage(new Gtk::Button);
    btn_insert->set_label("Insert");
    btn_prev = Gtk::manage(new Gtk::Button);
    btn_prev->set_label("<");
    btn_next = Gtk::manage(new Gtk::Button);
    btn_next->set_label(">");

    label_offset = Gtk::manage(new Gtk::Label);
    label_offset->set_text("From:");

    label_limit = Gtk::manage(new Gtk::Label);
    label_limit->set_text("Show Results:");

    number_offset = Gtk::manage(new sancho::NumberEntry);
    number_limit = Gtk::manage(new sancho::NumberEntry);
    browse_box = Gtk::manage(new Gtk::Box);

    browse_box->pack_start(*btn_insert);
    browse_box->pack_start(*btn_prev);
    browse_box->pack_start(*btn_next);

    browse_box->pack_start(*label_offset);
    browse_box->pack_start(*number_offset);

    browse_box->pack_start(*label_limit);
    browse_box->pack_start(*number_limit);

    paned_main.pack1(*data_scrolled_window);
    paned_main.pack2(*log_scrolled_window);

    box->pack_start(*toolbar, Gtk::PACK_SHRINK);
    box->pack_start(paned_main);
    box->pack_start(*browse_box, Gtk::PACK_SHRINK);

    tree_scrolled_window->add(*box);
    tree_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                     Gtk::POLICY_AUTOMATIC);
}
} // namespace sancho
