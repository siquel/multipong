#include <stdio.h>
#include <jkn/thread/thread.h>
#include <jkn/net/ip_address.h>
#include <jkn/net/socket.h>
#include <jkn/os.h>
#include <common/network.h>
#include "client.h"

int main(int, char**)
{
    bool result = common::networkInitialize();
    JKN_ASSERT(result, "Network initialization failed");

    if (!result)
    {
        return EXIT_FAILURE;
    }

    jkn::IPAddress serverAddress;
    jkn::addressSetHost(serverAddress, 127, 0, 0, 1);
    serverAddress.m_port = 1337;

    pong::Client client;

    client.connect(serverAddress);

    while (true)
    {
        client.sendPackets();
        client.receivePackets();
        jkn::sleep(1000);
    }
    
    common::networkShutdown();
    return 0;
}
