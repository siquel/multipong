#include "network.h"
#include <jkn/platform.h>
#if JKN_PLATFORM_WINDOWS
#   define NOMINMAX
#   define _WINSOCK_DEPRECATED_NO_WARNINGS
#   include <winsock2.h>
#   include <stdio.h> // sprintf
#   include <jkn/debug.h> // debugOutput
#endif

#include <jkn/error.h> // JKN_ASSERT

namespace common
{
    static bool s_networkInitialized = false;

    bool networkInitialize()
    {
        JKN_ASSERT(!s_networkInitialized, "Network already initialized");

        s_networkInitialized = true;

#if JKN_PLATFORM_WINDOWS
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            char buf[64];
            sprintf(buf, "Failed to initialize winsock, error %d\n", WSAGetLastError());
            jkn::debugOutput(buf);
            s_networkInitialized = false;
        }
#endif
        return s_networkInitialized;
    }

    bool networkInitialized()
    {
        return s_networkInitialized;
    }

    void networkShutdown()
    {
        JKN_ASSERT(s_networkInitialized, "Network not initialized");

#if JKN_PLATFORM_WINDOWS
        WSACleanup();
#endif
    }
}

