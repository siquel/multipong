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
    addr.m_type = jkn::IPAddressType::IPv4;
    addr.m_ipv4 = uint32_t(127) | (uint32_t(0) << 8) | (uint32_t(0) << 16) | (uint32_t(1) << 24);

    jkn::UDPSocket socket(addr);

    printf("Starting server on ip %d.%d.%d.%d : %d\n",
        (addr.m_ipv4 & 0xff),
        (addr.m_ipv4 >> 8) & 0xff,
        (addr.m_ipv4 >> 16) & 0xff,
        (addr.m_ipv4 >> 24) & 0xff,
        addr.m_port);

    deinitialize();
    return 0;
}