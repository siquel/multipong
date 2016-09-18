#include <stdio.h>
#include <jkn/thread/thread.h>
#include <jkn/net/ip_address.h>
#include <jkn/net/socket.h>
#include <jkn/os.h>

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
    jkn::IPAddress serverAddress;
    jkn::addressSetHost(serverAddress, 127, 0, 0, 1);
    serverAddress.m_port = 1337;


    jkn::IPAddress clientAddress;
    jkn::addressSetHost(clientAddress, INADDR_ANY);
    clientAddress.m_port = 0;
    jkn::UDPSocket sock(clientAddress);

    while (true)
    {
        char buffer[] = "Hello World!";
        
        sock.send(serverAddress, buffer, sizeof(buffer));
        jkn::sleep(1000);
    }
    
    deinitialize();
    return 0;
}