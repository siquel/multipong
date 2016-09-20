#pragma once

#include <jkn/platform.h>
#include <stdint.h> // uint

namespace common
{
    /*
    inline uint16_t endianSwap(uint16_t _in)
    {
        return (_in >> 8) | (_in << 8);
    }
    */
    
    // Network code is in little endian, convert value to little endian if host is big endian
    template <typename T>
    inline T hostToNetwork(T value)
    {
#if JKN_CPU_BIG_ENDIAN
#   error "not implemented"
#else
        return value;
#endif
    }


    // Network code is in little endian, convert it to big endian if host is big endian
    template <typename T>
    inline T networkToHost(T value)
    {
#if JKN_CPU_BIG_ENDIAN
#   error "not implemented"
#else
        return value;
#endif
    }
}

