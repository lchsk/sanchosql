#include <fstream>

#include "tab.hpp"
#include "../../string.hpp"

namespace sancho {
namespace ui {
namespace gtk {
const int LOG_BUFFER_MAX_SIZE = 2000; // characters

void insert_log_message(Glib::RefPtr<Gsv::Buffer> &log_buffer,
                        const Glib::ustring &message) {
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

  int run_yes_no_question(Gtk::Window* window, const std::string& question, const std::string& secondary)
{
      Gtk::MessageDialog dialog(*window, question,
          false /* use_markup */, Gtk::MESSAGE_QUESTION,
          Gtk::BUTTONS_YES_NO);
      dialog.set_secondary_text(secondary);

      return dialog.run();
}

AbstractTab::AbstractTab(const Glib::ustring &tab_name, TabType type)
    : tab_name(tab_name), type(type) {
    hb = Gtk::manage(new Gtk::HBox);
    b = Gtk::manage(new Gtk::Button);
    l = Gtk::manage(new Gtk::Label);

    event_box.add(*l);
    event_box.set_events(Gdk::BUTTON_RELEASE_MASK);
    event_box.set_tooltip_text("Middle button click / Control-W to close");
    // l->set_size_request(110, 20);
    // l->set_ellipsize(Pango::ELLIPSIZE_END);

    b->set_relief(Gtk::ReliefStyle::RELIEF_NONE);

    i = Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));

    b->add(*i);
    hb->pack_start(event_box, Gtk::PACK_SHRINK);
    hb->pack_start(*b, Gtk::PACK_SHRINK);

    set_header_label_text(tab_name);
}

void AbstractTab::set_header_label_text(const Glib::ustring& label)
{
  l->set_text(label);
}

void AbstractTab::show() const {
    tree_scrolled_window->show();
    tree_scrolled_window->show_all_children();
    hb->show_all_children();
}

  QueryTab::QueryTab(const Glib::ustring &tab_name, Gtk::Window* window)
    : AbstractTab(tab_name, TabType::Query), parent_window(window),
      file_status("", false, false)
  {
    tv = Gtk::manage(new Gtk::TextView);

    toolbar = Gtk::manage(new Gtk::Toolbar);

    btn_execute_editor_query = Gtk::manage(new Gtk::ToolButton);
    btn_execute_editor_query->set_icon_name("media-playback-start");
    btn_execute_editor_query->set_tooltip_text("Execute current query");

    btn_execute_all_editor_queries = Gtk::manage(new Gtk::ToolButton);
    btn_execute_all_editor_queries->set_icon_name("media-seek-forward");
    btn_execute_all_editor_queries->set_tooltip_text("Execute all code in editor");

    btn_open_file = Gtk::manage(new Gtk::ToolButton);
    btn_open_file->set_icon_name("document-open");
    btn_open_file->set_tooltip_text("Open new file");

    btn_save_file = Gtk::manage(new Gtk::ToolButton);
    btn_save_file->set_icon_name("document-save");
    btn_save_file->set_tooltip_text("Save file");

    btn_save_file_as = Gtk::manage(new Gtk::ToolButton);
    btn_save_file_as->set_icon_name("document-save-as");
    btn_save_file_as->set_tooltip_text("Save file as...");

    toolbar->append(*btn_execute_editor_query);
    toolbar->append(*btn_execute_all_editor_queries);
    toolbar->append(*btn_open_file);
    toolbar->append(*btn_save_file);
    toolbar->append(*btn_save_file_as);

    btn_open_file->signal_clicked().connect(sigc::bind<QueryTab *>(
        sigc::mem_fun(*this, &QueryTab::on_btn_open_file_clicked),
        this));
    btn_save_file->signal_clicked().connect(sigc::bind<QueryTab *>(
        sigc::mem_fun(*this, &QueryTab::on_btn_save_file_clicked),
        this));
    btn_save_file_as->signal_clicked().connect(sigc::bind<QueryTab *>(
                                                                   sigc::mem_fun(*this, &QueryTab::on_btn_save_file_as_clicked),
        this));

    tree = Gtk::manage(new Gtk::TreeView);

    tree_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    source_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    log_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    data_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

	box_editor = Gtk::manage(new Gtk::Box);
    box_editor->set_halign(Gtk::ALIGN_START);
    box_editor->set_spacing(10);
	label_cursor_position = Gtk::manage(new Gtk::Label);
	label_filename = Gtk::manage(new Gtk::Label);

    box_editor->pack_start(*label_cursor_position);
    box_editor->pack_start(*label_filename);

    // Set up code source view
    source_view = Gtk::manage(new Gsv::View);
    buffer = source_view->get_source_buffer();

    if (!buffer) {
        g_warning("Gsv::View::get_source_buffer() failed for source_view");
        return;
    }

    auto slot = sigc::mem_fun(*this, &QueryTab::on_cursor_position_changed);
    buffer->connect_property_changed("cursor-position", slot);

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
    auto slot_buffer_changed = sigc::mem_fun(*this, &QueryTab::on_buffer_changed);
    buffer->signal_changed().connect(slot_buffer_changed);

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

    // Structure of
    // paned_source:
    //     box_source
    //         source_scrolled_window
    //             source_view
    //         box_editor
    //     paned_results
    //         data_scrolled_window
    //             tree
    //         log_scrolled_window
    //             log

    box_source = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    box_source->pack_start(*source_scrolled_window, true, true);
    box_source->pack_start(*box_editor, false, false);

    buffer->set_style_scheme(style);

    source_scrolled_window->add(*source_view);
    log_scrolled_window->add(*log);
    data_scrolled_window->add(*tree);

    paned_source.pack1(*box_source);
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

    update_file_buttons();
}

void QueryTab::on_cursor_position_changed()
{
    const auto mark = buffer->get_insert();
    const auto iter = buffer->get_iter_at_mark(mark);

    const int line = iter.get_line() + 1;
    const int col = source_view->get_visual_column(iter) + 1;

    const std::string pos = std::to_string(line) + ":" + std::to_string(col);

    label_cursor_position->set_text(pos);
}

