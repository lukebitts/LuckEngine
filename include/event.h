#ifndef EVENT_H
#define EVENT_H
namespace luck { namespace event
{
    class EventDispatcher;

    struct Event //basic event layout
    {
        Event(){}
        virtual ~Event(){}
    };
}}
#endif
