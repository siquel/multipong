#include "packets.h"
#include <stdlib.h> // malloc
#include <jkn/error.h> // JKN_ASSERT
#include "serialization.h"

namespace common
{
    template <typename Stream>
    struct SerializeFunc
    {
        typedef bool(*type)(Stream&, Memory&, void*);
    };

    struct PacketData
    {
        SerializeFunc<ReadStream>::type read;
        SerializeFunc<WriteStream>::type write;
        size_t size;
    };

    template <typename Stream>
    bool serializeUsernamePacket(Stream& _stream, Memory& _from, void* _to) 
    { 
        UsernamePacket& packet = *(UsernamePacket*)_to;

        if (Stream::IsReading)
        {
            
        }

        return false; 
    }



#define IMPLEMENT_PACKET(packet) { serialize##packet<ReadStream>, serialize##packet<WriteStream>, sizeof(packet) }


    static PacketData s_packetData[] =
    {
        IMPLEMENT_PACKET(UsernamePacket),
    };


    void packetCreate(PacketType::Enum _type, Memory& _to)
    {
        size_t mem = s_packetData[_type].size;

        JKN_ASSERT(mem > 0, "Size must be greater that 0");

        _to.size = mem;
        _to.ptr = ::malloc(mem);
    }

    void packetDestroy(Memory& _from)
    {
        ::free(_from.ptr);
        _from.size = 0;
    }
}

