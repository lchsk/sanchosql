#ifndef QUERY_TAB_MODEL_HPP
#define QUERY_TAB_MODEL_HPP

#include "abstract_tab_model.hpp"

namespace sancho {
namespace db {
class QueryTabModel : public AbstractTabModel {
  public:
    QueryTabModel(
        const std::shared_ptr<sancho::db::ConnectionDetails>& conn_details)
        : AbstractTabModel(conn_details) {}

    std::string query;

    const std::string get_query() const { return query; }
};

} // namespace sancho
}

#endif
