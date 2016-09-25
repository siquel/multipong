#pragma once

#include <stdint.h> // uint32_t
#include <jkn/platform.h>
#if JKN_PLATFORM_WINDOWS
#   include <windows.h>
#endif

namespace common
{
#if JKN_PLATFORM_WINDOWS
    struct AtomicInt
    {
        mutable LONG m_val;

        AtomicInt(int val)
        {
            store(val);
        }

        int load() const
        {
            InterlockedExchangeAdd(&m_val, int32_t(0));
            return m_val;
        }

        void store(int val)
        {
            InterlockedExchange(&m_val, val);
        }
    };
#elif JKN_PLATFORM_LINUX
    struct AtomicInt
    {
        mutable int m_val;
        
        AtomicInt(int val)
        {
            store(val);
        }

        int load() const
        {
            __sync_fetch_and_add(&m_val, 0);
            return m_val;
        }

        void store(int val)
        {
            __sync_fetch_and_add(&m_val, val);
        }
    };
#endif

    // lock free single producer single consumer queue
    template <typename Ty, uint32_t Capacity>
    class SpScCircularQueue
    {
    public:
        SpScCircularQueue() :
            m_tail(0),
            m_head(0)
        {

        }
        // producer only
        bool push(const Ty& item)
        {
            const int32_t currentTail = m_tail.load();
            const int32_t nextTail = increment(currentTail);
            // compare tail to snapshot of head
            if (nextTail != m_head.load())
            {
                // store item
                m_array[currentTail] = item;
                // update tail index
                m_tail.store(nextTail);

                return true;
            }
            // full queue
            return false;
        }

        // consumer only
        // update head index after retrieving element
        bool pop(Ty& item)
        {
            // get head
            const int32_t currentHead = m_head.load();
            // compare head to snapshot of tail
            if (currentHead == m_tail.load())
            {
                // empty queue
                return false;
            }

            item = m_array[currentHead];
            // update head index
            m_head.store(increment(currentHead));
            return true;
        }
    private:
        int32_t increment(int32_t idx) const
        {
            return (idx + 1) % Capacity;
        }

        Ty m_array[Capacity];
        AtomicInt m_tail;
        AtomicInt m_head;
    };
}
