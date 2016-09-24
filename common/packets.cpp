#include "packets.h"
#include <stdlib.h> // malloc
#include <jkn/error.h> // JKN_ASSERT
#include "serialization.h"

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
}

