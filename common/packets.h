#pragma once

#include <stdint.h> // uint32_t

namespace common
{
    const static uint32_t MaxUsernameLength = 32;

    struct UsernamePacket
    {
        char m_username[MaxUsernameLength];
    };
}