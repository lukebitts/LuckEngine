#ifndef ENTERFRAMEEVENT_H
#define ENTERFRAMEEVENT_H
#include "event.h"
namespace luck { namespace event
{
    struct EnterFrameEvent : public Event
    {
        f64 currentFrame;
        f64 deltaTime;
        ~EnterFrameEvent(){}
    };
}}
#endif

