#include "client.h"
#include <jkn/net/ip_address.h>
#include <stdio.h>
#include <string.h> // memcpy
#include <inttypes.h>
#include <common/packets.h>
#include <common/serialization.h>

namespace pong
{
    Client::Client() :
        m_socket(),
        m_clientState(ClientState::Disconnected)
    {
        jkn::IPAddress address;
        jkn::addressSetHost(address, 0);
        address.m_port = 0;
        
        int32_t result = jkn::socket(m_socket, address);
        JKN_ASSERT(result == 0, "socket() failed");
        result = jkn::bind(m_socket, address);
        JKN_ASSERT(result == 0, "bind() failed");
        result = jkn::setToNonBlocking(m_socket);
        JKN_ASSERT(result == 0, "setToNonBlockin() failed");
    }

    Client::~Client()
    {

    }

    int32_t Client::sendPacket(common::PacketType::Enum _packetType, const common::Memory& _packet)
    {
        uint8_t buffer[256] = {};
        uint32_t protocol = 0xDEADBEEF;
        uint32_t packetSize = 0;

        common::packetProcessOutgoing(protocol, _packetType, _packet, buffer, sizeof(buffer), packetSize);

        if (!jkn::sendto(m_socket, m_serverAddress, buffer, packetSize))
        {
            printf("Send failed\n");
            return -1;
        }

        return 0;
    }

    void Client::update()
    {
        common::packetBegin();

        switch (m_clientState)
        {
        case ClientState::SendingUsername:
        {
            common::Memory mem;
            if (!common::packetCreate(common::PacketType::UsernamePacket, mem))
            {
                printf("Allocation failed\n");
                break;
            }
            common::UsernamePacket* packet = (common::UsernamePacket*) mem.ptr;
            char uname[] = "ThisIsMyUsername";
            strncpy(packet->m_username, uname, sizeof(uname));
            
            if (sendPacket(common::PacketType::UsernamePacket, mem) < 0)
            {
                printf("Packet send failure\n");
            }

            common::packetDestroy(mem);
        }
        break;
        default:

            break;
        }

        jkn::IPAddress from = {};

        uint8_t buffer[256];
        int32_t bytes = jkn::receive(m_socket, buffer, sizeof(buffer), from);

        if (bytes > 0)
        {
            common::Memory packetMem;
            common::PacketType::Enum packetType;

            if (common::packetProcessIncomingBuffer(0xDEADBEEF, buffer, bytes, packetMem, packetType) != 0)
            {
                JKN_ASSERT(0, "packetProcessIncomingBuffer failed");
            }

            switch (packetType)
            {
            case common::PacketType::RandomNumberPacket:
            {
                common::RandomNumberPacket& packet = *(common::RandomNumberPacket*)packetMem.ptr;
                printf("Got random number %" PRIx64 "\n", packet.m_randomNumber);
            }
            break;
            default:
                printf("Unknown packet %d\n", (int32_t)packetType);
                break;
            }

            common::packetDestroy(packetMem);
        }

        common::packetEnd();
    }

    void Client::connect(const jkn::IPAddress& _serverAddress)
    {
        m_clientState = ClientState::SendingUsername;
        m_serverAddress = _serverAddress;

        char ip[64];
        jkn::addressGetHostIp(_serverAddress, ip, sizeof(ip));

        printf("Connecting to server %s port %" PRIu16 "\n", ip, _serverAddress.m_port);
    }
}

