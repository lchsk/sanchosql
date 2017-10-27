#include "tab.hpp"

namespace san
{
    Tab::Tab()
    {
        hb = Gtk::manage(new Gtk::HBox);
        b = Gtk::manage(new Gtk::Button);
        l = Gtk::manage(new Gtk::Label("SQL Editor"));

        i = Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));

        b->add(*i);
        hb->pack_start(*l, Gtk::PACK_SHRINK);
        hb->pack_start(*b, Gtk::PACK_SHRINK);

        tv = Gtk::manage(new Gtk::TextView);

        toolbar = Gtk::manage(new Gtk::Toolbar);
        btn1 = Gtk::manage(new Gtk::ToolButton);
        btn1->set_icon_name("document-save");

        toolbar->append(*btn1);

        tree = Gtk::manage(new Gtk::TreeView);

        list_store = Gtk::ListStore::create(cr);

        tree->set_model(list_store);

        tree_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

        source_view = Gtk::manage(new Gsv::View);

        buffer = source_view->get_source_buffer();

        if (! buffer) {
            std::cerr << "Gsv::View::get_source_buffer () failed" << std::endl;
        }

        Glib::RefPtr<Gsv::LanguageManager> lm = Gsv::LanguageManager::get_default();
        Glib::RefPtr<Gsv::Language> lang = lm->get_language("sql");

        Glib::RefPtr<Gsv::StyleSchemeManager> sm = Gsv::StyleSchemeManager::get_default();
        Glib::RefPtr<Gsv::StyleScheme> style = sm->get_scheme("cobalt");

        buffer->set_language(lang);
        buffer->set_style_scheme(style);
        buffer->set_text("select * from country") ;

        const std::string s = buffer->get_text();
        std::cout << s << std::endl;

        box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
        box->pack_start(*toolbar, Gtk::PACK_SHRINK);
        box->pack_start(*source_view);
        box->pack_start(*tree);

        tree_scrolled_window->add(*box);
        tree_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        hb->show_all_children();
    }

    EasyTab::EasyTab()
    {
        hb = Gtk::manage(new Gtk::HBox);
        b = Gtk::manage(new Gtk::Button);
        l = Gtk::manage(new Gtk::Label("SQL Editor"));

        i = Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));

        b->add(*i);
        hb->pack_start(*l, Gtk::PACK_SHRINK);
        hb->pack_start(*b, Gtk::PACK_SHRINK);

        tv = Gtk::manage(new Gtk::TextView);

        toolbar = Gtk::manage(new Gtk::Toolbar);
        btn1 = Gtk::manage(new Gtk::ToolButton);
        btn1->set_icon_name("document-save");

        toolbar->append(*btn1);

        tree = Gtk::manage(new Gtk::TreeView);

        cr = std::make_shared<Gtk::TreeModel::ColumnRecord>();
        list_store = Gtk::ListStore::create(*cr);

        tree->set_model(list_store);

        tree_scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

        box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

        btn_reload = Gtk::manage(new Gtk::Button);
        btn_reload->set_label("Reload");
        btn_prev = Gtk::manage(new Gtk::Button);
        btn_prev->set_label("<");
        btn_next = Gtk::manage(new Gtk::Button);
        btn_next->set_label(">");

        label_offset = Gtk::manage(new Gtk::Label);
        label_offset->set_text("From:");

        label_limit = Gtk::manage(new Gtk::Label);
        label_limit->set_text("Show Results:");

        number_offset = Gtk::manage(new san::NumberEntry);
        number_limit = Gtk::manage(new san::NumberEntry);
        browse_box = Gtk::manage(new Gtk::Box);

        browse_box->pack_start(*btn_reload);
        browse_box->pack_start(*btn_prev);
        browse_box->pack_start(*btn_next);

        browse_box->pack_start(*label_offset);
        browse_box->pack_start(*number_offset);

        browse_box->pack_start(*label_limit);
        browse_box->pack_start(*number_limit);

        box->pack_start(*toolbar, Gtk::PACK_SHRINK);
        box->pack_start(*tree);
        box->pack_start(*browse_box, Gtk::PACK_SHRINK);

        tree_scrolled_window->add(*box);
        tree_scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        hb->show_all_children();
    }
}
