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
        : m_socket()
    {
        memset(m_clientConnected, 0, sizeof(m_clientConnected));
        memset(m_clientAddress, 0, sizeof(m_clientAddress));

        jkn::IPAddress address;
        jkn::addressSetHost(address, 0);
        address.m_port = Port;

        int32_t result = jkn::socket(m_socket, address);

        result = jkn::bind(m_socket, address);
        jkn::setToNonBlocking(m_socket);

        const uint32_t MaxPackets = 256;
        m_receiveQueue.reserve(MaxPackets);
        m_sendQueue.reserve(MaxPackets);
    }

    void Server::start()
    {
        printf("Starting server on port %" PRIu16 "\n", Port);

        const uint32_t MaxPacketMem = 1024;

        uint8_t buffer[MaxPacketMem] = {};
        jkn::IPAddress from = {};

        while (!processEvents())
        {
            int32_t bytes = 0;
            while ((bytes = jkn::receive(m_socket, buffer, sizeof(buffer), from)) != 0)
            {
                common::Memory packet;
                common::PacketType::Enum packetType;
                int32_t result = common::packetProcessIncomingBuffer(0xDEADBEEF, buffer, bytes, packet, packetType);

                if (result != 0)
                {
                    printf("packetProcessIncomingBuffer failed\n");
                    continue;
                }

                PacketEntry entry;
                entry.type = packetType;
                entry.packet = packet;
                entry.from = from;

                m_receiveQueue.push_back(entry);
            }

            uint32_t numIncomingPackets = m_receiveQueue.size();
            for (uint32_t i = 0; i < numIncomingPackets; ++i)
            {
                using namespace common;

                PacketEntry& entry = m_receiveQueue[i];

                switch (entry.type)
                {
                case PacketType::UsernamePacket:
                {
                    printf("Got username packet, now i just need to do something with it..\n");
                }
                break;
                }

                common::packetDestroy(entry.packet);
            }
            // clear all processed packets
            m_receiveQueue.clear();
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
