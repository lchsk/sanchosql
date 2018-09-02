#ifndef ABSTRACT_TAB_MODEL_HPP
#define ABSTRACT_TAB_MODEL_HPP

#include "../../config.hpp"
#include "../../pg_conn.hpp"

// #ifdef MOCK_PG_CONN

// #include "../../tests/mock_pg_conn.hpp"
// #define PG_CLASS MockPostgresConnection
// using ::testing::Exactly;

// #else

#define PG_CLASS sancho::PostgresConnection

// #endif

namespace sancho {
namespace db {
class AbstractTabModel {
  public:
    AbstractTabModel(
					 const std::shared_ptr<sancho::db::ConnectionDetails> &conn_details)
        : conn_details(conn_details),
          connection(std::make_unique<PG_CLASS>(conn_details)) {
#ifdef MOCK_PG_CONN
        EXPECT_CALL(*get_mock_connection(), init_connection())
            .Times(Exactly(1));
#endif

        connection->init_connection();
    }

    virtual ~AbstractTabModel() {}

    sancho::PostgresConnection &conn() const { return *connection; }

    virtual const std::string get_query() const = 0;

    virtual const std::string &get_sort_column() const {
        return EMPTY_SORT_COLUMN;
    };

    const Gdk::RGBA col_white = Gdk::RGBA("rgba(255, 255, 255, 1.0)");
    const Gdk::RGBA col_highlighted = Gdk::RGBA("rgba(100, 20, 210, 1.0)");
    const Gdk::RGBA col_inserted = Gdk::RGBA("rgba(40, 200, 10, 1.0)");
    std::map<std::string, Gtk::TreeModelColumn<Glib::ustring>> cols;
    std::unique_ptr<Gtk::TreeModelColumn<Gdk::RGBA>> col_color;

#ifdef MOCK_PG_CONN
    virtual MockPostgresConnection *get_mock_connection() {
        return static_cast<MockPostgresConnection *>(connection.get());
    }
#endif

  private:
    std::shared_ptr<sancho::db::ConnectionDetails> conn_details;
    std::unique_ptr<sancho::PostgresConnection> connection;

    const std::string EMPTY_SORT_COLUMN = "";
};
} // namespace sancho
}

#endif