  bool QueryTab::was_modified() const {
    return file_status.modified;
  }

  bool SimpleTab::was_modified() const {
    return false;
  }

  void QueryTab::on_buffer_changed()
{
  file_status.modified = true;
  update_file_buttons();
}

  void QueryTab::update_file_buttons()
{
  btn_save_file->set_sensitive(file_status.modified);
  label_filename->set_text(file_status.path);
}

  void QueryTab::on_btn_open_file_clicked(QueryTab* tab)
  {
    if (file_status.modified) {
      const auto confirmation = sancho::ui::gtk::run_yes_no_question(
                                                                   parent_window,
                                                                   "Are you sure?",
                                                                   "Changes to this file will be lost"
                                                                   );
      if (confirmation == Gtk::RESPONSE_NO) {
        return;
      }
    }

    Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*parent_window);

  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  auto filter_sql = Gtk::FileFilter::create();
  filter_sql->set_name("SQL files");
  filter_sql->add_pattern("*.sql");
  dialog.add_filter(filter_sql);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  int result = dialog.run();

  switch (result)
  {
    case Gtk::RESPONSE_OK:
    {
      const std::string path = dialog.get_filename();
      std::string contents;

      try {
        contents = read_file(path);

      } catch (const std::invalid_argument &e) {
        insert_log_message(log_buffer, Glib::ustring::compose("Unable to open '%1'\n", path));
        return;
      }

      buffer->set_text(contents);

      insert_log_message(log_buffer, Glib::ustring::compose("File '%1' was opened\n", path));

      file_status.modified = false;
      file_status.file_loaded = true;
      file_status.path = path;

      update_file_buttons();

      break;
    }
  }
  }

  const std::string QueryTab::read_file(const std::string& path)
  {
    std::ifstream sql_file(path);

    if (!sql_file.is_open()) {
      throw std::invalid_argument("unable to open file");
    }

    std::stringstream contents;
    std::string line;

    while (std::getline(sql_file, line))
      {
        std::istringstream iss(line);

        contents << line << "\n";
      }

    sql_file.close();

      return contents.str();
  }

  void QueryTab::save_file(const std::string& path)
  {
    std::ofstream sql_file(path);

    if (!sql_file.is_open()) {
      throw std::runtime_error("unable to open file");
    }

    sql_file << buffer->get_text();

    sql_file.close();
  }

  void QueryTab::on_btn_save_file_clicked(QueryTab* tab)
  {
    if (!file_status.modified) {
      return;
    }

    if (file_status.file_loaded) {
      try {
        save_file(file_status.path);
      } catch (std::runtime_error &e) {
        insert_log_message(log_buffer, Glib::ustring::compose("File '%1' could not be saved\n", file_status.path));
        return;
      }
    } else {
      on_btn_save_file_as_clicked(this);
    }

    file_status.modified = false;
    update_file_buttons();
  }

