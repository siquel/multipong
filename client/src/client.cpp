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
        jkn::IPAddress address{ jkn::IPAddressType::IPv4, uint32_t(0), uint16_t(0) };
        
        int32_t result = jkn::socket(m_socket, address);
        result = jkn::bind(m_socket, address);
        result = jkn::setToNonBlocking(m_socket);
    }

    Client::~Client()
    {

    }

    void Client::connect(const jkn::IPAddress& _serverAddress)
    {
        m_clientState = ClientState::SendingUsername;
        m_serverAddress = _serverAddress;

        char ip[64];
        jkn::addressGetHostIp(_serverAddress, ip, sizeof(ip));

        printf("Connecting to server %s port %" PRIu16 "\n", ip, _serverAddress.m_port);
    }

    void Client::sendPackets()
    {
        switch (m_clientState)
        {
        case ClientState::SendingUsername:
        {
            /*common::UsernamePacket packet;
            char username[] = "ThisIsMyUsername";
            memcpy(packet.m_username, username, sizeof(username));
            m_socket.send(m_serverAddress, packet.m_username, sizeof(username));*/



            uint8_t buffer[256] = {};
            common::WriteStream stream(buffer, sizeof(buffer));

            uint32_t protocol = 0xDEADBEEF;

            if (!stream.serializeBits(protocol, 32)) 
            {
                break;
            }
            int32_t type = (int32_t)common::PacketType::UsernamePacket;
            
            if (!stream.serializeInteger(type, 0, common::PacketType::Count - 1))
            {
                printf("Serialization failed for %d\n", type);
                break;
            }

            common::Memory mem;

            if (!common::packetCreate(common::PacketType::UsernamePacket, mem))
            {
                printf("Allocation failed\n");
                break;
            }

            common::UsernamePacket* packet = (common::UsernamePacket*)mem.ptr;
            strncpy(packet->m_username, "ThisIsMyUsername", 32);

            if (!common::serialize(stream, common::PacketType::UsernamePacket, mem))
            {
                printf("Serialization failed\n");
                break;
            }

            // flush remaining bits
            stream.flush();

            const uint8_t* send = stream.getData();
            uint32_t bytes = stream.getBytesProcessed();

            if (!jkn::sendto(m_socket, m_serverAddress, send, bytes))
            {
                printf("Send failed\n");
            }

            common::packetDestroy(mem);
        }
        break;
        default:
            break;
        }
    }

    void Client::receivePackets()
    {
        jkn::IPAddress from = {};

        char buffer[256];
        int32_t bytes = jkn::receive(m_socket, buffer, sizeof(buffer), from);

        if (bytes > 0)
        {
            char ip[64];
            jkn::addressGetHostIp(from, ip, sizeof(ip));
            printf("Got packet (%d bytes) from %s data = %s\n", bytes, ip, buffer);
        }
    }

}

