#include <stdio.h>
#include <jkn/net/socket.h>
#include <jkn/thread/thread.h>
#include <jkn/net/ip_address.h>
#include <jkn/jkn.h>
#include "server.h"
#include "event_queue.h"

namespace pong
{
    int32_t serverProc(void*)
    {
        pong::Server server;

        server.start();

        return EXIT_SUCCESS;
    }

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

    struct Context
    {
        int32_t run()
        {
            initialize();

            jkn::Thread serverThread;
            jkn::Thread manageThread;

            serverThread.start(serverProc);

            char buffer[256];

            while (fgets(buffer, sizeof(buffer), stdin) != NULL)
            {
                // replace new line
                buffer[strcspn(buffer, "\n")] = '\0';

                if (strcmp(buffer, "exit") == 0)
                {
                    m_eventQueue.pushExitEvent();
                    break;
                }
            }

            serverThread.join();

            deinitialize();

            return EXIT_SUCCESS;
        }

        pong::EventQueue m_eventQueue;
    };

    static Context s_ctx;

    bool nextEvent(pong::Event& _ev)
    {
        return s_ctx.m_eventQueue.pop(_ev);
    }
}
int main(int, char**)
{
    using namespace pong;
    return s_ctx.run();
}
