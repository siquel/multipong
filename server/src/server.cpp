#include "server.h"
#include <jkn/net/ip_address.h>
#include <stdio.h> // printf
#include <inttypes.h> // PRIxYY
#include "event_queue.h"
namespace pong
{
    extern bool nextEvent(Event&);

    const uint16_t Port = uint16_t(1337);

    Server::Server()
        : m_socket(jkn::IPAddress { jkn::IPAddressType::IPv4, uint32_t(0) /* ANY */, Port })
    {
        memset(m_clientConnected, 0, sizeof(m_clientConnected));
        memset(m_clientAddress, 0, sizeof(m_clientAddress));
    }

    void Server::start()
    {
        printf("Starting server on port %" PRIu16 "\n", Port);

        char buffer[256];
        jkn::IPAddress from = {};

        while (!processEvents())
        {
            int32_t bytes = m_socket.receive(buffer, sizeof(buffer), from);

            if (bytes > 0)
            {
                char ip[64];
                jkn::addressGetHostIp(from, ip, sizeof(ip));
                printf("Got packet (size of %d bytes) from %s containing \"%s\"\n", bytes, ip, buffer);

                const char response[] = "Haista paska";
                m_socket.send(from, response, sizeof(response));
            }
        }
    }

    bool Server::processEvents()
    {
        bool exit = false;

        Event ev;
        while (nextEvent(ev))
        {
            switch (ev.type)
            {
            case EventType::Exit:
                exit = true;
                break;

            default: 
                break;
            }
        }

        return exit;
    }
}