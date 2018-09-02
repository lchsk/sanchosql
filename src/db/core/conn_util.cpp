#include "conn_util.hpp"

namespace sancho {
    Column::Column(const std::string &column_name,
           const std::string &data_type, const std::string &char_length,
           bool is_nullable) :
        m_column_name(column_name), m_data_type(data_type),
          m_char_length(char_length), m_is_nullable(is_nullable) {

    }

    const std::string& Column::get_column_name() const noexcept
    {
        return m_column_name;
    }
    const std::string& Column::get_data_type() const noexcept
    {
        return m_data_type;
    }
    const std::string& Column::get_char_length() const noexcept
    {
        return m_char_length;
    }
    bool Column::is_nullable() const noexcept
    {
        return m_is_nullable;
    }


const std::string
get_data_type(const pqxx::oid oid,
              std::unordered_map<pqxx::oid, sancho::OidMapping> &oid_names) {
    if (oid_names.find(oid) == oid_names.end()) {
        return std::to_string(oid);
    } else {
        return oid_names.at(oid).data_type;
    }
}
} // namespace sancho
