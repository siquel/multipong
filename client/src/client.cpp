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
        m_socket(jkn::IPAddress{ jkn::IPAddressType::IPv4, uint32_t(0), uint16_t(0) }),
        m_clientState(ClientState::Disconnected)
    {
        
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

            uint8_t buffer[256];
            common::WriteStream stream(buffer, sizeof(buffer));

            uint32_t protocol = 0xDEADBEEF;

            stream.serializeBytes((uint8_t*)&protocol, sizeof(uint32_t));

            const uint8_t* send = stream.getData();
            uint32_t bytes = stream.getBytesProcessed();

            m_socket.send(m_serverAddress, send, bytes);

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
        int32_t bytes = m_socket.receive(buffer, sizeof(buffer), from);

        if (bytes > 0)
        {
            char ip[64];
            jkn::addressGetHostIp(from, ip, sizeof(ip));
            printf("Got packet (%d bytes) from %s data = %s\n", bytes, ip, buffer);
        }
    }

}

