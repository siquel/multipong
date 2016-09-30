#include "packets.h"
#include <stdlib.h> // malloc
#include <jkn/error.h> // JKN_ASSERT
#include "serialization.h"
#include "network_endian.h"
namespace common
{
    void* alignTop(void* _ptr, uint32_t _align)
    {
        uintptr_t ptr = (uintptr_t)_ptr;
        uint32_t mod = ptr % _align;

        if (mod) ptr += _align - mod;

        return (void*)ptr;
    }

    struct LinearAllocator
    {
        LinearAllocator(uint32_t _size) :
            m_offset(0),
            m_size(_size)
        {
            m_start = malloc(_size);
        }

        ~LinearAllocator()
        {
            free(m_start);
        }

        void* allocate(uint32_t _size, uint32_t _align = 4)
        {
            uint32_t size = _size + _align;

            if (m_offset + size > m_size) return NULL;

            void* ptr = alignTop((char*)m_start + m_offset, _align);
            m_offset += size;
            return ptr;
        }

        void deallocate(void*)
        {
            // not supported
        }

        void clear()
        {
            m_offset = 0;
        }

        void* m_start;
        uint32_t m_offset;
        uint32_t m_size;
    };

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

    static LinearAllocator s_allocator(1024 * 1024);
    static bool s_hasBegun = false;
    static uint32_t numAllocs = 0;
    static uint32_t numDeallocs = 0;

    void packetBegin()
    {
        JKN_ASSERT(!s_hasBegun, "Already called packetBegin()");
        JKN_ASSERT(numAllocs == 0, "Can not allocate before begin");
        JKN_ASSERT(numDeallocs == 0, "Can not deallocate before begin");
        numAllocs = 0;
        numDeallocs = 0;
        s_hasBegun = true;
    }

    bool packetCreate(PacketType::Enum _type, Memory& _to)
    {
        JKN_ASSERT(s_hasBegun, "Allocator hasnt begun");
        size_t mem = s_packetData[_type].size;

        JKN_ASSERT(mem > 0, "Size must be greater that 0");

        _to.size = mem;
        _to.ptr = s_allocator.allocate(mem);
        memset(_to.ptr, 0, _to.size);

        bool result = _to.ptr != NULL;

        if (result) ++numAllocs;

        return result;
    }

    void packetDestroy(Memory& _from)
    {
        JKN_ASSERT(s_hasBegun, "Allocator hasnt begun");
        s_allocator.deallocate(_from.ptr);
        _from.size = 0;
        ++numDeallocs;
    }

    void packetEnd()
    {
        JKN_ASSERT(s_hasBegun, "packetEnd() must be called first");
        JKN_ASSERT(numAllocs == numDeallocs, "Invalid amount of deallocs");
        numAllocs = 0;
        numDeallocs = 0;
        s_hasBegun = false;
        s_allocator.clear();
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
            return ProcessingErrorType::NoProtocol;
        }

        if (_protocolId != networkToHost(protocol))
        {
            return ProcessingErrorType::ProtocolMismatch;
        }

        int32_t packetType;

        if (!stream.serializeInteger(packetType, 0, PacketType::Count - 1))
        {
            return ProcessingErrorType::InvalidPacketType;
        }

        _packetType = PacketType::Enum(packetType);

        if (!packetCreate(common::PacketType::Enum(packetType), _packet))
        {
            return ProcessingErrorType::PacketAllocationFailed;
        }

        if (!serialize(stream, PacketType::Enum(packetType), _packet))
        {
            return ProcessingErrorType::PacketSerializationFailed;
        }

        return ProcessingErrorType::NoError;
    }
}