  void QueryTab::on_btn_save_file_as_clicked(QueryTab* tab)
  {
    Gtk::FileChooserDialog dialog("Save file as...", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*parent_window);
    dialog.set_do_overwrite_confirmation(true);

    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Save", Gtk::RESPONSE_ACCEPT);

    auto filter_sql = Gtk::FileFilter::create();
    filter_sql->set_name("SQL files");
    filter_sql->add_pattern("*.sql");
    dialog.add_filter(filter_sql);

    auto filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    dialog.add_filter(filter_any);

    int result = dialog.run();

    switch (result)
      {
      case Gtk::RESPONSE_ACCEPT:
        {
      const std::string path = dialog.get_filename();

      try {
        save_file(path);
      } catch (std::runtime_error &e) {
        insert_log_message(log_buffer, Glib::ustring::compose("File '%1' could not be saved\n", path));
        return;
      }

      insert_log_message(log_buffer, Glib::ustring::compose("File '%1' was saved\n", path));

      file_status.modified = false;
      file_status.file_loaded = true;
      file_status.path = path;

      update_file_buttons();

      break;
        }
      }
  }

SimpleTab::SimpleTab(const Glib::ustring &tab_name,
                     std::shared_ptr<sancho::db::SimpleTabModel> &model,
                     sancho::ui::gtk::ListViewType list_view_type)
    : AbstractTab(tab_name, TabType::List),
      tree(Gtk::manage(new Gtk::TreeView)), model(model) {
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

    btn_insert = Gtk::manage(new Gtk::ToolButton);
    btn_insert->set_icon_name("add");
    btn_insert->set_tooltip_text("Insert a new row");

    btn_prev = Gtk::manage(new Gtk::ToolButton);
    btn_prev->set_icon_name("previous");
    btn_prev->set_tooltip_text("Previous page of results");

    btn_next = Gtk::manage(new Gtk::ToolButton);
    btn_next->set_icon_name("next");
    btn_next->set_tooltip_text("Next page of results");

    btn_reset_filtering = Gtk::manage(new Gtk::ToolButton);
    btn_reset_filtering->set_icon_name("edit-clear");
    btn_reset_filtering->set_tooltip_text("Reset column mask and filtering");

    btn_table_info = Gtk::manage(new Gtk::ToolButton);
    btn_table_info->set_icon_name("document-properties");
    btn_table_info->set_tooltip_text("Show details");

    entry_column_mask = Gtk::manage(new Gtk::Entry);
    entry_column_mask->set_tooltip_text("List of column names to show");
    entry_column_mask->set_placeholder_text("List of column names to show");

    entry_filter = Gtk::manage(new Gtk::Entry);
    entry_filter->set_tooltip_text("Apply SQL filtering to the results");
    entry_filter->set_placeholder_text("Apply SQL filtering to the results");

    if (list_view_type == sancho::ui::gtk::ListViewType::Table) {
        toolbar->append(*btn_refresh);
        toolbar->append(*btn_accept);
        toolbar->append(*btn_insert);
        toolbar->append(*btn_prev);
        toolbar->append(*btn_next);
        toolbar->append(*btn_reset_filtering);
        toolbar->append(*btn_table_info);
        toolbar->append(*btn_primary_key_warning);

        set_header_label_text(Glib::ustring::compose("%1 (table)", tab_name));
    } if (list_view_type == sancho::ui::gtk::ListViewType::View) {
        toolbar->append(*btn_refresh);
        toolbar->append(*btn_prev);
        toolbar->append(*btn_next);
        toolbar->append(*btn_reset_filtering);
        toolbar->append(*btn_table_info);

        set_header_label_text(Glib::ustring::compose("%1 (view)", tab_name));
    }

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
    data_scrolled_window->add(*tree);

    box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

    label_offset = Gtk::manage(new Gtk::Label);
    label_offset->set_text("From:");

    label_limit = Gtk::manage(new Gtk::Label);
    label_limit->set_text("Show Results:");

    number_offset = Gtk::manage(new sancho::ui::gtk::NumberEntry);
    number_limit = Gtk::manage(new sancho::ui::gtk::NumberEntry);
    browse_box = Gtk::manage(new Gtk::Box);

    browse_box->pack_start(*label_offset);
    browse_box->pack_start(*number_offset);

    browse_box->pack_start(*label_limit);
    browse_box->pack_start(*number_limit);

    paned_main.pack1(*data_scrolled_window);
    paned_main.pack2(*log_scrolled_window);

    box->pack_start(*toolbar, Gtk::PACK_SHRINK);
    box->pack_start(*entry_column_mask, Gtk::PACK_SHRINK);
    box->pack_start(*entry_filter, Gtk::PACK_SHRINK);
    box->pack_start(paned_main);
    box->pack_start(*browse_box, Gtk::PACK_SHRINK);

    tree_scrolled_window->add(*box);
}
} // namespace sancho
}
}
