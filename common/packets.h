#pragma once

#include <stdint.h> // uint32_t
#include <stddef.h> // size_t

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

    struct ProcessingErrorType
    {
        enum Enum
        {
            NoError,
            NoProtocol,
            ProtocolMismatch,
            InvalidPacketType,
            PacketAllocationFailed,
            PacketSerializationFailed
        };
    };

    struct Memory
    {
        void* ptr;
        size_t size;
    };

    void packetBegin();

    bool packetCreate(PacketType::Enum packetType, Memory& _to);

    void packetDestroy(Memory& _packet);

    void packetEnd();

    int32_t packetProcessOutgoing(uint32_t _protocolId, PacketType::Enum _type, const Memory& _packet, uint8_t* _buffer, uint32_t _bufferSize, uint32_t& _streamSize);
    int32_t packetProcessIncomingBuffer(uint32_t _protocolId, 
        const uint8_t* _buffer, uint32_t _bytes, 
        Memory& packet, PacketType::Enum& _packetType);

    template <typename Stream>
    bool serialize(Stream& _stream, PacketType::Enum packetType, const Memory& _mem);

    const static uint32_t MaxUsernameLength = 32;

    struct UsernamePacket
    {
        char m_username[MaxUsernameLength];
    };
}
