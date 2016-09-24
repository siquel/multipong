#pragma once

#include <stdint.h> // uint32_t

namespace common
{
    struct PacketType
    {
        enum Enum
        {
            // Client -> Server
            UsernamePacket, // The client connects to the server, sending in the username
            // Server -> Client
            RandomNumberPacket, // The server responds by sending out unique random number
            Count
        };
    };

    struct Memory
    {
        void* ptr;
        size_t size;
    };


    bool packetCreate(PacketType::Enum packetType, Memory& _to);

    void packetDestroy(Memory& _packet);

    template <typename Stream>
    bool serialize(Stream& _stream, PacketType::Enum packetType, const Memory& _mem);

    const static uint32_t MaxUsernameLength = 32;

    struct UsernamePacket
    {
        char m_username[MaxUsernameLength];
    };
}