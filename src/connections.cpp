#include "connections.hpp"

namespace san
{
    Connections::Connections()
    {
        conn = std::make_shared<san::ConnectionDetails>();

        conn->host = "127.0.0.1";
        conn->user = "sancho";
        conn->password = "sancho";
        conn->dbname = "sancho";
        conn->port = "5432";
    }
}
