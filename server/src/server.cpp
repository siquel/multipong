#include "server.h"
#include <jkn/net/ip_address.h>
#include <stdio.h> // printf
#include <inttypes.h> // PRIxYY
#include "event_queue.h"

#include <common/packets.h>
#include <common/serialization.h> // stream

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

        uint8_t buffer[256];
        jkn::IPAddress from = {};

        while (!processEvents())
        {
            int32_t bytes = m_socket.receive(buffer, sizeof(buffer), from);
            int32_t packetType;
            char ip[64];

            if (bytes == 0) continue;
            
            jkn::addressGetHostIp(from, ip, sizeof(ip));
            //printf("Got packet (size of %d bytes) from %s containing \"%s\"\n", bytes, ip, buffer);

            common::ReadStream stream(buffer, bytes);
            uint32_t protocol = 0;
            
            if (!stream.serializeBits(protocol, 32)) continue;

            
            // if the packet type doesn't match we just drop it
            if (!stream.serializeInteger(packetType, 0, common::PacketType::Count)) continue;

            common::Memory packetMem;

            if (!common::packetCreate(common::PacketType::Enum(packetType), packetMem))
            {
                printf("Allocation failed\n");
                continue;
            }

            if (!common::serialize(stream, common::PacketType::Enum(packetType), packetMem))
            {
                printf("Serialization failed\n");
                continue;
            }

            printf("protocol = %x, packetType = %d, contents = %s\n", protocol, packetType, (char*)packetMem.ptr);

            const char response[] = "Haista paska";
            m_socket.send(from, response, sizeof(response));

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