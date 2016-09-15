#include <common/multipong_p.h>
#include <stdio.h>
#include <jkn/thread/thread.h>

int32_t func(void*)
{
    return 0;
}

int main(int, char**)
{
    jkn::Thread thread;
    thread.start(func);
    printf("hello from client\n");

    thread.join();

    return 0;
}