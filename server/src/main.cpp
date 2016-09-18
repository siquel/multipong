#include <stdio.h>
#include <jkn/net/socket.h>
#include <jkn/thread/thread.h>
#include <jkn/net/ip_address.h>
#include <jkn/jkn.h>

void initialize()
{
#if JKN_PLATFORM_WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "Failed to initialize winsock, error %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
#endif
}

void deinitialize()
{
#if JKN_PLATFORM_WINDOWS
    WSACleanup();
#endif
}

int main(int, char**)
{
    initialize();
    jkn::IPAddress addr = {};
    addr.m_port = 1337;
    
    jkn::addressSetHost(addr, INADDR_ANY);

    char ip[128] = {};
    jkn::addressGetHostIp(addr, ip, sizeof(ip));

    jkn::UDPSocket socket(addr);

    printf("Starting server on ip %s:%d\n", ip, addr.m_port);

    while (true)
    {
        jkn::IPAddress from = {};
        char buffer[256];

        int32_t bytes = socket.receive(buffer, sizeof(buffer), from);

        if (bytes > 0)
        {
            char ip[64];
            jkn::addressGetHostIp(from, ip, sizeof(ip));
            fprintf(stderr, "%s ==> %s\n", ip, buffer);
        }
    }

    deinitialize();
    return 0;
}