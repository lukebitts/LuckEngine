#ifndef KEYEVENT_H
#define KEYEVENT_H
#include "event.h"
namespace luck { namespace event
{
    struct KeyEvent : public Event
    {
        static const u8 KEYDOWN = 1;
        static const u8 KEYUP = 0;
        s32 keyCode;
        bool action;
        KeyEvent(s32 keyCode, bool action)
        {
            this->keyCode = keyCode;
            this->action = action;
        }
        ~KeyEvent(){}
    };
}}
#endif
