#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H
#include "event.h"
#include "Vector2.h"
namespace luck { namespace event
{
    struct MouseEvent : public Event
    {
        core::Vector2<u16> position;
        s32 button;
        bool action;
        s32 wheelPosition;
        MouseEvent(core::Vector2<u16> position, s32 button, bool action, s32 wheelPosition)
        {
            this->position = position;
            this->button = button;
            this->action = action;
            this->wheelPosition = wheelPosition;
        }
        ~MouseEvent(){}
    };
}}
#endif

