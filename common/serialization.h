#pragma once

#include <stdint.h> // uint32_t
#include "bit_writer.h"
#include "bit_reader.h"
#include <jkn/platform.h>
#include <string.h> // strlen
#include <jkn/error.h> //JKN_ASSERT

#if JKN_COMPILER_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4127) // conditional expression is constant (templates)
#endif

namespace common
{

    inline uint32_t popcount(uint32_t x)
    {
#if JKN_COMPILER_MSVC
        const uint32_t a = x - ((x >> 1) & 0x55555555);
        const uint32_t b = (((a >> 2) & 0x33333333) + (a & 0x33333333));
        const uint32_t c = (((b >> 4) + b) & 0x0f0f0f0f);
        const uint32_t d = c + (c >> 8);
        const uint32_t e = d + (d >> 16);
        const uint32_t result = e & 0x0000003f;
        return result;
#else
        return __builtin_popcount(x);
#endif
    }

#if JKN_COMPILER_MSVC
    inline uint32_t log2(uint32_t x)
    {
        const uint32_t a = x | (x >> 1);
        const uint32_t b = a | (a >> 2);
        const uint32_t c = b | (b >> 4);
        const uint32_t d = c | (c >> 8);
        const uint32_t e = d | (d >> 16);
        const uint32_t f = e >> 1;
        return popcount(f);
    }

    inline int bitsRequired(uint32_t min, uint32_t max)
    {
        return (min == max) ? 0 : log2(max - min) + 1;
    }
#else
    inline int bitsRequired(uint32_t min, uint32_t max)
    {
        return 32 - __builtin_clz(max - min);
    }
#endif

    struct StreamError
    {
        enum Enum
        {
            NoError,
            Overflow
        };
    };

    class WriteStream
    {
    public:
        enum { IsWriting = 1 };
        enum { IsReading = 0 };

        WriteStream(uint8_t* buffer, uint32_t bytes);

        bool serializeInteger(int32_t value, int32_t min, int32_t max);

        bool serializeBits(uint32_t value, uint32_t bits);

        bool serializeBytes(const uint8_t* data, uint32_t bytes);

        bool serializeAlign();

        void flush();

        const uint8_t* getData() const;

        uint32_t getBytesProcessed() const;

        uint32_t getBitsProcessed() const;

        uint32_t getBitsRemaining() const;

        uint32_t getTotalBits() const;

        uint32_t getTotalBytes() const;

        bool isError() const;
    private:
        BitWriter m_writer;
        StreamError::Enum m_error;
    };

    class ReadStream
    {
    public:
        enum { IsWriting = 0 };
        enum { IsReading = 1 };

        ReadStream(const uint8_t* buffer, uint32_t bytes);

        // deserialize integer to value
        // return true if succeeded
        bool serializeInteger(int32_t& value, int32_t min, int32_t max);

        // deserialize bits to value
        // return true if succeeded
        bool serializeBits(uint32_t& value, uint32_t bits);

        // deserialize bytes to data
        // return true if succeeded
        bool serializeBytes(uint8_t* data, uint32_t bytes);

        // align reader to boundary
        // returns true if read data was align
        bool serializeAlign();

        uint32_t getBitsProcessed() const;

        uint32_t getBitsRemaining() const;

        uint32_t getBytesProcessed() const;

        uint32_t getBytesRead() const;

        bool isError() const;
    private:
        BitReader m_reader;
        uint32_t m_bitsRead;
        StreamError::Enum m_error;
    };


#define serialize_bits(stream, value, bits)                                                             \
    for (;;) {                                                                                          \
        JKN_ASSERT(bits > 0, "Bits needs to be more than 0");                                           \
        JKN_ASSERT(bits <= 32, "Bits cant be more than 32");                                            \
        uint32_t uv;                                                                                    \
        if (Stream::IsWriting)                                                                          \
        {                                                                                               \
            uv = (uint32_t)value;                                                                       \
        }                                                                                               \
        if (!stream.serializeBits(uv, bits))                                                            \
        {                                                                                               \
            return false;                                                                               \
        }                                                                                               \
        if (Stream::IsReading)                                                                          \
        {                                                                                               \
            value = uv;                                                                                 \
        }                                                                                               \
    break; }


#define serialize_bytes(stream, data, bytes)                                              \
    for (;;) {                                                                                  \
        if (!stream.serializeBytes(data, bytes)) return false;                            \
    break; }

#define serialize_int32_range(stream, value, min, max)                                    \
    for (;;) {                                                                                  \
        JKN_ASSERT(min < max, "min must be less than max");                               \
        int32_t ivalue;                                                                   \
        if (Stream::IsWriting) {                                                          \
            JKN_ASSERT(int64_t(value) >= int64_t(min), "value must be more than min");    \
            JKN_ASSERT(int64_t(value) <= int64_t(max), "value must be less than max");    \
            ivalue = (int32_t)value;                                                      \
        }                                                                                 \
                                                                                          \
        if (!stream.serializeInteger(ivalue, min, max)) return false;                     \
                                                                                          \
        if (Stream::IsReading) {                                                          \
            value = ivalue;                                                               \
            if (value < min || value > max) return false;                                 \
        }                                                                                 \
    break; }

    template <typename Stream>
    inline bool serializeString(Stream& stream, char* string, uint32_t bufferSize)
    {
        uint32_t len;
        if (Stream::IsWriting)
        {
            len = uint32_t(strlen(string));
            JKN_ASSERT(len < (bufferSize - 1), "Out of bounds");
        }
        serialize_int32_range(stream, len, 0, bufferSize - 1);
        serialize_bytes(stream, (uint8_t*)string, len);

        if (Stream::IsReading)
        {
            string[len] = '\0';
        }

        return true;
    }

    template <typename Stream>
    inline bool serializeUint64(Stream& stream, uint64_t& value)
    {
        uint32_t hi, lo;
        if (Stream::IsWriting)
        {
            lo = value & 0xffffffff;
            hi = value >> 32;
        }
        serialize_bits(stream, lo, 32);
        serialize_bits(stream, hi, 32);

        if (Stream::IsReading)
        {
            value = (uint64_t(hi) << 32) | lo;
        }
        return true;
    }

#define serialize_string(stream, buffer, size)                                          \
    for (;;) {                                                                          \
        if (!serializeString(stream, buffer, size)) return false;                       \
    break; }
}

#define serialize_uint64(stream, value)                                                 \
    for (;;) {                                                                          \
        if (!serializeUint64(stream, value)) return false;                              \
    break; }

#if JKN_COMPILER_MSVC
#   pragma warning(pop)
#endif