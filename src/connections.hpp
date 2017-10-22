#ifndef CONNECTIONS_HPP
#define CONNECTIONS_HPP

#include <memory>

#include "connection_details.hpp"

class Connections
{
public:
    Connections();

    std::shared_ptr<san::ConnectionDetails>& connection() {
        return conn;
    }

    static Connections* instance() { return &ins; }

private:
    std::shared_ptr<san::ConnectionDetails> conn;
    static Connections ins;
};

#endif
