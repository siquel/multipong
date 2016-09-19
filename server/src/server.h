#pragma once

#include <jkn/net/socket.h> // UDPSocket
#include <jkn/net/ip_address.h> // IPAddress

namespace pong
{
    const uint32_t MaxClients = 32;

    class Server
    {
    public:
        Server();

        void start();

    private:
        bool processEvents();

        jkn::UDPSocket m_socket;

        // lookup for client index 
        bool m_clientConnected[MaxClients];
        // lookup for client index
        jkn::IPAddress m_clientAddress[MaxClients];
    private:
        Server(const Server&);
        Server& operator=(const Server&);
    };
}