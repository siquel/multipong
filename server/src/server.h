#pragma once

#include <jkn/net/socket.h> // UDPSocket
#include <jkn/net/ip_address.h> // IPAddress
#include <vector>
#include <common/packets.h>


namespace pong
{
    const uint32_t MaxClients = 32;

    struct PacketEntry
    {
        uint32_t type;
        common::Memory packet;
        jkn::IPAddress from;
    };

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

        uint64_t m_serverSeed;

        std::vector<PacketEntry> m_receiveQueue;
        std::vector<PacketEntry> m_sendQueue;
    private:
        Server(const Server&);
        Server& operator=(const Server&);
    };
}