#include "server.h"
#include <jkn/net/ip_address.h>
#include <stdio.h> // printf
#include <inttypes.h> // PRIxYY
#include "event_queue.h"

#include <common/packets.h>
#include <common/serialization.h> // stream
#include <common/hash.h> // murmur_hash_64
#include <time.h> // time

namespace pong
{
    using namespace common;

    extern bool nextEvent(Event&);

    const uint16_t Port = uint16_t(1337);

    Server::Server() : 
        m_socket(),
        m_serverSeed(0)
    {
        srand(uint32_t(time(NULL)));
        m_serverSeed = common::genSalt();

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

    void Server::processUsernamePacket(common::UsernamePacket* _packet, const jkn::IPAddress& _from)
    {
        printf("Got username packet, now i just need to do something with it..\n");
        printf("Data = %s\n", _packet->m_username);
        uint32_t len = (uint32_t)strlen(_packet->m_username);
        uint64_t hash =
            murmur_hash_64(&m_serverSeed,
                8u,
                murmur_hash_64(_packet->m_username, len, uint32_t(time(NULL))));
        
        common::Memory packetMemory;

        if (!common::packetCreate(PacketType::RandomNumberPacket, packetMemory))
        {
            JKN_ASSERT(0, "Allocation failed");
        }

        common::RandomNumberPacket& packet = *(RandomNumberPacket*)packetMemory.ptr;
        packet.m_randomNumber = hash;

        PacketEntry entry;
        entry.from = _from;
        entry.packet = packetMemory;
        entry.type = PacketType::RandomNumberPacket;

        m_sendQueue.push_back(entry);
    }

    void Server::start()
    {
        printf("Starting server on port %" PRIu16 "\n", Port);

        const uint32_t MaxPacketMem = 1024;

        uint8_t buffer[MaxPacketMem] = {};
        jkn::IPAddress from = {};

        while (!processEvents())
        {
            common::packetBegin();

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

            size_t numIncomingPackets = m_receiveQueue.size();
            for (size_t i = 0; i < numIncomingPackets; ++i)
            {
                using namespace common;

                PacketEntry& entry = m_receiveQueue[i];

                switch (entry.type)
                {
                case PacketType::UsernamePacket:
                {
                    processUsernamePacket((UsernamePacket*)entry.packet.ptr, entry.from);
                }
                break;
                }

                common::packetDestroy(entry.packet);
            }

            size_t numOutgoingPackets = m_sendQueue.size();
            for (size_t i = 0; i < numOutgoingPackets; ++i)
            {
                using namespace common;

                PacketEntry& entry = m_sendQueue[i];

                // TODO allocator
                uint32_t dataSize;

                if (packetProcessOutgoing(0xDEADBEEF, PacketType::Enum(entry.type), entry.packet, buffer, sizeof(buffer), dataSize) != 0)
                {
                    JKN_ASSERT(0, "PacketProcess failed");
                }

                if (!jkn::sendto(m_socket, entry.from, buffer, dataSize))
                {
                    JKN_ASSERT(0, "sendto failed");
                }

                common::packetDestroy(entry.packet);
            }

            // clear all processed packets
            m_receiveQueue.clear();
            m_sendQueue.clear();

            common::packetEnd();
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
