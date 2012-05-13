#ifndef EVENT_H
#define EVENT_H
namespace luck { namespace event
{
    class EventDispatcher;

    struct Event //basic event layout
    {
        /// @todo remove the text property after adding a LoadProgress event
        EventDispatcher* from;
        string text;
        Event(){}
        Event(string text) : text(text) {}
        virtual ~Event(){}
    };
}}
#endif
