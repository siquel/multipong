#pragma once

#include <common/circular_queue.h>

namespace pong
{
    struct EventType
    {
        enum Enum
        {
            Exit,
            Count
        };
    };

    union Event
    {
        uint16_t type;
    };

    const uint32_t MaxEvents = 1024u;

    class EventQueue
    {
    public:
        void pushExitEvent()
        {
            Event ev;
            ev.type = EventType::Exit;

            m_queue.push(ev);
        }

        bool pop(Event& ev)
        {
            return m_queue.pop(ev);
        }

        bool push(Event& ev)
        {
            return m_queue.push(ev);
        }

    private:
        common::SpScCircularQueue<Event, MaxEvents> m_queue;
    };
}