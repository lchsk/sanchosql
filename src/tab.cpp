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
}
