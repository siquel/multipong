#pragma once

#include <jkn/net/socket.h>

namespace pong
{
    class Server
    {
    public:
        Server();

        void start();

    private:
        bool processEvents();

        jkn::UDPSocket m_socket;

    private:
        Server(const Server&);
        Server& operator=(const Server&);
    };
}