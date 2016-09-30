#pragma once

#include <stdint.h>
#include <jkn/platform.h>

namespace pong
{
    uint64_t murmur_hash_64(const void * key, uint32_t length, uint64_t seed)
    {
        const uint64_t m = 0xc6a4a7935bd1e995ULL;
        const uint32_t r = 47;

        uint64_t h = seed ^ (length * m);

        const uint64_t * data = (const uint64_t*)key;
        const uint64_t * end = data + length / 8;

        while (data != end)
        {
#if JKN_CPU_LITTLE_ENDIAN
            uint64_t k = *data++;
#else
            uint64_t k = *data++;
            uint8_t * p = (uint8_t*)&k;
            uint8_t c;
            c = p[0]; p[0] = p[7]; p[7] = c;
            c = p[1]; p[1] = p[6]; p[6] = c;
            c = p[2]; p[2] = p[5]; p[5] = c;
            c = p[3]; p[3] = p[4]; p[4] = c;
#endif

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const uint8_t * data2 = (const uint8_t*)data;

        switch (length & 7)
        {
        case 7: h ^= uint64_t(data2[6]) << 48;
        case 6: h ^= uint64_t(data2[5]) << 40;
        case 5: h ^= uint64_t(data2[4]) << 32;
        case 4: h ^= uint64_t(data2[3]) << 24;
        case 3: h ^= uint64_t(data2[2]) << 16;
        case 2: h ^= uint64_t(data2[1]) << 8;
        case 1: h ^= uint64_t(data2[0]);
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }
}

