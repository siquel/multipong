#pragma once

#include <jkn/net/socket.h>
#include <jkn/net/ip_address.h>

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

        // Quick hax for now
        void sendPackets();
        void receivePackets();
    private:
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

