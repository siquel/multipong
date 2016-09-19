#include "client.h"
#include <jkn/net/ip_address.h>
#include <stdio.h>
#include <inttypes.h>

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
            const char buffer[] = "ThisIsMyUsername";
            m_socket.send(m_serverAddress, buffer, sizeof(buffer));
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

