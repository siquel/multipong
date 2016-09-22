#pragma once

#include <stdint.h> // uint32_t

namespace common
{
    struct PacketType
    {
        enum Enum
        {
            UsernamePacket,

            Count
        };
    };

    struct Memory
    {
        void* ptr;
        size_t size;
    };

    const static uint32_t MaxUsernameLength = 32;

    struct UsernamePacket
    {
        char m_username[MaxUsernameLength];
    };
}