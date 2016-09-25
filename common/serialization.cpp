#include "serialization.h"
#include <jkn/error.h>

namespace common
{
    //////////////////////////////////////////////////////////////////////////
    // write stream
    //////////////////////////////////////////////////////////////////////////

    WriteStream::WriteStream(uint8_t* buffer, uint32_t bytes) : 
        m_writer(buffer, bytes),
        m_error(StreamError::NoError)
    {

    }

    bool WriteStream::serializeInteger(int32_t value, int32_t min, int32_t max)
    {
        JKN_ASSERT(min < max, "Minimium needs to be smaller than max");
        JKN_ASSERT(value >= min, "Value needs to be higher than min");
        JKN_ASSERT(value <= max, "Value needs to be smaller than max");

        const int bits = bitsRequired(min, max);
        uint32_t unsignedValue = value - min;
        m_writer.writeBits(unsignedValue, bits);
        return true;
    }

    bool WriteStream::serializeBits(uint32_t value, uint32_t bits)
    {
        JKN_ASSERT(bits <= 32, "Max 32 bits (got %d)", bits);
        m_writer.writeBits(value, bits);
        return true;
    }

    bool WriteStream::serializeBytes(const uint8_t* data, uint32_t bytes)
    {
        JKN_ASSERT(data != NULL, "Data can't be nullptr");
        // align to byte boundary
        m_writer.writeAlign();
        m_writer.writeBytes(data, bytes);
        return true;
    }

    bool WriteStream::serializeAlign()
    {
        m_writer.writeAlign();
        return true;
    }

    void WriteStream::flush()
    {
        m_writer.flushBits();
    }

    const uint8_t* WriteStream::getData() const
    {
        return m_writer.getData();
    }

    uint32_t WriteStream::getBytesProcessed() const
    {
        return m_writer.getBytesWritten();
    }

    uint32_t WriteStream::getBitsProcessed() const
    {
        return m_writer.getBitsWritten();
    }

    uint32_t WriteStream::getBitsRemaining() const
    {
        return (m_writer.getTotalBytes() * 8) - m_writer.getBitsWritten();
    }

    uint32_t WriteStream::getTotalBits() const
    {
        return m_writer.getTotalBytes() * 8;
    }

    uint32_t WriteStream::getTotalBytes() const
    {
        return m_writer.getTotalBytes();
    }

    bool WriteStream::isError() const
    {
        return m_error != StreamError::NoError;
    }

    //////////////////////////////////////////////////////////////////////////
    // read stream
    //////////////////////////////////////////////////////////////////////////

    ReadStream::ReadStream(const uint8_t* buffer, uint32_t bytes) : 
        m_reader(buffer, bytes),
        m_bitsRead(0),
        m_error(StreamError::NoError)
    {

    }

    bool ReadStream::serializeInteger(int32_t & value, int32_t min, int32_t max)
    {
        JKN_ASSERT(min < max, "min needs to be less than max");
        const int bits = bitsRequired(min, max);

        if (m_reader.wouldOverflow(bits))
        {
            m_error = StreamError::Overflow;
            return false;
        }

        uint32_t unsignedValue = m_reader.readBits(bits);
        value = (int32_t)unsignedValue + min;
        m_bitsRead += bits;
        return true;
    }

    bool ReadStream::serializeBits(uint32_t & value, uint32_t bits)
    {
        JKN_ASSERT(bits <= 32, "bits needs to be less than 32");

        if (m_reader.wouldOverflow(bits))
        {
            m_error = StreamError::Overflow;
            return false;
        }

        value = m_reader.readBits(bits);
        m_bitsRead += bits;

        return true;
    }

    bool ReadStream::serializeBytes(uint8_t * data, uint32_t bytes)
    {
        if (!serializeAlign())
        {
            return false;
        }

        if (m_reader.wouldOverflow(bytes * 8))
        {
            m_error = StreamError::Overflow;
            return false;
        }
        m_reader.readBytes(data, bytes);
        m_bitsRead += bytes * 8;
        return true;
    }

    bool ReadStream::serializeAlign()
    {
        const uint32_t align = m_reader.getAlignBits();
        if (m_reader.wouldOverflow(align))
        {
            m_error = StreamError::Overflow;
            return false;
        }

        if (!m_reader.readAlign())
        {
            return false;
        }
        m_bitsRead += align;
        return true;
    }

    uint32_t ReadStream::getBitsProcessed() const
    {
        return m_bitsRead;
    }

    uint32_t ReadStream::getBitsRemaining() const
    {
        return m_reader.getBitsRemaining();
    }

    uint32_t ReadStream::getBytesProcessed() const
    {
        return (m_bitsRead + 7) / 8;
    }

    uint32_t ReadStream::getBytesRead() const
    {
        return m_reader.getBytesRead();
    }
    
    bool ReadStream::isError() const
    {
        return m_error != StreamError::NoError;
    }
}

