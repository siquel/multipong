#include <stdio.h>
#include <jkn/net/socket.h>
#include <jkn/thread/thread.h>
#include <jkn/net/ip_address.h>
#include <jkn/jkn.h>
#include "server.h"
#include "event_queue.h"
#include <common/network.h> // network
#include <string.h> // strcmp, strcspn
namespace pong
{
    int32_t serverProc(void*)
    {
        pong::Server server;

        server.start();

        return EXIT_SUCCESS;
    }

    struct Context
    {
        int32_t run()
        {
            bool result = common::networkInitialize();

            JKN_ASSERT(result, "Network initialization failed");

            if (!result)
            {
                return EXIT_FAILURE;
            }

            jkn::Thread serverThread;

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

            common::networkShutdown();

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
