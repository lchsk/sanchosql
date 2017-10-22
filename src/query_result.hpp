#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include <unordered_map>

#include <pqxx/pqxx>

#include "conn_util.hpp"

struct QueryResult
{
    QueryResult(pqxx::connection& conn,
                const std::string& query,
                std::unordered_map<pqxx::oid, OidMapping>& oid_names);

    std::vector<Column> columns;
    std::vector<std::vector<std::string> > data;
};

#endif