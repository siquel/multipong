#include "packets.h"
#include <stdlib.h> // malloc
#include <jkn/error.h> // JKN_ASSERT
#include "serialization.h"
#include "endian.h"

namespace common
{
    template <typename Stream>
    struct SerializeFunc
    {
        typedef bool(*type)(Stream&, const Memory&);
    };

    struct PacketData
    {
        SerializeFunc<ReadStream>::type read;
        SerializeFunc<WriteStream>::type write;
        size_t size;
    };

    template <typename Stream>
    bool serializeUsernamePacket(Stream& _stream, const Memory& _from) 
    { 
        UsernamePacket& packet = *(UsernamePacket*)_from.ptr;

        serialize_string(_stream, packet.m_username, MaxUsernameLength);

        return true; 
    }



#define IMPLEMENT_PACKET(packet) { serialize##packet<ReadStream>, serialize##packet<WriteStream>, sizeof(packet) }


    static PacketData s_packetData[] =
    {
        IMPLEMENT_PACKET(UsernamePacket),
    };

    template <>
    bool serialize<ReadStream>(ReadStream& _stream, PacketType::Enum packetType, const Memory& _mem)
    {
        return s_packetData[packetType].read(_stream, _mem);
    }

    template <>
    bool serialize<WriteStream>(WriteStream& _stream, PacketType::Enum _packetType, const Memory& _mem)
    {
        return s_packetData[_packetType].write(_stream, _mem);
    }

    bool packetCreate(PacketType::Enum _type, Memory& _to)
    {
        size_t mem = s_packetData[_type].size;

        JKN_ASSERT(mem > 0, "Size must be greater that 0");

        _to.size = mem;
        _to.ptr = ::malloc(mem);
        memset(_to.ptr, 0, _to.size);

        return _to.ptr != NULL;
    }

    void packetDestroy(Memory& _from)
    {
        ::free(_from.ptr);
        _from.size = 0;
    }

    int32_t packetProcessOutgoing(uint32_t _protocolId, PacketType::Enum _type, const Memory& _packet, uint8_t* _buffer, uint32_t _bufferSize, uint32_t& _streamSize)
    {
        WriteStream stream(_buffer, _bufferSize);

        if (!stream.serializeBits(hostToNetwork(_protocolId), 32))
        {
            return -1;
        }

        int32_t packetType = int32_t(_type);

        if (!stream.serializeInteger(packetType, 0, common::PacketType::Count - 1))
        {
            return -1;
        }

        if (!serialize(stream, _type, _packet))
        {
            return -1;
        }

        stream.flush();

        if (stream.isError())
        {
            return 1;
        }

        _streamSize = stream.getBytesProcessed();

        return 0;
    }

    int32_t packetProcessIncomingBuffer(uint32_t _protocolId, 
        const uint8_t* _buffer, uint32_t _bytes, 
        Memory& _packet, PacketType::Enum& _packetType)
    {
        ReadStream stream(_buffer, _bytes);

        uint32_t protocol = 0;

        if (!stream.serializeBits(protocol, 32))
        {
            return -1;
        }

        if (_protocolId != networkToHost(protocol))
        {
            return -1;
        }

        int32_t packetType;

        if (!stream.serializeInteger(packetType, 0, PacketType::Count - 1))
        {
            return -1;
        }

        _packetType = PacketType::Enum(packetType);

        if (!packetCreate(common::PacketType::Enum(packetType), _packet))
        {
            return -1;
        }

        if (!serialize(stream, PacketType::Enum(packetType), _packet))
        {
            return -1;
        }

        return 0;
    }
}

