#pragma once

#include <jkn/net/socket.h>
#include <jkn/net/ip_address.h>
#include <common/packets.h>

namespace pong
{
    struct ClientState
    {
        enum Enum
        {
            Disconnected, // Not doing anything
            SendingUsername, // The client connects to the server, sending in the username
            Count
        };
    };

    class Client
    {
    public:
        Client();
        ~Client();

        void connect(const jkn::IPAddress& _serverAddress);

        void update();
    private:
        // 0 on success, < 0 failure
        int32_t sendPacket(common::PacketType::Enum _type, const common::Memory& _packet);

        Client(const Client&);
        Client& operator=(const Client&);

    private:
        jkn::UDPSocket m_socket;
        // The current client state
        ClientState::Enum m_clientState;

        // Address of server connected to
        jkn::IPAddress m_serverAddress;
    };
}

